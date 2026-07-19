# Rover Capability

This document describes the capabilities of the rover, which components it has, and how the components interact with each other. 
This document also contains Engineering requirements.

## Components Overview

- The rover is equipped with 4 DC wheel motors
- The rover is equipped with a battery for the power source
- The rover uses an Arduino Uno R3
- The rover uses an ESP32-CAM
- The rover chassis is used to carry all components

## Components Interface

- The battery supplies the Arduino and ESP32 controller
- All 4 wheels are connected to the Arduino
- The ESP32 and Arduino are connected via the serial bus
- On Arduino, D0 and D1 are used for serial communication with the ESP32

## Capability

- The user shall be able to connect with the ESP32-CAM by using WiFi communication
- The ESP32-CAM shall Implementiert den HTTP-Server web interaction
- The ESP32-CAM shall provide an interface to control the rover forwards and backwards
- The web interface contains two levers: one to control the right wheels and one to control the left wheels
- The lever can be moved up for forwards and down for backwards
- The lever has a resolution of 5: from middle to up are 5 positions and from middle to down are 5 positions, for a total of 10 resolutions
- The lever shall move to the middle position when the user releases the lever
- The ESP32-CAM shall convert the position from the lever to -50 (down) to 50 (up)
- The ESP32-CAM shall provide a string on the serial bus when the user interacts with the web controller
- The string has no fixed length and is divided by using the semicolon ';'
- The first string is reserved to control the right wheels
- The second string is reserved to control the left wheels
- The range to control the wheel is -50 to 50
- This is an example of the string: 50;-30;0;0; 
- All controllers shall be programmed using the Arduino IDE software and installed independently
- the rover shall use, as long is possible, libriries from provider

## source and libriries

-ESP32-cam: https://github.com/espressif/esp32-camera/tree/master










