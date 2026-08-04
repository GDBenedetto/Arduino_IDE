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
#include <stdlib.h>
#include <string.h>

constexpr unsigned long SERIAL_BAUD_RATE = 115200;
constexpr int MIN_LEVER_VALUE = -50;
constexpr int MAX_LEVER_VALUE = 50;
constexpr size_t MAX_FRAME_LENGTH = 24;

int rightLeverValue = 0;
int leftLeverValue = 0;

char frameBuffer[MAX_FRAME_LENGTH];
size_t frameLength = 0;

int clampLeverValue(long value) {
  if (value < MIN_LEVER_VALUE) return MIN_LEVER_VALUE;
  if (value > MAX_LEVER_VALUE) return MAX_LEVER_VALUE;
  return static_cast<int>(value);
}

bool parseLeverValue(char *text, int *value) {
  if (text == nullptr || *text == '\0') return false;

  char *end = nullptr;
  const long parsed = strtol(text, &end, 10);
  if (*end != '\0') return false;

  *value = clampLeverValue(parsed);
  return true;
}

bool parseDriveFrame(char *frame) {
  char *firstSeparator = strchr(frame, ';');
  if (firstSeparator == nullptr) return false;
  *firstSeparator = '\0';

  char *secondSeparator = strchr(firstSeparator + 1, ';');
  if (secondSeparator == nullptr || *(secondSeparator + 1) != '\0') return false;
  *secondSeparator = '\0';

  int parsedRight = 0;
  int parsedLeft = 0;
  if (!parseLeverValue(frame, &parsedRight) ||
      !parseLeverValue(firstSeparator + 1, &parsedLeft)) {
    return false;
  }

  rightLeverValue = parsedRight;
  leftLeverValue = parsedLeft;
  return true;
}

void printLeverValues() {
  Serial.print(F("Right: "));
  Serial.print(rightLeverValue);
  Serial.print(F("  Left: "));
  Serial.println(leftLeverValue);
}

void processReceivedByte(char received) {
  if (received == '\r') return;

  if (received == '\n') {
    frameBuffer[frameLength] = '\0';
    if (parseDriveFrame(frameBuffer)) {
      printLeverValues();
    }
    frameLength = 0;
    return;
  }

  if (frameLength < MAX_FRAME_LENGTH - 1) {
    frameBuffer[frameLength++] = received;
  } else {
    // Drop an oversized/corrupted frame and wait for its terminating newline.
    frameLength = 0;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
}

void loop() {
  while (Serial.available() > 0) {
    processReceivedByte(static_cast<char>(Serial.read()));
  }
}
