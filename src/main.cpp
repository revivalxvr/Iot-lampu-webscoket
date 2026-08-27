#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// ======================================================
// WIFI
// ======================================================

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";


// ======================================================
// HARDWARE
// ======================================================

#define LED_PIN 26


// ======================================================
// SERVER
// ======================================================

WebServer server(80);
WebSocketsServer webSocket(81);


// ======================================================
// APPLICATION STATE
// ======================================================

// Tanaman yang dipilih
String selectedPlant = "sawi";

// Mode cahaya
// "auto" atau "manual"
String lightMode = "manual";

// Status LED
bool ledState = false;

// Durasi Auto dalam milliseconds
// Default = 5 detik
unsigned long autoDuration = 5000;

// Waktu terakhir LED berubah
unsigned long autoLastChange = 0;


// ======================================================
// HTML WEBSITE
// ======================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html lang="id">

<head>

    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1.0"
    >

    <title>UDAWA System</title>


    <style>

        * {
            box-sizing: border-box;
        }


        body {

            margin: 0;

            padding: 40px 20px;

            font-family:
                Arial,
                Helvetica,
                sans-serif;

            background: #f4f7f6;

            color: #222;

        }


        .container {

            width: 500px;

            max-width: 100%;

            margin: auto;

            background: white;

            padding: 30px;

            border-radius: 20px;

            box-shadow:
                0 10px 30px
                rgba(0, 0, 0, 0.08);

        }


        .title {

            text-align: center;

            margin-bottom: 30px;

        }


        .title h1 {

            margin: 0;

            font-size: 30px;

        }


        .title p {

            margin-top: 8px;

            color: #777;

        }


        .section {

            margin-top: 25px;

        }


        .section-title {

            font-size: 18px;

            font-weight: bold;

            margin-bottom: 12px;

        }


        .options {

            display: grid;

            grid-template-columns:
                repeat(2, 1fr);

            gap: 12px;

        }


        button {

            width: 100%;

            padding: 16px;

            border: none;

            border-radius: 12px;

            font-size: 16px;

            font-weight: bold;

            cursor: pointer;

            transition:
                transform 0.15s,
                opacity 0.15s;

        }


        button:hover {

            opacity: 0.85;

            transform: translateY(-2px);

        }


        button:active {

            transform:
                translateY(0);

        }


        .plant-button {

            background: #e8f5e9;

            color: #2e7d32;

        }


        .plant-button.active {

            background: #2e7d32;

            color: white;

        }


        .mode-button {

            background: #e3f2fd;

            color: #1565c0;

        }


        .mode-button.active {

            background: #1565c0;

            color: white;

        }


        .led-buttons {

            display: grid;

            grid-template-columns:
                repeat(2, 1fr);

            gap: 12px;

        }


        .on-button {

            background: #43a047;

            color: white;

        }


        .off-button {

            background: #e53935;

            color: white;

        }


        .status-box {

            margin-top: 25px;

            padding: 20px;

            border-radius: 15px;

            background: #f5f5f5;

            text-align: center;

        }


        .status-title {

            color: #777;

            font-size: 14px;

        }


        .led-status {

            margin-top: 8px;

            font-size: 28px;

            font-weight: bold;

        }


        .led-on {

            color: #43a047;

        }


        .led-off {

            color: #e53935;

        }


        /* ==========================================
           COUNTDOWN TIMER
           ========================================== */

        .countdown-box {

            margin-top: 15px;

            padding: 12px;

            background: #fff3e0;

            border-radius: 10px;

            color: #e65100;

            font-size: 18px;

            font-weight: bold;

            display: none;

        }


        .connection {

            margin-top: 20px;

            text-align: center;

            font-size: 14px;

            color: #777;

        }


        .connected {

            color: #43a047;

            font-weight: bold;

        }


        .disconnected {

            color: #e53935;

            font-weight: bold;

        }


        .info {

            margin-top: 20px;

            padding: 15px;

            background: #fafafa;

            border-radius: 10px;

            font-size: 14px;

        }


        .info-row {

            display: flex;

            justify-content:
                space-between;

            margin-bottom: 8px;

        }


        .info-row:last-child {

            margin-bottom: 0;

        }


        /* ==========================================
           AUTO DURATION
           ========================================== */

        .auto-duration-select {

            width: 100%;

            padding: 16px;

            border: none;

            border-radius: 12px;

            background: #fff3e0;

            color: #e65100;

            font-size: 16px;

            font-weight: bold;

            cursor: pointer;

            outline: none;

        }


    </style>

</head>


<body>


<div class="container">


    <!-- ========================================= -->
    <!-- TITLE -->
    <!-- ========================================= -->

    <div class="title">

        <h1>
            🌱 UDAWA Smart System
        </h1>

        <p>
            ESP32 WebSocket Control
        </p>

    </div>


    <!-- ========================================= -->
    <!-- PLANT SELECTION -->
    <!-- ========================================= -->

    <div class="section">

        <div class="section-title">

            Pilih Tanaman

        </div>


        <div class="options">


            <button
                id="plant-sawi"
                class="plant-button active"
                onclick="selectPlant('sawi')"
            >

                🌱 Sawi

            </button>


            <button
                id="plant-kangkung"
                class="plant-button"
                onclick="selectPlant('kangkung')"
            >

                🌿 Kangkung

            </button>


        </div>

    </div>


    <!-- ========================================= -->
    <!-- LIGHT MODE -->
    <!-- ========================================= -->

    <div class="section">

        <div class="section-title">

            Mode Cahaya

        </div>


        <div class="options">


            <button
                id="mode-auto"
                class="mode-button"
                onclick="selectMode('auto')"
            >

                🤖 Auto

            </button>


            <button
                id="mode-manual"
                class="mode-button active"
                onclick="selectMode('manual')"
            >

                🎛️ Manual

            </button>


        </div>

    </div>


    <!-- ========================================= -->
    <!-- AUTO DURATION -->
    <!-- ========================================= -->

    <div
        id="auto-duration-control"
        class="section"
        style="display:none"
    >

        <div class="section-title">

            Durasi Auto

        </div>


        <select
            id="auto-duration"
            class="auto-duration-select"
            onchange="setAutoDuration(this.value)"
        >

            <option value="5">
                5 detik
            </option>

            <option value="10">
                10 detik
            </option>

            <option value="15">
                15 detik
            </option>

            <option value="20">
                20 detik
            </option>

            <option value="25">
                25 detik
            </option>

            <option value="30">
                30 detik
            </option>

            <option value="35">
                35 detik
            </option>

            <option value="40">
                40 detik
            </option>

            <option value="45">
                45 detik
            </option>

            <option value="50">
                50 detik
            </option>

            <option value="55">
                55 detik
            </option>

            <option value="60">
                1 menit
            </option>

        </select>

    </div>


    <!-- ========================================= -->
    <!-- MANUAL LED CONTROL -->
    <!-- ========================================= -->

    <div
        id="manual-control"
        class="section"
    >

        <div class="section-title">

            Kontrol Lampu

        </div>


        <div class="led-buttons">


            <button
                class="on-button"
                onclick="setLed(true)"
            >

                💡 ON

            </button>


            <button
                class="off-button"
                onclick="setLed(false)"
            >

                🔴 OFF

            </button>


        </div>

    </div>


    <!-- ========================================= -->
    <!-- STATUS -->
    <!-- ========================================= -->

    <div class="status-box">

        <div class="status-title">

            STATUS LAMPU

        </div>


        <div
            id="led-status"
            class="led-status led-off"
        >

            💡 LAMPU MATI

        </div>


        <!-- TAMPILAN COUNTDOWN TIMER -->
        <div
            id="countdown-timer"
            class="countdown-box"
        >

            ⏱️ Sisa Waktu: <span id="timer-val">0</span> detik

        </div>

    </div>


    <!-- ========================================= -->
    <!-- INFORMATION -->
    <!-- ========================================= -->

    <div class="info">


        <div class="info-row">

            <span>
                Tanaman
            </span>

            <strong id="plant-status">
                Sawi
            </strong>

        </div>


        <div class="info-row">

            <span>
                Mode
            </span>

            <strong id="mode-status">
                Manual
            </strong>

        </div>


        <div
            class="info-row"
            id="duration-status-row"
            style="display:none"
        >

            <span>
                Durasi Auto
            </span>

            <strong id="duration-status">
                5 detik
            </strong>

        </div>


    </div>


    <!-- ========================================= -->
    <!-- CONNECTION -->
    <!-- ========================================= -->

    <div
        id="connection"
        class="connection disconnected"
    >

        🔴 WebSocket: Disconnected

    </div>


</div>


<script>


// ==================================================
// WEBSOCKET (Port asli 8181)
// ==================================================

const socket = new WebSocket(

    "ws://" +
    window.location.hostname +
    ":8181"

);

let countdownInterval = null;
let currentRemainingTime = 0;


// ==================================================
// WEBSOCKET CONNECTED
// ==================================================

socket.onopen = function() {

    console.log(
        "WebSocket connected"
    );


    const connection =
        document.getElementById(
            "connection"
        );


    connection.innerText =
        "🟢 WebSocket: Connected";


    connection.className =
        "connection connected";

};


// ==================================================
// RECEIVE MESSAGE
// ==================================================

socket.onmessage = function(event) {

    console.log(
        "Received:",
        event.data
    );


    try {

        const data =
            JSON.parse(
                event.data
            );


        updateUI(data);

    }

    catch (error) {

        console.error(
            "Invalid JSON:",
            error
        );

    }

};


// ==================================================
// WEBSOCKET DISCONNECTED
// ==================================================

socket.onclose = function() {

    console.log(
        "WebSocket disconnected"
    );


    const connection =
        document.getElementById(
            "connection"
        );


    connection.innerText =
        "🔴 WebSocket: Disconnected";


    connection.className =
        "connection disconnected";

};


// ==================================================
// COUNTDOWN TIMER LOGIC
// ==================================================

function startCountdown(seconds) {

    clearInterval(countdownInterval);
    currentRemainingTime = seconds;

    const timerBox =
        document.getElementById(
            "countdown-timer"
        );

    const timerVal =
        document.getElementById(
            "timer-val"
        );

    if (seconds > 0) {

        timerBox.style.display =
            "block";

        timerVal.innerText =
            currentRemainingTime;

        countdownInterval = setInterval(() => {

            currentRemainingTime--;

            if (currentRemainingTime <= 0) {

                clearInterval(countdownInterval);

                timerBox.style.display =
                    "none";

            } else {

                timerVal.innerText =
                    currentRemainingTime;

            }

        }, 1000);

    } else {

        timerBox.style.display =
            "none";

    }

}

function stopCountdown() {

    clearInterval(countdownInterval);

    document.getElementById(
        "countdown-timer"
    ).style.display = "none";

}


// ==================================================
// UPDATE UI
// ==================================================

function updateUI(data) {


    // ----------------------------------------------
    // LED & COUNTDOWN
    // ----------------------------------------------

    if (data.led === true) {

        document.getElementById(
            "led-status"
        ).innerText =
            "💡 LAMPU HIDUP";


        document.getElementById(
            "led-status"
        ).className =
            "led-status led-on";

        if (data.mode === "auto" && data.autoDuration) {

            startCountdown(
                Number(data.autoDuration)
            );

        }

    }

    else {

        document.getElementById(
            "led-status"
        ).innerText =
            "🔴 LAMPU MATI";


        document.getElementById(
            "led-status"
        ).className =
            "led-status led-off";

        stopCountdown();

    }


    // ----------------------------------------------
    // PLANT
    // ----------------------------------------------

    if (data.plant) {

        document.getElementById(
            "plant-status"
        ).innerText =
            capitalize(
                data.plant
            );


        document.getElementById(
            "plant-sawi"
        ).classList.remove(
            "active"
        );


        document.getElementById(
            "plant-kangkung"
        ).classList.remove(
            "active"
        );


        document.getElementById(
            "plant-" + data.plant
        ).classList.add(
            "active"
        );

    }


    // ----------------------------------------------
    // MODE
    // ----------------------------------------------

    if (data.mode) {

        document.getElementById(
            "mode-status"
        ).innerText =
            capitalize(
                data.mode
            );


        document.getElementById(
            "mode-auto"
        ).classList.remove(
            "active"
        );


        document.getElementById(
            "mode-manual"
        ).classList.remove(
            "active"
        );


        document.getElementById(
            "mode-" + data.mode
        ).classList.add(
            "active"
        );


        // ------------------------------------------
        // Manual control visibility
        // ------------------------------------------

        const manualControl =
            document.getElementById(
                "manual-control"
            );


        if (data.mode === "manual") {

            manualControl.style.display =
                "block";

            stopCountdown();

        }

        else {

            manualControl.style.display =
                "none";

        }


        // ------------------------------------------
        // Auto duration visibility
        // ------------------------------------------

        const autoDurationControl =
            document.getElementById(
                "auto-duration-control"
            );


        const durationStatusRow =
            document.getElementById(
                "duration-status-row"
            );


        if (data.mode === "auto") {

            autoDurationControl.style.display =
                "block";

            durationStatusRow.style.display =
                "flex";

        }

        else {

            autoDurationControl.style.display =
                "none";

            durationStatusRow.style.display =
                "none";

        }

    }


    // ----------------------------------------------
    // AUTO DURATION
    // ----------------------------------------------

    if (data.autoDuration) {

        const duration =
            Number(
                data.autoDuration
            );


        document.getElementById(
            "auto-duration"
        ).value =
            duration;


        const durationText =
            duration === 60
                ? "1 menit"
                : duration + " detik";


        document.getElementById(
            "duration-status"
        ).innerText =
            durationText;

    }

}


// ==================================================
// SELECT PLANT
// ==================================================

function selectPlant(plant) {


    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {

        console.log(
            "WebSocket belum terhubung"
        );

        return;

    }


    const message = {

        type: "plant",

        value: plant

    };


    socket.send(
        JSON.stringify(message)
    );

}


// ==================================================
// SELECT MODE
// ==================================================

function selectMode(mode) {


    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {

        console.log(
            "WebSocket belum terhubung"
        );

        return;

    }


    const message = {

        type: "mode",

        value: mode

    };


    socket.send(
        JSON.stringify(message)
    );

}


// ==================================================
// SET AUTO DURATION
// ==================================================

function setAutoDuration(seconds) {


    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {

        console.log(
            "WebSocket belum terhubung"
        );

        return;

    }


    const message = {

        type: "autoDuration",

        value: Number(seconds)

    };


    socket.send(
        JSON.stringify(message)
    );

}


// ==================================================
// SET LED
// ==================================================

function setLed(state) {


    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {

        console.log(
            "WebSocket belum terhubung"
        );

        return;

    }


    const message = {

        type: "led",

        value: state

    };


    socket.send(
        JSON.stringify(message)
    );

}


// ==================================================
// CAPITALIZE
// ==================================================

function capitalize(text) {

    return text.charAt(0).toUpperCase()
        + text.slice(1);

}


</script>


</body>

</html>

)rawliteral";


// ======================================================
// SEND STATUS
// ======================================================

void sendStatus() {

    String message = "{";

    message += "\"plant\":\"";
    message += selectedPlant;
    message += "\",";

    message += "\"mode\":\"";
    message += lightMode;
    message += "\",";

    message += "\"led\":";
    message += ledState
        ? "true"
        : "false";

    message += ",";

    message += "\"autoDuration\":";
    message += autoDuration / 1000;

    message += "}";


    // Kirim ke semua client
    webSocket.broadcastTXT(
        message
    );


    Serial.print(
        "[STATUS] "
    );

    Serial.println(
        message
    );
}


// ======================================================
// SEND STATUS TO ONE CLIENT
// ======================================================

void sendStatusToClient(
    uint8_t clientNum
) {

    String message = "{";

    message += "\"plant\":\"";
    message += selectedPlant;
    message += "\",";

    message += "\"mode\":\"";
    message += lightMode;
    message += "\",";

    message += "\"led\":";
    message += ledState
        ? "true"
        : "false";

    message += ",";

    message += "\"autoDuration\":";
    message += autoDuration / 1000;

    message += "}";


    webSocket.sendTXT(
        clientNum,
        message
    );

}


// ======================================================
// HANDLE WEBSOCKET EVENT
// ======================================================

void webSocketEvent(

    uint8_t clientNum,

    WStype_t type,

    uint8_t* payload,

    size_t length

) {


    switch (type) {


        // ==========================================
        // CLIENT DISCONNECTED
        // ==========================================

        case WStype_DISCONNECTED:

            Serial.printf(

                "[WS] Client %u disconnected\n",

                clientNum

            );

            break;


        // ==========================================
        // CLIENT CONNECTED
        // ==========================================

        case WStype_CONNECTED:

            Serial.printf(

                "[WS] Client %u connected\n",

                clientNum

            );


            // Kirim state saat ini
            // hanya ke client baru

            sendStatusToClient(
                clientNum
            );

            break;


        // ==========================================
        // TEXT MESSAGE
        // ==========================================

        case WStype_TEXT:

        {

            String message =
                String(
                    (char*)payload
                );


            Serial.print(
                "[WS] Received: "
            );

            Serial.println(
                message
            );


            // ======================================
            // PLANT - SAWI
            // ======================================

            if (
                message.indexOf(
                    "\"type\":\"plant\""
                ) >= 0
            ) {


                if (
                    message.indexOf(
                        "\"value\":\"sawi\""
                    ) >= 0
                ) {

                    selectedPlant =
                        "sawi";


                    Serial.println(
                        "[PLANT] Sawi selected"
                    );


                    sendStatus();

                }


                // ==================================
                // PLANT - KANGKUNG
                // ==================================

                else if (
                    message.indexOf(
                        "\"value\":\"kangkung\""
                    ) >= 0
                ) {

                    selectedPlant =
                        "kangkung";


                    Serial.println(
                        "[PLANT] Kangkung selected"
                    );


                    sendStatus();

                }

            }


            // ======================================
            // MODE
            // ======================================

            else if (
                message.indexOf(
                    "\"type\":\"mode\""
                ) >= 0
            ) {


                // ==================================
                // AUTO
                // ==================================

                if (
                    message.indexOf(
                        "\"value\":\"auto\""
                    ) >= 0
                ) {

                    lightMode =
                        "auto";


                    // Mulai siklus Auto
                    autoLastChange =
                        millis();


                    // Saat Auto dimulai,
                    // LED langsung ON

                    ledState =
                        true;


                    digitalWrite(
                        LED_PIN,
                        HIGH
                    );


                    Serial.println(
                        "[MODE] Auto"
                    );


                    sendStatus();

                }


                // ==================================
                // MANUAL
                // ==================================

                else if (
                    message.indexOf(
                        "\"value\":\"manual\""
                    ) >= 0
                ) {

                    lightMode =
                        "manual";


                    Serial.println(
                        "[MODE] Manual"
                    );


                    sendStatus();

                }

            }


            // ======================================
            // AUTO DURATION
            // ======================================

            else if (
                message.indexOf(
                    "\"type\":\"autoDuration\""
                ) >= 0
            ) {

                int duration = 0;


                // Cari posisi value
                int valuePos =
                    message.indexOf(
                        "\"value\":"
                    );


                if (valuePos >= 0) {

                    String valueString =
                        message.substring(
                            valuePos + 8
                        );


                    duration =
                        valueString.toInt();

                }


             

                if (
                    duration >= 5 &&
                    duration <= 60
                ) {

                    autoDuration =
                        (unsigned long)duration
                        * 1000UL;


                    // Reset timer & nyalakan lampu
                    autoLastChange =
                        millis();

                    ledState =
                        true;

                    digitalWrite(
                        LED_PIN,
                        HIGH
                    );


                    Serial.print(
                        "[AUTO] Duration: "
                    );


                    Serial.print(
                        duration
                    );


                    Serial.println(
                        " seconds"
                    );


                    sendStatus();

                }

                else {

                    Serial.println(
                        "[AUTO] Invalid duration"
                    );

                }

            }


            else if (
                message.indexOf(
                    "\"type\":\"led\""
                ) >= 0
            ) {


                if (
                    message.indexOf(
                        "\"value\":true"
                    ) >= 0
                ) {

                    if (
                        lightMode ==
                        "manual"
                    ) {

                        ledState =
                            true;


                        digitalWrite(
                            LED_PIN,
                            HIGH
                        );


                        Serial.println(
                            "[LED] ON"
                        );


                        sendStatus();

                    }

                    else {

                        Serial.println(
                            "[LED] Ignored - Auto mode"
                        );

                    }

                }


                // ----------------------------------
                // LED OFF
                // ----------------------------------

                else if (
                    message.indexOf(
                        "\"value\":false"
                    ) >= 0
                ) {


                    if (
                        lightMode ==
                        "manual"
                    ) {

                        ledState =
                            false;


                        digitalWrite(
                            LED_PIN,
                            LOW
                        );


                        Serial.println(
                            "[LED] OFF"
                        );


                        sendStatus();

                    }

                    else {

                        Serial.println(
                            "[LED] Ignored - Auto mode"
                        );

                    }

                }

            }


            break;

        }


        default:

            break;

    }

}


// ======================================================
// HTTP ROOT
// ======================================================

void handleRoot() {

    server.send(

        200,

        "text/html",

        INDEX_HTML

    );

}


// ======================================================
// SETUP
// ======================================================

void setup() {


    // ==================================================
    // SERIAL
    // ==================================================

    Serial.begin(
        115200
    );


    delay(1000);


    Serial.println();
    Serial.println(
        "================================="
    );

    Serial.println(
        "UDAWA Smart System"
    );

    Serial.println(
        "================================="
    );


    // ==================================================
    // LED
    // ==================================================

    pinMode(
        LED_PIN,
        OUTPUT
    );


    digitalWrite(
        LED_PIN,
        LOW
    );


    // ==================================================
    // WIFI
    // ==================================================

    WiFi.begin(

        WIFI_SSID,

        WIFI_PASSWORD

    );


    Serial.print(
        "Connecting to WiFi"
    );


    while (
        WiFi.status() !=
        WL_CONNECTED
    ) {

        delay(500);

        Serial.print(
            "."
        );

    }


    Serial.println();

    Serial.println(
        "WiFi connected!"
    );


    Serial.print(
        "IP Address: "
    );


    Serial.println(
        WiFi.localIP()
    );


    // ==================================================
    // HTTP SERVER
    // ==================================================

    server.on(

        "/",

        handleRoot

    );


    server.begin();


    Serial.println(
        "HTTP Server started"
    );


    // ==================================================
    // WEBSOCKET SERVER
    // ==================================================

    webSocket.begin();


    webSocket.onEvent(

        webSocketEvent

    );


    Serial.println(
        "WebSocket Server started"
    );


    Serial.println(
        "WebSocket Port: 81"
    );


    Serial.println(
        "================================="
    );

}


// ======================================================
// LOOP
// ======================================================

void loop() {


    // HTTP
    server.handleClient();


    // WebSocket
    webSocket.loop();


    // ==================================================
    // AUTO LIGHT TIMER (Nyala 1x Lalu Mati Total)
    // ==================================================

    if (
        lightMode == "auto" &&
        ledState == true
    ) {

        unsigned long currentMillis =
            millis();


        if (
            currentMillis -
            autoLastChange >=
            autoDuration
        ) {

            // Matikan LED
            ledState = false;

            digitalWrite(
                LED_PIN,
                LOW
            );


            Serial.println(
                "[AUTO] Timer Selesai - LED MATI"
            );

            // Update status ke interface
            sendStatus();

        }

    }

}