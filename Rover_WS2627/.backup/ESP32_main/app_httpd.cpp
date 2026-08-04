/*
 * Minimal ESP-IDF HTTP server for the rover controls.
 * This file is compiled automatically with ESP32_main.ino by the Arduino IDE.
 */

#include <Arduino.h>
#include <esp_http_server.h>

extern void setDriveCommand(int rightWheel, int leftWheel);

namespace {

constexpr int MIN_DRIVE_VALUE = -50;
constexpr int MAX_DRIVE_VALUE = 50;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
  <title>Rover control</title>
  <style>
    :root { color-scheme: dark; font-family: system-ui, sans-serif; }
    body { margin: 0; min-height: 100svh; background: #101820; color: #f5f7fa;
           display: grid; place-items: center; }
    main { width: min(94vw, 42rem); text-align: center; }
    h1 { font-size: 1.35rem; margin: 0 0 1.5rem; }
    .controls { display: flex; justify-content: space-evenly; gap: 1rem; }
    .lever { display: grid; justify-items: center; gap: .75rem; font-weight: 700; }
    output { min-width: 3ch; font-variant-numeric: tabular-nums; color: #7ee787; }
    input[type=range] { appearance: slider-vertical; -webkit-appearance: slider-vertical;
                         width: 3.2rem; height: min(55vh, 22rem); accent-color: #58a6ff;
                         touch-action: none; }
    .hint { margin-top: 1.5rem; color: #9da7b1; font-size: .9rem; }
  </style>
</head>
<body>
  <main>
    <h1>Rover control</h1>
    <section class="controls" aria-label="Wheel controls">
      <label class="lever">LEFT WHEELS <output id="left-value">0</output>
        <input id="left" type="range" min="-5" max="5" value="0" step="1"
               aria-label="Left wheels: forward is up, reverse is down">
      </label>
      <label class="lever">RIGHT WHEELS <output id="right-value">0</output>
        <input id="right" type="range" min="-5" max="5" value="0" step="1"
               aria-label="Right wheels: forward is up, reverse is down">
      </label>
    </section>
    <p class="hint">Up: forward &middot; down: reverse &middot; release: stop</p>
  </main>
  <script>
    const levers = {
      left: document.querySelector('#left'),
      right: document.querySelector('#right')
    };

    function driveValue(lever) { return Number(lever.value) * 10; }

    function updateDisplay(name) {
      document.querySelector(`#${name}-value`).value = driveValue(levers[name]);
    }

    function sendCommand() {
      const right = driveValue(levers.right);
      const left = driveValue(levers.left);
      fetch(`/control?right=${right}&left=${left}`, { cache: 'no-store' }).catch(() => {});
    }

    function resetLever(name) {
      if (levers[name].value !== '0') {
        levers[name].value = 0;
        updateDisplay(name);
        sendCommand();
      }
    }

    for (const [name, lever] of Object.entries(levers)) {
      lever.addEventListener('input', () => {
        updateDisplay(name);
        sendCommand();
      });
      lever.addEventListener('pointerup', () => resetLever(name));
      lever.addEventListener('pointercancel', () => resetLever(name));
      lever.addEventListener('lostpointercapture', () => resetLever(name));
      updateDisplay(name);
    }

    // If the control page is backgrounded, stop both sides before it disappears.
    document.addEventListener('visibilitychange', () => {
      if (document.hidden) {
        levers.left.value = 0;
        levers.right.value = 0;
        updateDisplay('left');
        updateDisplay('right');
        fetch('/control?right=0&left=0', { cache: 'no-store', keepalive: true }).catch(() => {});
      }
    });
  </script>
</body>
</html>
)rawliteral";

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
