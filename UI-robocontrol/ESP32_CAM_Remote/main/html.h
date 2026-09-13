#pragma once

const char MAIN_PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
  >

  <title>ESP32-CAM Fernbedienung</title>

  <style>

    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 20px;
    }

    h1 {
      margin-bottom: 10px;
    }

    .camera {
      margin: 0 auto 20px auto;
    }

    .camera img {
      width: min(420px, 90vw);
      border: 2px solid #333;
      border-radius: 8px;
    }


    /* ------------------------------
       Kreuzförmige Steuerung
       ------------------------------ */

    .direction-pad {
      display: grid;
      grid-template-columns: 90px 90px 90px;
      grid-template-rows: 70px 70px 70px;

      justify-content: center;
      gap: 8px;

      margin: 20px auto;
    }

    .up {
      grid-column: 2;
      grid-row: 1;
    }

    .left {
      grid-column: 1;
      grid-row: 2;
    }

    .stop {
      grid-column: 2;
      grid-row: 2;
    }

    .right {
      grid-column: 3;
      grid-row: 2;
    }

    .down {
      grid-column: 2;
      grid-row: 3;
    }


    /* ------------------------------
       Allgemeine Buttons
       ------------------------------ */

    button {
      width: 100%;
      height: 100%;

      font-size: 20px;
      cursor: pointer;
    }

    .aux-buttons {
      display: flex;
      justify-content: center;
      gap: 10px;

      margin-top: 20px;
      flex-wrap: wrap;
    }

    .aux-buttons button {
      width: 120px;
      height: 55px;
    }

  </style>
</head>


<body>

  <h1>ESP32-CAM Fernbedienung</h1>


  <!-- ============================
       Kamera
       ============================ -->

  <div class="camera">

    <img
      src="{{STREAM_URL}}"
      alt="Live-Bild der ESP32-CAM"
    >

  </div>


  <!-- ============================
       Kreuzförmige Steuerung
       ============================ -->

  <div class="direction-pad">

    <div class="up">
      <a href="/move/forward">
        <button>▲</button>
      </a>
    </div>

    <div class="left">
      <a href="/move/left">
        <button>◀</button>
      </a>
    </div>

    <div class="stop">
      <a href="/move/stop">
        <button>■</button>
      </a>
    </div>

    <div class="right">
      <a href="/move/right">
        <button>▶</button>
      </a>
    </div>

    <div class="down">
      <a href="/move/backward">
        <button>▼</button>
      </a>
    </div>

  </div>


  <!-- ============================
       Weitere Funktionen
       ============================ -->

  <h2>Weitere Funktionen</h2>

  <div class="aux-buttons">

    <div>
      <p>LED</p>
      <p>Status: {{LED_STATE}}</p>
      {{LED_BUTTON}}
    </div>

    <div>
      <p>AUX 2</p>
      <a href="/aux2">
        <button>AUX 2</button>
      </a>
    </div>

    <div>
      <p>AUX 3</p>
      <a href="/aux3">
        <button>AUX 3</button>
      </a>
    </div>

  </div>
    
  <br>
	<a href=":81/stream">Go to port 81 - :81/stream</a>

</body>

</html>
)rawliteral";
