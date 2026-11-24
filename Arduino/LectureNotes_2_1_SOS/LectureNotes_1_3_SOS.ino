
/*
 * Project: SOS
 * Description: Send the SOS signal using the built-in LED.
 */

// Standard Morse code timing:
// Dit (dot) = 200 ms
// Dah (dash) = 3 * Dit
// Pause between dots/dashes = 1 * Dit
// Pause between letters = 3 * Dit
// Pause between repetitions = 7 * Dit

// Function declaration (prototype)
void sendSOS(int ditDuration = 200);  // this line is needed because Function definition is after setup()

const int LED_PIN = LED_BUILTIN;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  sendSOS();  // Uses the standard dit duration of 200 ms
}

// Function definition
// Send SOS using the specified dit duration
void sendSOS(int ditDuration = 200) {

  int dahDuration = 3 * ditDuration;

  // Letter S: ...
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ditDuration);
    digitalWrite(LED_PIN, LOW);

    if (i < 2) delay(ditDuration);
  }

  // Pause between S and O
  delay(2 * ditDuration);

  // Letter O: ---
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(dahDuration);
    digitalWrite(LED_PIN, LOW);

    if (i < 2) delay(ditDuration);
  }

  // Pause between O and S
  delay(2 * ditDuration);

  // Letter S: ...
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ditDuration);
    digitalWrite(LED_PIN, LOW);

    if (i < 2) delay(ditDuration);
  }

  // Pause before repeating SOS
  delay(7 * ditDuration);
}
