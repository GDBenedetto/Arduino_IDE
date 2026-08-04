/*
 * Arduino Uno rover serial receiver
 *
 * HardwareSerial uses D0 (RX) and D1 (TX).  Connect:
 *   ESP32-CAM GPIO1 (TX0) -> Uno D0 (RX)
 *   ESP32-CAM GPIO3 (RX0) <- Uno D1 (TX)
 *
 * The USB serial monitor shares this UART. Disconnect the ESP32-CAM while
 * uploading this sketch, then reconnect it and open the monitor at 115200.
 */

#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
}


void loop() {
  static char buf[128];
  static byte pos = 0;

  while (Serial.available() > 0) {  // solange Daten im Puffer sind ist die Schleife aktiv
    byte b = Serial.read();   // b ist das rohe Byte 

    Serial.print("Byte HEX: 0x");
    if (b < 16) Serial.print("0");
    Serial.print(b, HEX);

    Serial.print("  DEC: ");
    Serial.print(b, DEC);

    Serial.print("  BIN: ");
    for (int i = 7; i >= 0; i--) {
      Serial.print((b >> i) & 1);
    }
    Serial.println();

    char c = (char)b;

    if (c == '\n') {
      buf[pos] = '\0';
      Serial.print("String: ");
      Serial.println(buf);
      pos = 0;
    } else if (pos < sizeof(buf) - 1) {
      buf[pos++] = c;
    }
  }
}
