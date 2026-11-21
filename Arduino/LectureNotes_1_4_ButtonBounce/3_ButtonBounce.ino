#include <Arduino.h>

const uint8_t buttonPin = 2;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if (readButtonPress()) {
    Serial.println("Button pressed");
  }
}


bool readButtonPress() {
  if (digitalRead(buttonPin) != LOW) { return false; }
  delay(20);
  if (digitalRead(buttonPin) != LOW) { return false; }

  while (digitalRead(buttonPin) == LOW) { delay(1); }
  delay(20);
  if (digitalRead(buttonPin) == HIGH) { return true; }

  return false;  // Is this line needed???
}
