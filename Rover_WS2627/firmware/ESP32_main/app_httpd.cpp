/*
 * Minimal ESP-IDF HTTP server for the rover controls.
 * This file is compiled automatically with ESP32_main.ino by the Arduino IDE.
 */

#include <Arduino.h>
#include <esp_http_server.h>

#include "index.html"

extern void setDriveCommand(int rightWheel, int leftWheel);

namespace {

constexpr int MIN_DRIVE_VALUE = -50;
constexpr int MAX_DRIVE_VALUE = 50;

int clampDriveValue(long value) {
  if (value < MIN_DRIVE_VALUE) return MIN_DRIVE_VALUE;
  if (value > MAX_DRIVE_VALUE) return MAX_DRIVE_VALUE;
  return static_cast<int>(value);
}

bool parseDriveValue(const char *text, int *value) {
  if (text == nullptr || *text == '\0') return false;

  char *end = nullptr;
  const long parsed = strtol(text, &end, 10);
  if (*end != '\0') return false;

  *value = clampDriveValue(parsed);
  return true;
}

esp_err_t indexHandler(httpd_req_t *request) {
  httpd_resp_set_type(request, "text/html; charset=utf-8");
  return httpd_resp_send(request, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

esp_err_t controlHandler(httpd_req_t *request) {
  const size_t queryLength = httpd_req_get_url_query_len(request);
  if (queryLength == 0 || queryLength >= 64) {
    return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Expected right and left values");
  }

  char query[64];
  char rightText[8];
  char leftText[8];
  if (httpd_req_get_url_query_str(request, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "right", rightText, sizeof(rightText)) != ESP_OK ||
      httpd_query_key_value(query, "left", leftText, sizeof(leftText)) != ESP_OK) {
    return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Expected right and left values");
  }

  int rightWheel = 0;
  int leftWheel = 0;
  if (!parseDriveValue(rightText, &rightWheel) || !parseDriveValue(leftText, &leftWheel)) {
    return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Drive values must be integers");
  }

  setDriveCommand(rightWheel, leftWheel);
  return httpd_resp_send(request, nullptr, 0);
}

const httpd_uri_t INDEX_URI = {
  .uri = "/",
  .method = HTTP_GET,
  .handler = indexHandler,
  .user_ctx = nullptr
};

const httpd_uri_t CONTROL_URI = {
  .uri = "/control",
  .method = HTTP_GET,
  .handler = controlHandler,
  .user_ctx = nullptr
};

}  // namespace

void startHttpServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 2;

  httpd_handle_t server = nullptr;
  if (httpd_start(&server, &config) != ESP_OK) {
    return;
  }

  if (httpd_register_uri_handler(server, &INDEX_URI) != ESP_OK ||
      httpd_register_uri_handler(server, &CONTROL_URI) != ESP_OK) {
    httpd_stop(server);
  }
}
