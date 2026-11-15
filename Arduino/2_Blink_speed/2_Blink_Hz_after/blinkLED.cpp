
# include "blinkLED.h"

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