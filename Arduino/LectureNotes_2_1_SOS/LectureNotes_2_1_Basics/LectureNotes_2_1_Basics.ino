/*
  Programming basics: control the Arduino Uno's built-in LED.
  No external wiring is needed.

  1. Upload the sketch and open the Serial Monitor at 9600 baud.
  2. Select "Newline" or "Both NL & CR" as the line ending.
  3. Type 3 and click Send: the LED turns on for 3 seconds, then off.
  4. Try 1, 5, or 0. Try invalid input such as hello or 12.

  Send one number at a time. This simple example uses delay(), so it
  finishes the countdown before reading the next command.
*/

#include <Arduino.h>

// A variable names a value. int stores a whole number.
// const means this pin number cannot change.
const int ledPin = LED_BUILTIN;

// A function gives a task a name. Its parameter receives a value.
// void means this function does not return a value.
void turnLedOn(int seconds) {
  digitalWrite(ledPin, HIGH);

  // while repeats instructions as long as its condition is true.
  while (seconds > 0) {
    Serial.print("LED ON. Seconds remaining: ");
    Serial.println(seconds);
    delay(1000);
    seconds--; // Subtract 1 and store the new value.
  }

  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF. Send another number (0-9).");
}

void setup() {
  // setup() runs once after power-on or reset.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
  delay(2000); // Give the Serial Monitor time to connect.
  Serial.println("LED timer: select Newline in the Serial Monitor.");
  Serial.println("Send 1-9: LED on for that many seconds, then off.");
  Serial.println("Send 0: LED off. Try sending 3 now!");
}

void loop() {
  // loop() repeats. if runs its block only when the condition is true.
  if (Serial.available() > 0) {
    String userInput = Serial.readStringUntil('\n'); // String stores text.
    userInput.trim(); // Remove spaces and the CR line-ending character.

    // bool stores true or false. && means all conditions must be true.
    bool isValid = userInput.length() == 1 &&
                   userInput[0] >= '0' && userInput[0] <= '9';

    // if / else if / else choose one branch; they are not loops.
    // ! means "not", == compares values, and = assigns a value.
    if (!isValid) {
      Serial.println("Invalid input. Send one number from 0 to 9.");
    } else if (userInput == "0") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF. Send another number (0-9).");
    } else {
      int seconds = userInput.toInt(); // Convert valid text to a number.
      turnLedOn(seconds);            // Call our function.
    }
  }
}
