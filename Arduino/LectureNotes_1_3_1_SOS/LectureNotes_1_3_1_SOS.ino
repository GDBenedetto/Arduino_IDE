const int LED_PIN = LED_BUILTIN;

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



void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  sendSOS();  // Uses the standard dit duration of 200 ms
}