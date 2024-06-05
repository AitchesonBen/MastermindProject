#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

#define ST_SINGLEPLAYER 1
#define ST_MULTIPLAYER 2
int machine_state;

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Wire.begin();
  Serial.begin(9600);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  Rules();
}

void Rules() {
  Message("Mastermind", "Rules");
}

void Message(String line1, String line2) {
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  delay(2500);
  lcd.clear();
}
