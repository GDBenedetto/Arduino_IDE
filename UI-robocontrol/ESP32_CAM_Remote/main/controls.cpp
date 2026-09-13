#include "controls.h"
#include "config.h"

String outputLedState = "off";

void controlsBegin() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void handleControlRequest(const String& request) {

  // --------------------------------
  // Bewegungsbefehle
  // --------------------------------

  if (request.indexOf("GET /move/forward") >= 0) {
    Serial.println("BEFEHL: VORWAERTS");
    return;
  }

  if (request.indexOf("GET /move/backward") >= 0) {
    Serial.println("BEFEHL: RUECKWAERTS");
    return;
  }

  if (request.indexOf("GET /move/left") >= 0) {
    Serial.println("BEFEHL: LINKS");
    return;
  }

  if (request.indexOf("GET /move/right") >= 0) {
    Serial.println("BEFEHL: RECHTS");
    return;
  }


  // --------------------------------
  // LED
  // --------------------------------

  if (request.indexOf("GET /led/on") >= 0) {
    digitalWrite(LED_PIN, HIGH);
    outputLedState = "on";

    Serial.println("LED: EIN");
  }

  if (request.indexOf("GET /led/off") >= 0) {
    digitalWrite(LED_PIN, LOW);
    outputLedState = "off";

    Serial.println("LED: AUS");
  }
}
