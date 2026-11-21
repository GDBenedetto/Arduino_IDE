// Blink the built-in LED at a frequency given in Hz.

// Function declaration: tells Arduino about our function.
void blinkLED(float frequencyHz);  // This declaration is optional

void setup() {

  // Configure the built-in LED pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Start with the LED turned off.
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  // Blink twice per second.
  // The 'f' means 2.0 is a float: a number that can have decimals.
  blinkLED(2.0f);
}

// This function performs ONE complete ON/OFF cycle.
// frequencyHz is the number of complete cycles per second.
// 'void' means the function does not return a value.
void blinkLED(float frequencyHz) {
  // Zero or negative frequencies are not valid for blinking.
  // Turn the LED off and leave the function.
  if (frequencyHz <= 0.0f) {
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }

  // One second contains 1000 milliseconds.
  // A full cycle has two equal parts: ON and OFF.
  // Therefore, each part lasts 500 / frequencyHz milliseconds.
  //
  // Example: at 2 Hz, each part lasts 500 / 2 = 250 ms.
  unsigned long halfPeriodMs =
      (unsigned long)(500.0f / frequencyHz);

  // delay() uses whole milliseconds.
  // Keep each part at least 1 ms long.
  if (halfPeriodMs < 1) {
    halfPeriodMs = 1;
  }

  // Turn the LED on and wait for half a cycle.
  digitalWrite(LED_BUILTIN, HIGH);
  delay(halfPeriodMs);

  // Turn the LED off and wait for the other half.
  digitalWrite(LED_BUILTIN, LOW);
  delay(halfPeriodMs);
}