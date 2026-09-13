#pragma once

#include <Arduino.h>
#include <stdint.h>

// ==============================
// WLAN
// ==============================

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// ==============================
// Webserver
// ==============================

constexpr uint16_t UI_PORT = 80;
constexpr uint16_t STREAM_PORT = 81;


// ==============================
// AI Thinker ESP32-CAM
// ==============================

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM      26
#define SIOC_GPIO_NUM      27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// AI-Thinker: eingebauter Flash-LED-Pin
#define LED_PIN            4
