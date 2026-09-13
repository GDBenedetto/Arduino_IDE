#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "camera.h"
#include "controls.h"
#include "ui.h"

// Port 80: UI + Fernbedienungsbefehle
WiFiServer uiServer(UI_PORT);

void connectWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);

  bool apStarted = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  if (!apStarted) {
    Serial.println("Access Point konnte nicht gestartet werden.");
    return;
  }

  Serial.println();
  Serial.println("ESP32-CAM Access Point gestartet.");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("Passwort: ");
  Serial.println(WIFI_PASSWORD);
  Serial.print("AP-IP-Adresse: ");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP32-CAM Remote startet...");

  controlsBegin();
  cameraBegin();
  connectWiFi();

  uiServer.begin();
  cameraStreamBegin();

  Serial.println();
  Serial.println("=================================");
  Serial.println("Fernbedienung:");
  Serial.print("http://");
  Serial.println(WiFi.softAPIP());

  Serial.print("Kamerastream:");
  Serial.print("http://");
  Serial.print(WiFi.softAPIP());
  Serial.println(":81/stream");
  Serial.println("=================================");
}

void loop() {
  uiHandleClient(uiServer);
  delay(1);
}
