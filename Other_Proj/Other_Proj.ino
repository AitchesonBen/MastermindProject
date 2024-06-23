#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Wire.h>

#define PIN 6

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(4, 6, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

#define ST_SELECT_COLOUR 1
#define ST_GUESS_COLOUR 2
#define ST_SEND_COLOURS 3
int machine_state;

int potPin = A0;                  // potentiometer control
int potVal = 0;
int buttonPin = 2;                // button control
int buttonPress = 0;
int column = 0;                   // coordinates for led matrix
int row = 0;
uint32_t playerGuess[4] = {};     // store players colour guesses
uint32_t playerColourGuess;

int numbers[4];
uint32_t mainArray[20][4] = {};
int capLimit;

int difficulty = 0;

bool dr = false;
bool buttonPressed = false;

void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.setBrightness(25);
  pinMode(buttonPin, INPUT_PULLUP);
  Wire.begin(8);                 // join I2C bus with address #8
  Wire.onReceive(Receive_Event); // register event
  matrix.clear();
  matrix.show();

  machine_state = ST_SELECT_COLOUR;
}

uint32_t RED = 0xf800;
uint32_t BLUE = 0x001f;
uint32_t YELLOW = 0xffe0;
uint32_t GREEN = 0x07e0;
uint32_t ORANGE = 0xfbe0;
uint32_t PINK = 0xf81f;

void loop() {
  potVal = analogRead(potPin);
  buttonPress = digitalRead(buttonPin);
  if (dr) {
    State_Stuff();
    State_Transition();
    delay(100);
  }
}

void pickArray(int x) {
  if (x == 3) {
    capLimit = 5;
  } else if (x == 4){
    capLimit = 3;
  } else if (x == 5){
    capLimit = 1;
  }
}

void shiftDown() {
  for (int i = 19; i > 0; i--) {
    for (int j = 0; j < 4; j++) {
      mainArray[i][j] = mainArray[i-1][j];
    }
  }
}

void intoArray() {
  for (int i = 0; i < 4; i++) {
    mainArray[0][i] = playerGuess[i];
  }
}

void callArray() {
  for (int i = 0; i < capLimit; i++) {
    for (int j = 0; j < 4; j++) {
      matrix.writePixel(j, i + 1, mainArray[i][j]);
    }
  }
}

void pushGuess() {
  shiftDown();
  intoArray();
  matrix.clear();
  callArray();
}

void State_Stuff() {
  switch(machine_state) {
    case ST_SELECT_COLOUR:
      Select_Colour(potVal, column, 0);
      matrix.show();
      break;
    case ST_GUESS_COLOUR:
      playerGuess[column] = playerColourGuess;
      matrix.writePixel(column, 0, playerGuess[column]);
      column += 1;
      delay(500);
      break;

    case ST_SEND_COLOURS:
      Wire.onRequest(Request_Event);
      break;

    default:
      Serial.println("something broke");
      break;
  }
}

void State_Transition(){
  buttonPress = digitalRead(buttonPin);
  switch(machine_state) {
    case ST_SELECT_COLOUR:
      if (buttonPress == 0) {
        machine_state = ST_GUESS_COLOUR;
      }
      break;
    case ST_GUESS_COLOUR:
      if (buttonPress == 1) {
        buttonPressed = true;
      } else {
        buttonPressed = false;
      }
      if (buttonPressed) {
        machine_state = ST_SELECT_COLOUR;
        buttonPressed = false;
      }
      if (column == 4) {
        pushGuess();
        column = 0;
        machine_state = ST_SEND_COLOURS;
      }
      break;

    case ST_SEND_COLOURS:
      machine_state = ST_SELECT_COLOUR;
      break;

    default:
      Serial.println("something broke");
      break;
  }
}

int Numbers_Colours(uint32_t color) {
  if (color == RED) return 1;
  if (color == BLUE) return 2;
  if (color == YELLOW) return 3;
  if (color == GREEN) return 4;
  if (color == ORANGE) return 5;
  if (color == PINK) return 6;
  return -1;
}

void Translate_Colours() {
  for (int i = 0; i < 4; i++) {
    numbers[i] = Numbers_Colours(playerGuess[i]);
  }
}

void Select_Colour(int potVal, int currentColumn, int currentRow) {
  switch(potVal) {
    case 0 ... 171:
      matrix.writePixel(currentColumn, currentRow, RED);
      playerColourGuess = RED;
      break;
    
    case 172 ... 342:
      matrix.writePixel(currentColumn, currentRow, BLUE);
      playerColourGuess = BLUE;
      break;

    case 343 ... 513:
      matrix.writePixel(currentColumn, currentRow, YELLOW);
      playerColourGuess = YELLOW;
      break;

    case 514 ... 684:
      matrix.writePixel(currentColumn, currentRow, GREEN);
      playerColourGuess = GREEN;
      break;

    case 685 ... 855:
      matrix.writePixel(currentColumn, currentRow, ORANGE);
      playerColourGuess = ORANGE;
      break;

    case 856 ... 1023:
      playerColourGuess = PINK;
      matrix.writePixel(currentColumn, currentRow, PINK);
      break;

    default:
      Serial.println("OUT OF RANGE");
      break;
  }
}

void Receive_Event(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read();        // receive byte as a character
    Serial.print(c);             // print the character
  }
  int x = Wire.read();           // receive byte as an integer
  if (x == 9) {
    dr = false;
    matrix.clear();
  } else {
    difficulty = x;
    pickArray(x);
    dr = true;
  }
  Serial.println(x);             // print the integer
}

void Request_Event() {
  Translate_Colours();
  if (dr) {
    for (int i = 0; i < 4; i++) {
      Wire.write(numbers[i]);
      Serial.println(numbers[i]);
    }
  }
}