/*
 * ESP32-CAM rover controller
 * Target: AI Thinker ESP32-CAM
 *
 * UART0 is connected to the Arduino Uno:
 *   ESP32 GPIO1 (TX0) -> Uno D0 (RX)
 *   ESP32 GPIO3 (RX0) <- Uno D1 (TX)
 *
 * Do not use the ESP32-CAM USB programmer while the Uno is connected to
 * UART0.  Upload first, then connect the serial link to the Uno.
 */

#include <WiFi.h>

// Change these before deploying the rover.
constexpr char AP_SSID[] = "Rover-ESP32";
constexpr char AP_PASSWORD[] = "123456789";

constexpr unsigned long UNO_BAUD_RATE = 115200;
constexpr unsigned long DRIVE_TRANSMIT_INTERVAL_MS = 100;  // 10 Hz

constexpr bool ENABLE_SERIAL_DIAGNOSTICS = false;   // UART0 is the Uno protocol link.  Leave this false during normal rover use,
                                                    // otherwise diagnostic text will be received by the Uno as malformed commands.

void startHttpServer();         // Startet den eingebetteten HTTP-Server und registriert die Webseiten- und
                                // Steuerungsrouten. Dadurch kann der ESP32 über das lokale WLAN vom Browser
                                // aus angesprochen werden und Fahrbefehle empfangen.

volatile int currentRightWheel = 0;  // Set by app_httpd.cpp after a browser control request has been validated.
volatile int currentLeftWheel = 0;   // int reads/writes are atomic on ESP32, so loop() can safely snapshot them.

void setDriveCommand(int rightWheel, int leftWheel) {  // this is function with two parameters, which is called from app_httpd.cpp after a browser control request has been validated.
  currentRightWheel = rightWheel;
  currentLeftWheel = leftWheel;
}

void setup() {
  Serial.begin(UNO_BAUD_RATE);

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    // There is no safe recovery path without a display or a separate debug UART.
    // Keep the controller inactive rather than running without a known network.
    while (true) {
      delay(1000);
    }
  }

  if (ENABLE_SERIAL_DIAGNOSTICS) {
    Serial.print("Rover controller: http://");
    Serial.println(WiFi.softAPIP());
  }

  startHttpServer();
}

void loop() {
  static unsigned long lastTransmitMs = 0;
  const unsigned long now = millis();

  // Unsigned subtraction remains correct when millis() wraps around.
  if (now - lastTransmitMs >= DRIVE_TRANSMIT_INTERVAL_MS) {
    lastTransmitMs = now;
    const int rightWheel = currentRightWheel;
    const int leftWheel = currentLeftWheel;
    // One complete, newline-terminated rover command every 100 ms.
    Serial.printf("%d;%d;\n", rightWheel, leftWheel);
  }
}
