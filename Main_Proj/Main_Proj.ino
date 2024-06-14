#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

int buttonPin = 7;

#define PIN 6
#define NUMPIXELS 5

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//first state machine
#define ST_FIRSTLAUNCH 0
#define ST_SINGLEPLAYER 1
#define ST_MULTIPLAYER 2
#define ST_EASY 3
#define ST_MEDIUM 4
#define ST_HARD 5
#define ST_PLAYERSELECT 6
#define ST_DIFFICULTIES 7
int machine_state;

//second state machine
#define ST_NOTSTARTED 0
#define ST_START 1
#define ST_PLAYERSTURN 2
int game_state = 1;
String difficulty;
int mode;

//3rd state to manage other states
#define ST_MACHINE 1
#define ST_GAME 2
#define ST_CHOOSECOLOUR 3
#define ST_RANDOMCHOOSE 4
int states = 1;

//4th state machine to control LEDs
#define ST_SELECT_COLOUR 1
#define ST_GUESS_COLOUR 2
#define ST_SEND_COLOUR 3
int led_state;

int sensorValue;
int buttonState;
int led = 1;

#define I2C_ADDR 0x27
//LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

uint32_t playerGuess[4] = {};     // store players colour guesses
uint32_t playerColourGuess;

//Colours
uint32_t RED = pixels.Color(255, 0, 0);
uint32_t BLUE = pixels.Color(0, 0, 255);
uint32_t YELLOW = pixels.Color(255, 255, 0);
uint32_t GREEN = pixels.Color(0, 255, 0);
uint32_t ORANGE = pixels.Color(255, 125, 0);
uint32_t PINK = pixels.Color(255, 0, 255);

void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pixels.setBrightness(64);
  pixels.clear();
  pixels.show();
  machine_state = ST_FIRSTLAUNCH;
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.backlight();
  Wire.begin();
  Serial.begin(9600);
  lcd.clear();
  led_state = ST_SELECT_COLOUR;
  randomSeed(analogRead(1));
  //Rules();
}

void loop() {
  // put your main code here, to run repeatedly:
  States();
  // Serial.println(led_state);
}

void States() {
  switch(states) {
    case ST_MACHINE:
      Machine_States();
      break;

    case ST_GAME:
      Game_States();
      break;

    case ST_CHOOSECOLOUR:
      Colour_States();
      Another_Colour();
      break;

    case ST_RANDOMCHOOSE:
      Random_Colours();
      break;
  }
}

void Machine_States() {
  switch(machine_state) {
    case ST_FIRSTLAUNCH:
      TimeMessage("Select amount", "of players!");
      PlayerSelect();
      break;

    case ST_SINGLEPLAYER:
      mode = ST_SINGLEPLAYER;
      TimeMessage("Selected", "1 Player");
      Difficulties();   
      break;

    case ST_MULTIPLAYER:
      mode = ST_MULTIPLAYER;
      TimeMessage("Selected", "2 Player");
      Difficulties();   
      break;

    case ST_EASY:
      difficulty = "Easy";
      TimeMessage("Selected", "Easy mode");
      if (mode == ST_MULTIPLAYER) {
        states = 3;
      } else {
        states = 4;
      }
      break;

    case ST_MEDIUM:
      difficulty = "Medium";
      TimeMessage("Selected", "Medium mode");
      if (mode == ST_MULTIPLAYER) {
        states = 3;
      } else {
        states = 4;
      }
      break;

    case ST_HARD:
      difficulty = "Hard";
      TimeMessage("Selected", "Hard mode"); 
      if (mode == ST_MULTIPLAYER) {
        states = 3;
      } else {
        Random_Colours();
        states = 4;
      }
      break;
  }
}

void Colour_States() {
  sensorValue = analogRead(A0);
  switch(led_state) {
    case ST_SELECT_COLOUR:
      Message("Pick the", "Colours");
      SelectColour(sensorValue, led);
      pixels.show();
      delay(25);
      break;

    case ST_GUESS_COLOUR:
      Message("Pick the", "Colours");
      playerGuess[led] = playerColourGuess;
      Serial.println(playerGuess[led]);
      pixels.setPixelColor(led, playerGuess[led]);
      led += 1;
      if (led == 5) {
        states = 2;
      }
      delay(2000);
      break;
  }
}

void Another_Colour() {
  buttonState = digitalRead(buttonPin);
  switch(led_state) {
    case ST_SELECT_COLOUR:
      if (buttonState == 0) {
        led_state = ST_GUESS_COLOUR;
      }
      break;

    case ST_GUESS_COLOUR:
      if (buttonState == 1) {
        led_state = ST_SELECT_COLOUR;
      }
      break;
  }
}

void Game_States() {
  switch(game_state) {
      case ST_START:
        Game();
        break;
      case ST_PLAYERSTURN:
        Send_Arduino();
        break;
  }
}

void Game() {
  TimeMessage("Game will", "START!");
  TimeMessage("Difficulty:", difficulty);
  game_state = ST_PLAYERSTURN;
}

void PlayerSelect() {
  bool loop = true;
  while(loop) {
    sensorValue = analogRead(A0);
    buttonState = digitalRead(buttonPin);
    if(sensorValue < 600) {
      Message("1 Player?", "");
      if (buttonState == 0) {
        machine_state = ST_SINGLEPLAYER;
        loop = false;
      }
    }
    else {
      Message("2 Player?", "");
      if (buttonState == 0) {
        machine_state = ST_MULTIPLAYER;
        loop = false;
      }
    }
  }
  lcd.clear();
}

void SelectColour(int value, int currentLED) {
  switch(value) {
    case 0 ... 171:
      pixels.setPixelColor(currentLED, RED);
      playerColourGuess = RED;
      break;
    
    case 172 ... 342:
      pixels.setPixelColor(currentLED, BLUE);
      playerColourGuess = BLUE;
      break;

    case 343 ... 513:
      pixels.setPixelColor(currentLED, YELLOW);
      playerColourGuess = YELLOW;
      break;

    case 514 ... 684:
      pixels.setPixelColor(currentLED, GREEN);
      playerColourGuess = GREEN;
      break;

    case 685 ... 855:
      pixels.setPixelColor(currentLED, ORANGE);
      playerColourGuess = ORANGE;
      break;

    case 856 ... 1023:
      pixels.setPixelColor(currentLED, PINK);
      playerColourGuess = PINK;
      break;
  }
}

void Random_Colours() {
  int temp;
  for (int i=1; i < 5; i++) {
    temp = random(1024);
    Serial.println(temp);
    SelectColour(temp, led);
    pixels.show();
    playerGuess[led] = playerColourGuess;
    pixels.setPixelColor(led, playerGuess[led]);
    led++;
    delay(250);
  }
  states = 2;
}

void Difficulties() {
  bool loop = true;
  while(loop) {
    sensorValue = analogRead(A0);
    buttonState = digitalRead(buttonPin);
    if (sensorValue < 400) {
      Message("Difficulty:", "Easy  ");
      if (buttonState == 0) {
        machine_state = ST_EASY;
        loop = false;
      }
    }
    else if (sensorValue > 400 && sensorValue < 800) {
      Message("Difficulty:", "Medium");
      if (buttonState == 0) {
        machine_state = ST_MEDIUM;
        loop = false;
      }
    }
    else {
      Message("Difficulty:", "Hard  ");
      if (buttonState == 0) {
        machine_state = ST_HARD;
        Serial.println(ST_HARD);
        loop = false;
      }
    }
  }
  lcd.clear();
}

void Send_Arduino() {
  TimeMessage("GO", "PLAYER");
  Wire.beginTransmission(8);
  Wire.write("Difficulty is: ");
  Wire.write(machine_state);
  Wire.endTransmission();
  delay(1000);
}

void Rules() {
  TimeMessage("Mastermind", "Rules");
  TimeMessage("First pick", "1 or 2 player");
  TimeMessage("Difficulty", "Pick one of 3");
  TimeMessage("Player A", "Will pick colours");
  TimeMessage("Using switch", "and button");
  TimeMessage("Player B will", "guess colours");
  TimeMessage("After attempt", "This screen will:");
  TimeMessage("Tell whats right", "and whats wrong");
  TimeMessage("Guess until", "you can win!");
}

void TimeMessage(String line1, String line2) {
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  delay(3000);
  lcd.clear();
}

void Message(String line1, String line2) {
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}