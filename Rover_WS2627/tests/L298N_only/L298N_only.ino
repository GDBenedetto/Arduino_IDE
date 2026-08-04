/**
 * L298N Motor Driver
 * Zwei Gleichstrommotoren ohne PWM
 *
 * ENA- und ENB-Jumper am L298N bleiben gesteckt.
 */

// Motor A
#define IN1 9
#define IN2 8

// Motor B
#define IN3 7
#define IN4 6

void setup() {
  Serial.begin(115200);

  // Richtungspins als Ausgänge festlegen
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Motoren beim Start stoppen
  stopMotors();
}

void loop() {
  // Beide Motoren vorwärts
  Serial.println("FORWARD");

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // delay(5000);

  // // Beide Motoren stoppen
  // Serial.println("STOP");

  // stopMotors();

  // delay(2000);

  // // Beide Motoren rückwärts
  // Serial.println("BACKWARD");

  // digitalWrite(IN1, LOW);
  // digitalWrite(IN2, HIGH);

  // digitalWrite(IN3, LOW);
  // digitalWrite(IN4, HIGH);

  // delay(2000);

  // // Beide Motoren stoppen
  // Serial.println("STOP");

  // stopMotors();

  // delay(1000);
}

void stopMotors() {
  // Beide Eingänge LOW: Motoren werden freigegeben
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}







