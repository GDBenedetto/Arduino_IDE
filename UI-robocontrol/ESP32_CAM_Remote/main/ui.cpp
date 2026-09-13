#include <Arduino.h>
#include <WiFi.h>

#include "ui.h"
#include "config.h"
#include "controls.h"
#include "html.h"

void sendHttpHeader(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();
}

void uiHandleClient(WiFiServer& server) {

  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  // Erste HTTP-Zeile lesen:
  // z.B. "GET /led/on HTTP/1.1"
  String request = client.readStringUntil('\r');

  Serial.print("HTTP: ");
  Serial.println(request);


  // --------------------------------
  // Browser -> ESP32 -> Hardware
  // --------------------------------

  handleControlRequest(request);


  // --------------------------------
  // Antwort
  //
  // WICHTIG:
  // Egal welcher Befehl aufgerufen wurde,
  // danach wird wieder dieselbe
  // Hauptoberfläche gesendet.
  // --------------------------------

  String html = MAIN_PAGE_HTML;

  String streamUrl =
      "http://" +
      WiFi.localIP().toString() +
      ":" +
      String(STREAM_PORT) +
      "/stream";

  html.replace("{{STREAM_URL}}", streamUrl);

  html.replace("{{LED_STATE}}", outputLedState);

  if (outputLedState == "off") {
    html.replace(
      "{{LED_BUTTON}}",
      "<a href=\"/led/on\"><button>LED EIN</button></a>"
    );
  } else {
    html.replace(
      "{{LED_BUTTON}}",
      "<a href=\"/led/off\"><button>LED AUS</button></a>"
    );
  }


  // --------------------------------
  // HTTP + HTML senden
  // --------------------------------

  sendHttpHeader(client);
  client.print(html);

  delay(1);
  client.stop();
}
