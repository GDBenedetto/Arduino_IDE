# Rover Capability

This document describes the rover's capabilities, its components, and how they interact with each other. It also outlines the engineering requirements.

## Components Overview

- The rover is equipped with two DC wheel motors.
- The rover includes a battery as its power source.
- The rover uses an Arduino Uno R3.
- The rover uses an ESP32-CAM.
- The chassis carries all components.

## Component Interfaces

- The battery powers both the Arduino and the ESP32 controller.
- Both wheels are connected to the Arduino.
- The ESP32 and Arduino are connected through a serial bus.
- On the Arduino, D0 and D1 are used for serial communication with the ESP32.

## Capability Requirements

- The user shall be able to connect to the ESP32-CAM using Wi-Fi.
- The ESP32-CAM shall implement an HTTP server for web interaction.
- The ESP32-CAM shall provide an interface to control the rover forward and backward.
- The web interface shall contain two levers: one for controlling the right wheel and one for the left wheel.
- The lever can be moved up for forward movement and down for backward movement.
- The lever shall have a resolution of 5: five positions from the center upward and five positions from the center downward, for a total of 10 resolutions.
- The lever shall return to the middle position when the user releases it.
- The ESP32-CAM shall convert the lever position from -50 (down) to 50 (up).
- The ESP32-CAM shall provide a string on the serial bus when the user interacts with the web controller.
- The string has no fixed length and is separated using semicolons (;).
- The first string position is reserved for controlling the right wheel (example: firstStringP;secondStringP;0;0;).
- The second string position is reserved for controlling the left wheel.
- Example of the all string: 50;-30;0;0;.
- The wheel control range shall be from -50 to 50.
- The ESP32-CAM shall transmit the full lever string on the serial bus at 5 Hz or better in a new line.
- All controllers shall be programmed using the Arduino IDE and installed independently.
- The rover shall use libraries from the provider for as long as possible.

## Connection Settings

- Wi-Fi network: Rover-ESP32
- Password: rovercontrol
- Address: http://192.168.4.1
- HTTP port: 80 (no port suffix is needed)
- Serial Monitor baud rate: 115200

## Sources and Libraries

- ESP32-CAM: https://github.com/espressif/esp32-camera/tree/master

## Project Folder Tree

```text
Rover_WS25/
├── docs/
│   └── Rover_Capability.md
├── firmware/
│   ├── esp32/
│   │   └── ESP32_main.ino
│   └── uno_r3/
│       └── UNO_R3_main.ino
├── libraries/
│   ├── esp32-camera-master/
│   └── third_party/
└── README.md
```

