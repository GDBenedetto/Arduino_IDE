// Blink the built-in LED at a frequency given in Hz.

// Function declaration: tells Arduino about our function.
void blinkLED(float frequencyHz);  // This declaration here not optional

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

