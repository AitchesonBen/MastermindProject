#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#define PIN 6
#define NUMPIXELS 9

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define ST_SELECT_COLOUR 1
#define ST_GUESS_COLOUR 2
#define ST_SEND_COLOURS 3
int machine_state;

int potPin = A0;                  // potentiometer control
int potVal = 0;
int buttonPin = 2;                // button control
int buttonPress = 0;
int led = 0;                      // current led place
uint32_t playerGuess[4] = {};     // store players colour guesses
uint32_t playerColourGuess;

int difficulty = 0;

void setup() {
  pixels.begin();
  Serial.begin(9600);
  pixels.setBrightness(64);
  pinMode(buttonPin, INPUT_PULLUP);

  Wire.begin(8);                // join I2C bus with address #8
  Wire.onReceive(receiveEvent); // register event

  machine_state = ST_SELECT_COLOUR;
}

uint32_t RED = pixels.Color(255, 0, 0);
uint32_t BLUE = pixels.Color(0, 0, 255);
uint32_t YELLOW = pixels.Color(255, 255, 0);
uint32_t GREEN = pixels.Color(0, 255, 0);
uint32_t ORANGE = pixels.Color(255, 125, 0);
uint32_t PINK = pixels.Color(255, 0, 255);
uint32_t PURPLE = pixels.Color(125, 0, 255);

void loop() {
  potVal = analogRead(potPin);
  buttonPress = digitalRead(buttonPin);
  stateStuff();
  stateTransition();
  delay(100);
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read();        // receive byte as a character
    Serial.print(c);             // print the character
  }
  int x = Wire.read();           // receive byte as an integer
  difficulty = x;
  Serial.println(x);             // print the integer
}

void stateStuff() {
  switch(machine_state) {
    case ST_SELECT_COLOUR:
      selectColour(potVal, buttonPress, led);
      pixels.show();
      delay(25);
      break;
    case ST_GUESS_COLOUR:
      playerGuess[led] = playerColourGuess;
      Serial.println(playerGuess[led]);
      pixels.setPixelColor(led, playerGuess[led]);
      led += 1;
      delay(2000);
      break;

    default:
      Serial.println("something broke");
      break;
  }
}

void stateTransition(){
  switch(machine_state) {
    case ST_SELECT_COLOUR:
      if (buttonPress == 0) {
        machine_state = ST_GUESS_COLOUR;
      }
      break;
    case ST_GUESS_COLOUR:
      if (buttonPress == 1) {
        machine_state = ST_SELECT_COLOUR;
      }
      if (led <=4) {
        machine_state = 
      }
      break;

    default:
      Serial.println("something broke");
      break;
  }
}

void selectColour(int potVal, int buttonPress, int currentLED) {
  switch(potVal){
    case 0 ... 145:
      Serial.println("red");
      // pixels.fill(RED);
      pixels.setPixelColor(currentLED, RED);
      playerColourGuess = RED;
      break;
    case 146 ... 291:
      Serial.println("blue");
      // pixels.fill(BLUE);
      pixels.setPixelColor(currentLED, BLUE);
      playerColourGuess = BLUE;
      break;
    case 292 ... 437:
      Serial.println("yellow");
      // pixels.fill(YELLOW);
      pixels.setPixelColor(currentLED, YELLOW);
      playerColourGuess = YELLOW;
      break;
    case 438 ... 583:
      Serial.println("green");
      // pixels.fill(GREEN);
      pixels.setPixelColor(currentLED, GREEN);
      playerColourGuess = GREEN;
      break;
    case 584 ... 729:
      Serial.println("orange");
      // pixels.fill(ORANGE);
      pixels.setPixelColor(currentLED, ORANGE);
      playerColourGuess = ORANGE;
      break;
    case 730 ... 875:
      Serial.println("pink");
      // pixels.fill(PINK);
      pixels.setPixelColor(currentLED, PINK);
      playerColourGuess = PINK;
      break;
    case 876 ... 1023:
      Serial.println("purple"); 
      // pixels.fill(PURPLE);
      pixels.setPixelColor(currentLED, PURPLE);
      playerColourGuess = PURPLE;
      break;
    default:
      Serial.println("OUT OF RANGE");
      break;
  }
}
