#include <Arduino.h>
#include <esp32-hal-psram.h>

#include "camera.h"
#include "config.h"

#include "esp_camera.h"
#include "esp_http_server.h"

static httpd_handle_t streamHttpd = NULL;

static const char* STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=frame";

static const char* STREAM_BOUNDARY =
    "\r\n--frame\r\n";

static const char* STREAM_PART =
    "Content-Type: image/jpeg\r\n"
    "Content-Length: %zu\r\n\r\n";


void cameraBegin() {

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk  = XCLK_GPIO_NUM;
  config.pin_pclk  = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href  = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn  = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;

  // Für das Streaming:
  config.pixel_format = PIXFORMAT_JPEG;

  // Kleines Bild -> weniger Daten -> höhere Bildrate
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;

  if (psramFound()) {
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  }


  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf(
      "Kamera-Initialisierung fehlgeschlagen: 0x%x\n",
      err
    );

    while (true) {
      delay(1000);
    }
  }

  Serial.println("Kamera erfolgreich initialisiert.");
}


// ----------------------------------------------------
// Einzelnes JPEG-Frame als MJPEG-Teil senden
// ----------------------------------------------------

static esp_err_t streamHandler(httpd_req_t* request) {

  esp_err_t result =
      httpd_resp_set_type(
        request,
        STREAM_CONTENT_TYPE
      );

  if (result != ESP_OK) {
    return result;
  }


  while (true) {

    camera_fb_t* fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Kameraaufnahme fehlgeschlagen.");
      return ESP_FAIL;
    }


    char partHeader[64];

    int headerLength = snprintf(
      partHeader,
      sizeof(partHeader),
      STREAM_PART,
      fb->len
    );


    result = httpd_resp_send_chunk(
      request,
      STREAM_BOUNDARY,
      strlen(STREAM_BOUNDARY)
    );

    if (result == ESP_OK) {

      result = httpd_resp_send_chunk(
        request,
        partHeader,
        headerLength
      );
    }

    if (result == ESP_OK) {

      result = httpd_resp_send_chunk(
        request,
        (const char*)fb->buf,
        fb->len
      );
    }


    esp_camera_fb_return(fb);


    if (result != ESP_OK) {
      Serial.println("Browser hat den Stream beendet.");
      break;
    }

    delay(10);
  }

  return result;
}


// ----------------------------------------------------
// Streaming-Webserver auf Port 81
// ----------------------------------------------------

void cameraStreamBegin() {

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.server_port = STREAM_PORT;

  httpd_uri_t streamUri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = streamHandler,
    .user_ctx  = NULL
  };


  esp_err_t result =
      httpd_start(&streamHttpd, &config);

  if (result == ESP_OK) {

    httpd_register_uri_handler(
      streamHttpd,
      &streamUri
    );

    Serial.print(
      "Kamera-Stream gestartet auf Port "
    );
    Serial.println(STREAM_PORT);

  } else {

    Serial.printf(
      "Stream-Webserver konnte nicht gestartet werden: 0x%x\n",
      result
    );
  }
}
