/*
 * Rover GIGA R1 WiFi touchscreen controller.
 *
 * GIGA R1 (Wi-Fi station) --> ESP32-CAM access point (192.168.4.1:80)
 *
 * The ESP32-CAM must run this project's existing ESP32_main sketch. This
 * sketch sends its HTTP command format:
 *   GET /control?right=<value>&left=<value>
 * where each value is an integer between -50 and 50.
 *
 * Stability notes (see accompanying change summary for full details):
 *  - Touch points are rotated 90 degrees before use. The touch controller
 *    always reports coordinates in the panel's native portrait orientation;
 *    it has no knowledge of display.setRotation(), so raw points must be
 *    converted into landscape space by hand or touches land in the wrong
 *    place (the right lever in particular could barely be reached at all).
 *  - Each HTTP request now fully drains its response instead of reading for
 *    a fixed 25 ms window. GIGA's networking core is known to eventually
 *    hang a connect() call for many seconds if a socket is repeatedly
 *    closed before its response is fully read -- see
 *    arduino/ArduinoCore-mbed#937.
 *  - The on-screen levers -- and the values they'd send -- are forced to
 *    neutral whenever Wi-Fi is down, so the display never optimistically
 *    shows a command that isn't actually reaching the rover.
 */

#include <Arduino_GigaDisplay_GFX.h>
#include <Arduino_GigaDisplayTouch.h>
#include <WiFi.h>

namespace {

constexpr char ROVER_WIFI_SSID[] = "Rover-ESP32";
constexpr char ROVER_WIFI_PASSWORD[] = "123456789";
const IPAddress ROVER_IP(192, 168, 4, 1);
constexpr uint16_t ROVER_HTTP_PORT = 80;

constexpr int MIN_DRIVE_VALUE = -50;
constexpr int MAX_DRIVE_VALUE = 50;
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 5000;
constexpr unsigned long COMMAND_INTERVAL_MS = 150;    // ~6.7 Hz while held; see sendDriveCommand().
constexpr unsigned long HTTP_DRAIN_TIMEOUT_MS = 200;  // Safety ceiling; see sendDriveCommand().
constexpr unsigned long HEARTBEAT_INTERVAL_MS = 5000;

// The GIGA Display Shield is used in landscape orientation (800 x 480).
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 480;
constexpr int HEADER_HEIGHT = 76;
constexpr int TRACK_TOP = 130;
constexpr int TRACK_BOTTOM = 425;
constexpr int TRACK_WIDTH = 44;
constexpr int TOUCH_LANE_HALF_WIDTH = 130;
constexpr int LEFT_TRACK_X = 210;
constexpr int RIGHT_TRACK_X = 590;

constexpr uint16_t COLOR_BACKGROUND = 0x0841;
constexpr uint16_t COLOR_PANEL = 0x10A2;
constexpr uint16_t COLOR_TRACK = 0x5AEB;
constexpr uint16_t COLOR_TEXT = 0xFFFF;
constexpr uint16_t COLOR_MUTED_TEXT = 0xBDF7;
constexpr uint16_t COLOR_LEFT = 0x07FF;
constexpr uint16_t COLOR_RIGHT = 0xFD20;
constexpr uint16_t COLOR_STOP = 0xF800;
constexpr uint16_t COLOR_CONNECTED = 0x07E0;

GigaDisplay_GFX display;
Arduino_GigaDisplayTouch touchDetector;

int leftDrive = 0;
int rightDrive = 0;
bool commandDirty = true;  // Send a stop command once Wi-Fi becomes available.
bool lastWiFiConnected = false;
bool lastHttpRequestSucceeded = false;
unsigned long lastWiFiAttemptMs = 0;
unsigned long lastCommandSentMs = 0;
unsigned long lastHeartbeatMs = 0;

int clampDriveValue(int value) {
  if (value < MIN_DRIVE_VALUE) return MIN_DRIVE_VALUE;
  if (value > MAX_DRIVE_VALUE) return MAX_DRIVE_VALUE;
  return value;
}

int driveValueFromTouchY(int y) {
  y = constrain(y, TRACK_TOP, TRACK_BOTTOM);
  // Up is forward (+50); down is reverse (-50).
  return clampDriveValue(map(y, TRACK_BOTTOM, TRACK_TOP,
                             MIN_DRIVE_VALUE, MAX_DRIVE_VALUE));
}

int thumbYFromDriveValue(int value) {
  return map(clampDriveValue(value), MIN_DRIVE_VALUE, MAX_DRIVE_VALUE,
             TRACK_BOTTOM, TRACK_TOP);
}

// The touch controller always reports (x, y) in the panel's native
// portrait orientation (480 wide x 800 tall) and has no idea
// display.setRotation() was ever called -- the touch and GFX libraries are
// independent and don't share rotation state. This rotates a raw point
// into our landscape (800 x 480) drawing space to match.
void rotateTouchPoint(int rawX, int rawY, int *outX, int *outY) {
  *outX = rawY;
  *outY = SCREEN_HEIGHT - rawX;
}

void drawCenteredText(const char *text, int centerX, int baselineY,
                      uint8_t textSize, uint16_t color) {
  display.setTextSize(textSize);
  display.setTextColor(color);
  const int16_t textWidth = static_cast<int16_t>(strlen(text)) * 6 * textSize;
  display.setCursor(centerX - textWidth / 2, baselineY);
  display.print(text);
}

void drawHeader() {
  display.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, COLOR_PANEL);
  drawCenteredText("ROVER CONTROL", SCREEN_WIDTH / 2, 13, 3, COLOR_TEXT);

  display.setTextSize(2);
  display.setCursor(18, 48);
  if (WiFi.status() == WL_CONNECTED) {
    display.setTextColor(COLOR_CONNECTED);
    display.print("Wi-Fi: connected");
  } else {
    display.setTextColor(COLOR_STOP);
    display.print("Wi-Fi: reconnecting");
  }

  display.setCursor(500, 48);
  if (lastHttpRequestSucceeded) {
    display.setTextColor(COLOR_CONNECTED);
    display.print("ESP32: reachable");
  } else {
    display.setTextColor(COLOR_MUTED_TEXT);
    display.print("ESP32: waiting");
  }
}

void drawLever(int centerX, const char *label, int value, uint16_t accentColor) {
  const int laneLeft = centerX - TOUCH_LANE_HALF_WIDTH;
  const int laneWidth = TOUCH_LANE_HALF_WIDTH * 2;
  display.fillRect(laneLeft, HEADER_HEIGHT, laneWidth,
                   SCREEN_HEIGHT - HEADER_HEIGHT, COLOR_BACKGROUND);

  drawCenteredText(label, centerX, 92, 2, COLOR_TEXT);
  display.fillRoundRect(centerX - TRACK_WIDTH / 2, TRACK_TOP,
                        TRACK_WIDTH, TRACK_BOTTOM - TRACK_TOP, 18,
                        COLOR_TRACK);
  display.drawRoundRect(centerX - TRACK_WIDTH / 2, TRACK_TOP,
                        TRACK_WIDTH, TRACK_BOTTOM - TRACK_TOP, 18,
                        COLOR_MUTED_TEXT);

  const int centerY = thumbYFromDriveValue(value);
  display.fillRoundRect(centerX - 82, centerY - 18, 164, 36, 12,
                        value == 0 ? COLOR_STOP : accentColor);
  display.drawRoundRect(centerX - 82, centerY - 18, 164, 36, 12, COLOR_TEXT);

  char valueText[8];
  snprintf(valueText, sizeof(valueText), "%+d", value);
  drawCenteredText(valueText, centerX, centerY - 8, 2, COLOR_BACKGROUND);
  drawCenteredText("FORWARD", centerX, TRACK_TOP - 24, 1, COLOR_MUTED_TEXT);
  drawCenteredText("REVERSE", centerX, TRACK_BOTTOM + 10, 1, COLOR_MUTED_TEXT);
}

void drawInterface() {
  display.fillScreen(COLOR_BACKGROUND);
  drawHeader();
  drawLever(LEFT_TRACK_X, "LEFT", leftDrive, COLOR_LEFT);
  drawLever(RIGHT_TRACK_X, "RIGHT", rightDrive, COLOR_RIGHT);
}

void updateLevers(int newLeftDrive, int newRightDrive) {
  newLeftDrive = clampDriveValue(newLeftDrive);
  newRightDrive = clampDriveValue(newRightDrive);

  if (newLeftDrive != leftDrive) {
    leftDrive = newLeftDrive;
    drawLever(LEFT_TRACK_X, "LEFT", leftDrive, COLOR_LEFT);
    commandDirty = true;
  }
  if (newRightDrive != rightDrive) {
    rightDrive = newRightDrive;
    drawLever(RIGHT_TRACK_X, "RIGHT", rightDrive, COLOR_RIGHT);
    commandDirty = true;
  }
}

void serviceWiFi() {
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (connected != lastWiFiConnected) {
    lastWiFiConnected = connected;
    commandDirty = true;  // Send a known-safe (0,0) state after reconnecting.
    if (connected) {
      Serial.print("Wi-Fi connected, IP ");
      Serial.print(WiFi.localIP());
      Serial.print(", RSSI ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
    } else {
      lastHttpRequestSucceeded = false;
      Serial.println("Wi-Fi disconnected");
    }
    drawHeader();
  }

  if (connected || millis() - lastWiFiAttemptMs < WIFI_RETRY_INTERVAL_MS) return;

  lastWiFiAttemptMs = millis();
  Serial.println("Wi-Fi: attempting to (re)connect...");
  WiFi.begin(ROVER_WIFI_SSID, ROVER_WIFI_PASSWORD);
}

bool sendDriveCommand() {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClient client;
  if (!client.connect(ROVER_IP, ROVER_HTTP_PORT)) return false;

  client.print("GET /control?right=");
  client.print(rightDrive);
  client.print("&left=");
  client.print(leftDrive);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(ROVER_IP);
  client.println("Connection: close");
  client.println();

  // Fully drain the response before closing. GIGA's networking core has a
  // known issue (arduino/ArduinoCore-mbed#937) where repeatedly closing a
  // socket before its response is fully read can eventually make a *later*
  // connect() call hang for many seconds -- far worse than the few extra
  // milliseconds spent waiting here. HTTP_DRAIN_TIMEOUT_MS is only a safety
  // ceiling for a server that never closes; on a healthy local link this
  // loop should exit in well under that.
  const unsigned long requestStartedMs = millis();
  while ((client.connected() || client.available()) &&
         millis() - requestStartedMs < HTTP_DRAIN_TIMEOUT_MS) {
    while (client.available() > 0) client.read();
    delay(1);
  }
  client.stop();
  return true;
}

void serviceDriveCommand() {
  if (!commandDirty || millis() - lastCommandSentMs < COMMAND_INTERVAL_MS) return;

  lastCommandSentMs = millis();
  const bool requestSucceeded = sendDriveCommand();
  if (requestSucceeded) commandDirty = false;

  if (requestSucceeded != lastHttpRequestSucceeded) {
    lastHttpRequestSucceeded = requestSucceeded;
    Serial.println(requestSucceeded ? "ESP32: reachable" : "ESP32: request failed");
    drawHeader();
  }
}

void serviceTouch() {
  if (!lastWiFiConnected) {
    // Nothing computed here could reach the rover anyway -- keep the
    // on-screen levers honestly at neutral instead of showing a value that
    // isn't actually being delivered.
    updateLevers(0, 0);
    return;
  }

  GDTpoint_t points[5];
  uint8_t contactCount = touchDetector.getTouchPoints(points);
  if (contactCount > 5) contactCount = 5;  // Defensive; hardware/library cap at 5.

  bool leftTouched = false;
  bool rightTouched = false;
  int newLeftDrive = 0;
  int newRightDrive = 0;

  for (uint8_t i = 0; i < contactCount; ++i) {
    int x, y;
    rotateTouchPoint(static_cast<int>(points[i].x), static_cast<int>(points[i].y),
                     &x, &y);

    // Accept touches anywhere in the lever's visible lane, not just the
    // narrow drawn track -- otherwise a finger that overshoots slightly
    // past the top/bottom of the track (easy to do at full deflection)
    // gets silently dropped for that frame and the lever snaps to zero.
    if (y < HEADER_HEIGHT || y > SCREEN_HEIGHT) continue;

    if (abs(x - LEFT_TRACK_X) <= TOUCH_LANE_HALF_WIDTH) {
      newLeftDrive = driveValueFromTouchY(y);
      leftTouched = true;
    } else if (abs(x - RIGHT_TRACK_X) <= TOUCH_LANE_HALF_WIDTH) {
      newRightDrive = driveValueFromTouchY(y);
      rightTouched = true;
    }
  }

  updateLevers(leftTouched ? newLeftDrive : 0,
               rightTouched ? newRightDrive : 0);

  // Refresh a stationary held lever at the command rate. Releasing it
  // changes the value to zero, which queues the stop command immediately.
  if (leftTouched || rightTouched) commandDirty = true;
}

void serviceHeartbeat() {
  if (millis() - lastHeartbeatMs < HEARTBEAT_INTERVAL_MS) return;
  lastHeartbeatMs = millis();
  Serial.print("[heartbeat] t=");
  Serial.print(millis() / 1000);
  Serial.print("s wifi=");
  Serial.print(lastWiFiConnected ? "up" : "down");
  Serial.print(" esp32=");
  Serial.print(lastHttpRequestSucceeded ? "ok" : "?");
  Serial.print(" left=");
  Serial.print(leftDrive);
  Serial.print(" right=");
  Serial.println(rightDrive);
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Rover GIGA R1 controller starting...");

  display.begin();
  display.setRotation(1);  // Landscape: 800 x 480.
  drawInterface();

  if (!touchDetector.begin()) {
    Serial.println("Touch controller init FAILED");
    display.fillScreen(COLOR_BACKGROUND);
    drawCenteredText("Touch controller unavailable", SCREEN_WIDTH / 2,
                     SCREEN_HEIGHT / 2, 2, COLOR_STOP);
    while (true) delay(1000);
  }
  Serial.println("Touch controller init OK");

  lastWiFiAttemptMs = millis() - WIFI_RETRY_INTERVAL_MS;
}

void loop() {
  serviceWiFi();
  serviceTouch();
  serviceDriveCommand();
  serviceHeartbeat();
  delay(2);
}
