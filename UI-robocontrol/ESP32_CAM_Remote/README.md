# ESP32-CAM Fernbedienung

## Projektstruktur

- `main.ino` – Programmstart, WLAN und Server
- `config.h` – WLAN, Ports und AI-Thinker-Pinbelegung
- `camera.cpp/.h` – Kamera-Initialisierung und MJPEG-Stream
- `controls.cpp/.h` – HTTP-Befehle und GPIO/Aktionslogik
- `ui.cpp/.h` – HTTP-Anfragen und Auslieferung der Hauptseite
- `html.h` – komplette HTML/CSS-Oberfläche in einem Block

## Voraussetzungen

- ESP32-CAM AI Thinker
- Arduino-ESP32
- Kamera-Bibliothek `esp_camera` (Bestandteil des ESP32-Arduino-Systems)
- ESP-IDF HTTP-Server API `esp_http_server.h` (Bestandteil der ESP32-Arduino-Plattform)

## Verhalten

- Port 80: Hauptseite + Steuerbefehle
- Port 81: kontinuierlicher MJPEG-Kamerastream
- Jeder Steuerklick verändert die URL, danach liefert der ESP32 wieder dieselbe Hauptoberfläche.
- Die Bewegungsbefehle sind Platzhalter und werden zunächst nur über Serial ausgegeben.
- GPIO 4 ist beim AI-Thinker der eingebaute Flash-LED-Pin.

## Hinweis

Der Kamerastream verwendet für die kontinuierliche Übertragung die native `esp_http_server`-API. 
Die Befehlsverarbeitung der Fernbedienung bleibt dagegen bewusst bei `request.indexOf(...)`, damit sie an die zuvor eingeführte Unterrichtsmethode anschließt.
