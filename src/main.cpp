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
// LED PIN
// ======================================================

// Sawi    -> LED merah  -> GPIO 26
// Kangkung -> LED kuning -> GPIO 17

#define SAWI_LED_PIN 26
#define KANGKUNG_LED_PIN 17

// ======================================================
// SERVER
// ======================================================

WebServer server(80);
WebSocketsServer webSocket(81);

// ======================================================
// TANAMAN YANG DIPILIH
// ======================================================

String selectedPlant = "sawi";

// ======================================================
// STATUS MASING-MASING LED
// ======================================================

bool sawiLedState = false;
bool kangkungLedState = false;

// ======================================================
// DURASI MASING-MASING LED
// Default 5 detik
// ======================================================

unsigned long sawiDuration = 5000;
unsigned long kangkungDuration = 5000;

// ======================================================
// WAKTU LED MULAI MENYALA
// ======================================================

unsigned long sawiStartTime = 0;
unsigned long kangkungStartTime = 0;

// ======================================================
// MODE MASING-MASING LED
//
// "manual"  = ON/OFF manual
// "duration" = menyala selama durasi lalu mati
// ======================================================

String sawiMode = "manual";
String kangkungMode = "manual";

// ======================================================
// HELPER
// ======================================================

int getSelectedPin() {

    if (selectedPlant == "kangkung") {
        return KANGKUNG_LED_PIN;
    }

    return SAWI_LED_PIN;
}

bool getSelectedLedState() {

    if (selectedPlant == "kangkung") {
        return kangkungLedState;
    }

    return sawiLedState;
}

String getSelectedMode() {

    if (selectedPlant == "kangkung") {
        return kangkungMode;
    }

    return sawiMode;
}

unsigned long getSelectedDuration() {

    if (selectedPlant == "kangkung") {
        return kangkungDuration;
    }

    return sawiDuration;
}

// ======================================================
// HTML
// ======================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html lang="id">

<head>

    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1"
    >

    <title>UDAWA Smart System</title>

    <!-- Pico CSS -->
    <link
        rel="stylesheet"
        href="https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css"
    >

    <style>

        body {
            background: #f5f7f5;
        }

        main {
            max-width: 720px;
            margin: auto;
            padding: 20px;
        }

        header {
            text-align: center;
            margin-bottom: 30px;
        }

        header h1 {
            margin-bottom: 5px;
        }

        header p {
            color: #777;
            margin-top: 0;
        }

        .plant-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
        }

        .plant-card {
            cursor: pointer;
            text-align: center;
            padding: 20px;
            border: 2px solid transparent;
            transition: 0.2s;
        }

        .plant-card.active {
            border-color: #2e7d32;
            background: #edf7ee;
        }

        .plant-icon {
            font-size: 35px;
            margin-bottom: 5px;
        }

        .lamp-status {
            text-align: center;
            padding: 25px;
        }

        .lamp-icon {
            font-size: 55px;
            margin-bottom: 10px;
            filter: grayscale(1);
            opacity: 0.35;
        }

        .lamp-icon.on {
            filter: none;
            opacity: 1;
        }

        .status-on {
            color: #2e7d32;
        }

        .status-off {
            color: #777;
        }

        .timer {
            font-size: 28px;
            font-weight: bold;
            color: #e65100;
        }

        .control-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
        }

        .duration-grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 8px;
        }

        .duration-grid button {
            margin: 0;
            padding: 10px 5px;
            font-size: 14px;
        }

        .duration-grid button.active {
            background: #2e7d32;
            border-color: #2e7d32;
        }

        .info-row {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            border-bottom: 1px solid #eee;
        }

        .info-row:last-child {
            border-bottom: none;
        }

        .connection {
            text-align: center;
            font-size: 14px;
        }

        .connected {
            color: #2e7d32;
        }

        .disconnected {
            color: #c62828;
        }

        @media (max-width: 600px) {

            .plant-grid {
                grid-template-columns: 1fr;
            }

            .control-grid {
                grid-template-columns: 1fr;
            }

            .duration-grid {
                grid-template-columns: repeat(3, 1fr);
            }

        }

    </style>

</head>


<body>

<main>

    <!-- ========================================= -->
    <!-- HEADER -->
    <!-- ========================================= -->

    <header>

        <h1>🌱 UDAWA</h1>

        <p>
            UDAWA Smart System
        </p>

    </header>


    <!-- ========================================= -->
    <!-- PILIH TANAMAN -->
    <!-- ========================================= -->

    <article>

        <h3>Pilih Tanaman</h3>

        <div class="plant-grid">

            <article
                id="plant-sawi"
                class="plant-card active"
                onclick="selectPlant('sawi')"
            >

                <div class="plant-icon">
                    🌱
                </div>

                <strong>
                    Sawi
                </strong>

                <small>
                    Lampu Merah
                </small>

            </article>


            <article
                id="plant-kangkung"
                class="plant-card"
                onclick="selectPlant('kangkung')"
            >

                <div class="plant-icon">
                    🌿
                </div>

                <strong>
                    Kangkung
                </strong>

                <small>
                    Lampu Kuning
                </small>

            </article>

        </div>

    </article>


    <!-- ========================================= -->
    <!-- STATUS LAMPU -->
    <!-- ========================================= -->

    <article class="lamp-status">

        <div
            id="lamp-icon"
            class="lamp-icon"
        >
            💡
        </div>

        <h2 id="plant-name">
            Sawi
        </h2>

        <h3
            id="lamp-status"
            class="status-off"
        >
            LAMPU MATI
        </h3>


        <div
            id="timer-box"
            style="display:none"
        >

            <small>
                Lampu akan mati dalam
            </small>

            <div
                id="timer"
                class="timer"
            >
                0 detik
            </div>

        </div>

    </article>


    <!-- ========================================= -->
    <!-- MODE MANUAL -->
    <!-- ========================================= -->

    <article>

        <h3>Kontrol Manual</h3>

        <p>
            Nyalakan atau matikan lampu secara langsung.
        </p>

        <div class="control-grid">

            <button
                onclick="manualOn()"
            >
                💡 ON
            </button>

            <button
                class="secondary"
                onclick="manualOff()"
            >
                ⛔ OFF
            </button>

        </div>

    </article>


    <!-- ========================================= -->
    <!-- DURASI -->
    <!-- ========================================= -->

    <article>

        <h3>Nyala Berdurasi</h3>

        <p>
            Lampu akan menyala sekali selama durasi
            yang dipilih, lalu mati.
        </p>


        <div class="duration-grid">

            <button
                id="duration-5"
                onclick="setDuration(5)"
            >
                5s
            </button>

            <button
                id="duration-10"
                onclick="setDuration(10)"
            >
                10s
            </button>

            <button
                id="duration-15"
                onclick="setDuration(15)"
            >
                15s
            </button>

            <button
                id="duration-20"
                onclick="setDuration(20)"
            >
                20s
            </button>

            <button
                id="duration-30"
                onclick="setDuration(30)"
            >
                30s
            </button>

            <button
                id="duration-40"
                onclick="setDuration(40)"
            >
                40s
            </button>

            <button
                id="duration-50"
                onclick="setDuration(50)"
            >
                50s
            </button>

            <button
                id="duration-60"
                onclick="setDuration(60)"
            >
                1 menit
            </button>

        </div>

    </article>


    <!-- ========================================= -->
    <!-- INFORMASI -->
    <!-- ========================================= -->

    <article>

        <h3>Informasi</h3>

        <div class="info-row">

            <span>
                Tanaman
            </span>

            <strong id="info-plant">
                Sawi
            </strong>

        </div>


        <div class="info-row">

            <span>
                GPIO
            </span>

            <strong id="info-pin">
                26
            </strong>

        </div>


        <div class="info-row">

            <span>
                Mode
            </span>

            <strong id="info-mode">
                Manual
            </strong>

        </div>


        <div class="info-row">

            <span>
                Durasi
            </span>

            <strong id="info-duration">
                5 detik
            </strong>

        </div>

    </article>


    <!-- ========================================= -->
    <!-- CONNECTION -->
    <!-- ========================================= -->

    <div
        id="connection"
        class="connection disconnected"
    >
        🔴 WebSocket Disconnected
    </div>

</main>


<script>

// ==================================================
// WEBSOCKET
// PORT TETAP 81
// ==================================================

const socket = new WebSocket(
    "ws://" +
    window.location.hostname +
    ":8181"
);

let countdownInterval = null;
let currentRemainingTime = 0;


// ==================================================
// CONNECTED
// ==================================================

socket.onopen = function() {

    const connection =
        document.getElementById(
            "connection"
        );

    connection.innerText =
        "🟢 WebSocket Connected";

    connection.className =
        "connection connected";

};


// ==================================================
// MESSAGE
// ==================================================

socket.onmessage = function(event) {

    try {

        const data =
            JSON.parse(event.data);

        updateUI(data);

    }

    catch(error) {

        console.error(
            "Invalid JSON",
            error
        );

    }

};


// ==================================================
// DISCONNECTED
// ==================================================

socket.onclose = function() {

    const connection =
        document.getElementById(
            "connection"
        );

    connection.innerText =
        "🔴 WebSocket Disconnected";

    connection.className =
        "connection disconnected";

};


// ==================================================
// PILIH TANAMAN
// ==================================================

function selectPlant(plant) {

    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {
        return;
    }

    socket.send(
        JSON.stringify({
            type: "plant",
            value: plant
        })
    );

}


// ==================================================
// MANUAL ON
// ==================================================

function manualOn() {

    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {
        return;
    }

    socket.send(
        JSON.stringify({
            type: "manual",
            value: true
        })
    );

}


// ==================================================
// MANUAL OFF
// ==================================================

function manualOff() {

    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {
        return;
    }

    socket.send(
        JSON.stringify({
            type: "manual",
            value: false
        })
    );

}


// ==================================================
// SET DURASI
// ==================================================

function setDuration(seconds) {

    if (
        socket.readyState !==
        WebSocket.OPEN
    ) {
        return;
    }

    socket.send(
        JSON.stringify({
            type: "duration",
            value: Number(seconds)
        })
    );

}


// ==================================================
// COUNTDOWN
// ==================================================

function startCountdown(seconds) {

    clearInterval(
        countdownInterval
    );

    currentRemainingTime =
        seconds;

    const timerBox =
        document.getElementById(
            "timer-box"
        );

    const timer =
        document.getElementById(
            "timer"
        );

    if (seconds <= 0) {

        timerBox.style.display =
            "none";

        return;

    }

    timerBox.style.display =
        "block";

    timer.innerText =
        currentRemainingTime +
        " detik";


    countdownInterval =
        setInterval(
            function() {

                currentRemainingTime--;

                if (
                    currentRemainingTime <= 0
                ) {

                    clearInterval(
                        countdownInterval
                    );

                    timerBox.style.display =
                        "none";

                }

                else {

                    timer.innerText =
                        currentRemainingTime +
                        " detik";

                }

            },
            1000
        );

}


// ==================================================
// STOP COUNTDOWN
// ==================================================

function stopCountdown() {

    clearInterval(
        countdownInterval
    );

    document.getElementById(
        "timer-box"
    ).style.display =
        "none";

}


// ==================================================
// UPDATE UI
// ==================================================

function updateUI(data) {

    // ----------------------------------------------
    // TANAMAN
    // ----------------------------------------------

    if (data.plant) {

        const plant =
            data.plant;

        const plantName =
            plant === "sawi"
                ? "Sawi"
                : "Kangkung";

        const gpio =
            plant === "sawi"
                ? "26"
                : "17";


        document.getElementById(
            "plant-name"
        ).innerText =
            plantName;


        document.getElementById(
            "info-plant"
        ).innerText =
            plantName;


        document.getElementById(
            "info-pin"
        ).innerText =
            gpio;


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
            "plant-" + plant
        ).classList.add(
            "active"
        );

    }


    // ----------------------------------------------
    // LED STATUS
    // ----------------------------------------------

    if (
        typeof data.led !==
        "undefined"
    ) {

        const status =
            document.getElementById(
                "lamp-status"
            );

        const icon =
            document.getElementById(
                "lamp-icon"
            );


        if (data.led === true) {

            status.innerText =
                "LAMPU HIDUP";

            status.className =
                "status-on";


            icon.classList.add(
                "on"
            );

        }

        else {

            status.innerText =
                "LAMPU MATI";

            status.className =
                "status-off";


            icon.classList.remove(
                "on"
            );

            stopCountdown();

        }

    }


    // ----------------------------------------------
    // MODE
    // ----------------------------------------------

    if (data.mode) {

        document.getElementById(
            "info-mode"
        ).innerText =
            data.mode === "duration"
                ? "Durasi"
                : "Manual";

    }


    // ----------------------------------------------
    // DURASI
    // ----------------------------------------------

    if (data.duration) {

        const duration =
            Number(data.duration);


        document.getElementById(
            "info-duration"
        ).innerText =
            duration === 60
                ? "1 menit"
                : duration + " detik";


        // Hilangkan active semua
        document
            .querySelectorAll(
                ".duration-grid button"
            )
            .forEach(
                button => {
                    button.classList.remove(
                        "active"
                    );
                }
            );


        const selectedButton =
            document.getElementById(
                "duration-" +
                duration
            );


        if (selectedButton) {

            selectedButton.classList.add(
                "active"
            );

        }

    }


    // ----------------------------------------------
    // COUNTDOWN
    // ----------------------------------------------

    if (
        data.led === true &&
        data.mode === "duration" &&
        data.remaining > 0
    ) {

        startCountdown(
            Number(data.remaining)
        );

    }

}


// ==================================================
// DEFAULT DURASI
// ==================================================

document.getElementById(
    "duration-5"
).classList.add(
    "active"
);

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

    message += "\"led\":";
    message += getSelectedLedState()
        ? "true"
        : "false";

    message += ",";

    message += "\"mode\":\"";
    message += getSelectedMode();
    message += "\",";

    message += "\"duration\":";
    message += getSelectedDuration() / 1000;

    message += ",";

    // ----------------------------------------------
    // Hitung sisa waktu
    // ----------------------------------------------

    unsigned long remaining = 0;

    if (
        getSelectedMode() == "duration" &&
        getSelectedLedState()
    ) {

        unsigned long elapsed =
            millis() -
            (
                selectedPlant == "sawi"
                    ? sawiStartTime
                    : kangkungStartTime
            );

        unsigned long duration =
            getSelectedDuration();

        if (elapsed < duration) {

            remaining =
                (duration - elapsed + 999) /
                1000;

        }

    }

    message += "\"remaining\":";
    message += remaining;

    message += "}";


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
// SEND STATUS TO CLIENT
// ======================================================

void sendStatusToClient(
    uint8_t clientNum
) {

    String message = "{";

    message += "\"plant\":\"";
    message += selectedPlant;
    message += "\",";

    message += "\"led\":";
    message += getSelectedLedState()
        ? "true"
        : "false";

    message += ",";

    message += "\"mode\":\"";
    message += getSelectedMode();
    message += "\",";

    message += "\"duration\":";
    message += getSelectedDuration() / 1000;

    message += ",";


    unsigned long remaining = 0;

    if (
        getSelectedMode() == "duration" &&
        getSelectedLedState()
    ) {

        unsigned long elapsed =
            millis() -
            (
                selectedPlant == "sawi"
                    ? sawiStartTime
                    : kangkungStartTime
            );

        unsigned long duration =
            getSelectedDuration();

        if (elapsed < duration) {

            remaining =
                (duration - elapsed + 999) /
                1000;

        }

    }


    message += "\"remaining\":";
    message += remaining;

    message += "}";


    webSocket.sendTXT(
        clientNum,
        message
    );

}


// ======================================================
// WEBSOCKET EVENT
// ======================================================

void webSocketEvent(

    uint8_t clientNum,

    WStype_t type,

    uint8_t* payload,

    size_t length

) {

    switch (type) {


        // ==========================================
        // DISCONNECTED
        // ==========================================

        case WStype_DISCONNECTED:

            Serial.printf(
                "[WS] Client %u disconnected\n",
                clientNum
            );

            break;


        // ==========================================
        // CONNECTED
        // ==========================================

        case WStype_CONNECTED:

            Serial.printf(
                "[WS] Client %u connected\n",
                clientNum
            );

            sendStatusToClient(
                clientNum
            );

            break;


        // ==========================================
        // TEXT
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
            // SELECT PLANT
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
            // MANUAL CONTROL
            // ======================================

            else if (
                message.indexOf(
                    "\"type\":\"manual\""
                ) >= 0
            ) {

                bool state =
                    message.indexOf(
                        "\"value\":true"
                    ) >= 0;


                if (
                    selectedPlant ==
                    "sawi"
                ) {

                    sawiMode =
                        "manual";

                    sawiLedState =
                        state;

                    digitalWrite(
                        SAWI_LED_PIN,
                        state
                            ? HIGH
                            : LOW
                    );

                }

                else {

                    kangkungMode =
                        "manual";

                    kangkungLedState =
                        state;

                    digitalWrite(
                        KANGKUNG_LED_PIN,
                        state
                            ? HIGH
                            : LOW
                    );

                }


                Serial.print(
                    "[MANUAL] "
                );

                Serial.print(
                    selectedPlant
                );

                Serial.print(
                    " -> "
                );

                Serial.println(
                    state
                        ? "ON"
                        : "OFF"
                );


                sendStatus();

            }


            // ======================================
            // DURASI
            // ======================================

            else if (
                message.indexOf(
                    "\"type\":\"duration\""
                ) >= 0
            ) {

                int duration =
                    0;


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


                // Pastikan 5 - 60 detik
                if (
                    duration >= 5 &&
                    duration <= 60
                ) {

                    unsigned long durationMs =
                        (unsigned long)duration *
                        1000UL;


                    if (
                        selectedPlant ==
                        "sawi"
                    ) {

                        sawiDuration =
                            durationMs;

                        sawiMode =
                            "duration";

                        sawiLedState =
                            true;

                        sawiStartTime =
                            millis();

                        digitalWrite(
                            SAWI_LED_PIN,
                            HIGH
                        );

                    }

                    else {

                        kangkungDuration =
                            durationMs;

                        kangkungMode =
                            "duration";

                        kangkungLedState =
                            true;

                        kangkungStartTime =
                            millis();

                        digitalWrite(
                            KANGKUNG_LED_PIN,
                            HIGH
                        );

                    }


                    Serial.print(
                        "[DURATION] "
                    );

                    Serial.print(
                        selectedPlant
                    );

                    Serial.print(
                        " -> "
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
                        "[DURATION] Invalid duration"
                    );

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

    // ==============================================
    // SERIAL
    // ==============================================

    Serial.begin(
        115200
    );

    delay(1000);


    Serial.println();

    Serial.println(
        "================================="
    );

    Serial.println(
        "UDAWA Smart Plant Lighting"
    );

    Serial.println(
        "================================="
    );


    // ==============================================
    // LED
    // ==============================================

    pinMode(
        SAWI_LED_PIN,
        OUTPUT
    );

    pinMode(
        KANGKUNG_LED_PIN,
        OUTPUT
    );


    digitalWrite(
        SAWI_LED_PIN,
        LOW
    );

    digitalWrite(
        KANGKUNG_LED_PIN,
        LOW
    );


    // ==============================================
    // WIFI
    // ==============================================

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


    // ==============================================
    // HTTP SERVER
    // PORT 80
    // ==============================================

    server.on(
        "/",
        handleRoot
    );

    server.begin();


    Serial.println(
        "HTTP Server started"
    );

    Serial.println(
        "HTTP Port: 80"
    );


    // ==============================================
    // WEBSOCKET SERVER
    // PORT 81
    // ==============================================

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

    // ==============================================
    // HTTP
    // ==============================================

    server.handleClient();


    // ==============================================
    // WEBSOCKET
    // ==============================================

    webSocket.loop();


    unsigned long currentMillis =
        millis();


    // ==================================================
    // TIMER SAWI
    // ==================================================

    if (
        sawiMode == "duration" &&
        sawiLedState == true
    ) {

        if (
            currentMillis -
            sawiStartTime >=
            sawiDuration
        ) {

            sawiLedState =
                false;

            sawiMode =
                "manual";

            digitalWrite(
                SAWI_LED_PIN,
                LOW
            );


            Serial.println(
                "[TIMER] Sawi selesai -> OFF"
            );


            sendStatus();

        }

    }


    // ==================================================
    // TIMER KANGKUNG
    // ==================================================

    if (
        kangkungMode == "duration" &&
        kangkungLedState == true
    ) {

        if (
            currentMillis -
            kangkungStartTime >=
            kangkungDuration
        ) {

            kangkungLedState =
                false;

            kangkungMode =
                "manual";

            digitalWrite(
                KANGKUNG_LED_PIN,
                LOW
            );


            Serial.println(
                "[TIMER] Kangkung selesai -> OFF"
            );


            sendStatus();

        }

    }

}
