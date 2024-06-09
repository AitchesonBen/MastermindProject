#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUMPIXELS 9

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int potPin = A0;
int potVal = 0;
int buttonPin = 2;
int buttonPress = 0;

void setup() {
  pixels.begin();
  Serial.begin(9600);
  pixels.setBrightness(64);
  pinMode(buttonPin, INPUT_PULLUP);
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
  Serial.println(buttonPress);
  selectColour(potVal);
  pixels.show();
  delay(25);
}

void selectColour(int potVal) {
  switch(potVal){
    case 0 ... 145:
      Serial.println("red");
      pixels.fill(RED);
      break;
    case 146 ... 291:
      Serial.println("blue");
      pixels.fill(BLUE);
      break;
    case 292 ... 437:
      Serial.println("yellow");
      pixels.fill(YELLOW);
      break;
    case 438 ... 583:
      Serial.println("green");
      pixels.fill(GREEN);
      break;
    case 584 ... 729:
      Serial.println("orange");
      pixels.fill(ORANGE);
      break;
    case 730 ... 875:
      Serial.println("pink");
      pixels.fill(PINK);
      break;
    case 876 ... 1023:
      Serial.println("purple"); 
      pixels.fill(PURPLE);
      break;
    default:
      Serial.println("OUT OF RANGE");
      break;
  }
}