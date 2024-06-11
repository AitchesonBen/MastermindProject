#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

int buttonPin = 7;

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
int states = 1;

int sensorValue;
int buttonState;

#define I2C_ADDR 0x27

//LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

void setup() {
  // put your setup code here, to run once:
  machine_state = ST_FIRSTLAUNCH;
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.backlight();
  Wire.begin();
  Serial.begin(9600);
  lcd.clear();
  //Rules();
}

void loop() {
  // put your main code here, to run repeatedly:
  States();
  Serial.println(game_state);
}

void States() {
  switch(states) {
    case ST_MACHINE:
      Machine_States();
      break;
    case ST_GAME:
      Game_States();
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
      states = 2;
      break;

    case ST_MEDIUM:
      difficulty = "Medium";
      TimeMessage("Selected", "Medium mode");
      states = 2;
      break;

    case ST_HARD:
      difficulty = "Hard";
      TimeMessage("Selected", "Hard mode"); 
      states = 2;
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