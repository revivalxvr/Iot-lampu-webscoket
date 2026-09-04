#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// ======================================================
// WIFI
// ======================================================

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ======================================================
// PIN RELAY
// ======================================================

#define RELAY_A_PIN 18
#define RELAY_B_PIN 17

// ======================================================
// WEB SERVER & WEBSOCKET
// ======================================================

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// ======================================================
// RELAY STATE
// ======================================================

bool relayAState = false;
bool relayBState = false;

// ======================================================
// TIMER
// ======================================================

unsigned long relayAOffTime = 0;
unsigned long relayBOffTime = 0;

// ======================================================
// HTML
// ======================================================

const char MAIN_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="id">

<head>

    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1.0"
    >

    <title>ESP32 Relay Control</title>

    <link
        rel="stylesheet"
        href="https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css"
    >

    <style>

        body {
            max-width: 1100px;
            margin: auto;
            padding: 20px;
        }

        header {
            margin-bottom: 30px;
        }

        .status {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 6px 12px;
            border-radius: 20px;
            background: #f1f1f1;
            font-size: 14px;
        }

        .status-dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: #999;
        }

        .connected .status-dot {
            background: #20a464;
        }

        .relay-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 20px;
        }

        .relay-card {
            border: 1px solid #ddd;
            border-radius: 12px;
            padding: 20px;
        }

        .relay-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .relay-title {
            font-size: 1.3rem;
            font-weight: bold;
        }

        .relay-badge {
            padding: 5px 12px;
            border-radius: 20px;
            font-size: 13px;
            background: #eee;
        }

        .relay-badge.on {
            background: #d1fae5;
            color: #047857;
        }

        .relay-badge.off {
            background: #fee2e2;
            color: #b91c1c;
        }

        .remaining {
            display: flex;
            justify-content: space-between;
            padding: 10px;
            margin-top: 15px;
            border-radius: 8px;
            background: #f5f5f5;
        }

        .button-group {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }

        .timer-info {
            color: #666;
            font-size: 14px;
        }

        @media (max-width: 700px) {

            .relay-grid {
                grid-template-columns: 1fr;
            }

        }

    </style>

</head>

<body>

<header>

    <h1>Relay Control Lampu UDAWA System</h1>

    <p>
        Kontrol Relay menggunakan WebSocket
    </p>

    <span id="connectionStatus" class="status">

        <span class="status-dot"></span>

        <span id="connectionText">
            Connecting...
        </span>

    </span>

</header>


<main>

<div class="relay-grid">

    <!-- ==================================================
         RELAY A
         ================================================== -->

    <article class="relay-card">

        <div class="relay-header">

            <span class="relay-title">
               Lampu Sayur Kangkung
            </span>

            <span
                id="relayABadge"
                class="relay-badge off"
            >
                OFF
            </span>

        </div>


        <label>
            Status

            <select id="relayAToggle">

                <option value="off">
                    OFF
                </option>

                <option value="on">
                    ON
                </option>

            </select>

        </label>


        <label>
            Mode

            <select id="relayAMode">

                <option value="manual">
                    Manual
                </option>

                <option value="duration">
                    Durasi (Auto OFF)
                </option>

            </select>

        </label>


        <label>
            Durasi

            <select id="relayADuration">

                <option value="5">
                    5 detik
                </option>

                <option value="10">
                    10 detik
                </option>

                <option value="30">
                    30 detik
                </option>

                <option value="60">
                    1 menit
                </option>

                <option value="300">
                    5 menit
                </option>

            </select>

        </label>


        <p class="timer-info">
            Pilih mode Durasi untuk mematikan relay
            secara otomatis setelah waktu habis.
        </p>


        <div class="button-group">

            <button
                onclick="turnOn('A')"
            >
                Nyalakan
            </button>

            <button
                class="secondary"
                onclick="turnOff('A')"
            >
                Matikan
            </button>

        </div>


        <div class="remaining">

            <span>
                Sisa Waktu
            </span>

            <strong id="relayARemaining">
                0 detik
            </strong>

        </div>

    </article>


    <!-- ==================================================
         RELAY B
         ================================================== -->

    <article class="relay-card">

        <div class="relay-header">

            <span class="relay-title">
               Lampu Sayur Sawi
            </span>

            <span
                id="relayBBadge"
                class="relay-badge off"
            >
                OFF
            </span>

        </div>


        <label>
            Status

            <select id="relayBToggle">

                <option value="off">
                    OFF
                </option>

                <option value="on">
                    ON
                </option>

            </select>

        </label>


        <label>
            Mode

            <select id="relayBMode">

                <option value="manual">
                    Manual
                </option>

                <option value="duration">
                    Durasi (Auto OFF)
                </option>

            </select>

        </label>


        <label>
            Durasi

            <select id="relayBDuration">

                <option value="5">
                    5 detik
                </option>

                <option value="10">
                    10 detik
                </option>

                <option value="30">
                    30 detik
                </option>

                <option value="60">
                    1 menit
                </option>

                <option value="300">
                    5 menit
                </option>

            </select>

        </label>


        <p class="timer-info">
            Pilih mode Durasi untuk mematikan relay
            secara otomatis setelah waktu habis.
        </p>


        <div class="button-group">

            <button
                onclick="turnOn('B')"
            >
                Nyalakan
            </button>

            <button
                class="secondary"
                onclick="turnOff('B')"
            >
                Matikan
            </button>

        </div>


        <div class="remaining">

            <span>
                Sisa Waktu
            </span>

            <strong id="relayBRemaining">
                0 detik
            </strong>

        </div>

    </article>

</div>


<article style="margin-top: 25px;">

    <h3>ℹ️ Cara Penggunaan</h3>

    <ul>

        <li>
            Pilih <strong>Manual</strong> untuk
            mengontrol relay tanpa timer.
        </li>

        <li>
            Pilih <strong>Durasi</strong> untuk
            menggunakan timer otomatis.
        </li>

        <li>
            Klik <strong>Nyalakan</strong> untuk
            menghidupkan relay.
        </li>

        <li>
            Setelah durasi habis, relay akan
            otomatis OFF.
        </li>

        <li>
            Relay A dan Relay B memiliki timer
            yang berjalan secara independen.
        </li>

    </ul>

</article>

</main>


<footer style="text-align:center; margin-top:30px;">

    <small>
        ESP32 + WebSocket + Relay Control
    </small>

</footer>


<script>

let socket;


// ======================================================
// CONNECT WEBSOCKET
// ======================================================

function connectWebSocket() {

    socket = new WebSocket(
        "ws://" + window.location.hostname + ":8181"
    );


    socket.onopen = function() {

        console.log("WebSocket Connected");

        updateConnection(true);

    };


    socket.onclose = function() {

        console.log("WebSocket Disconnected");

        updateConnection(false);

        setTimeout(connectWebSocket, 2000);

    };


    socket.onerror = function(error) {

        console.log("WebSocket Error:", error);

    };


    socket.onmessage = function(event) {

        try {

            const data = JSON.parse(event.data);

            updateRelay(data);

        } catch (error) {

            console.error(
                "Invalid JSON:",
                event.data
            );

        }

    };

}


// ======================================================
// CONNECTION STATUS
// ======================================================

function updateConnection(connected) {

    const status =
        document.getElementById(
            "connectionStatus"
        );

    const text =
        document.getElementById(
            "connectionText"
        );


    if (connected) {

        status.classList.add("connected");

        text.innerText = "Connected";

    } else {

        status.classList.remove("connected");

        text.innerText = "Disconnected";

    }

}


// ======================================================
// UPDATE RELAY
// ======================================================

function updateRelay(data) {

    updateSingleRelay(
        "A",
        data.relayA,
        data.remainingA
    );

    updateSingleRelay(
        "B",
        data.relayB,
        data.remainingB
    );

}


// ======================================================
// UPDATE SINGLE RELAY
// ======================================================

function updateSingleRelay(
    relay,
    state,
    remaining
) {

    const badge =
        document.getElementById(
            "relay" + relay + "Badge"
        );

    const toggle =
        document.getElementById(
            "relay" + relay + "Toggle"
        );

    const remainingElement =
        document.getElementById(
            "relay" + relay + "Remaining"
        );


    if (state) {

        badge.innerText = "ON";

        badge.classList.remove("off");

        badge.classList.add("on");

        toggle.value = "on";

    } else {

        badge.innerText = "OFF";

        badge.classList.remove("on");

        badge.classList.add("off");

        toggle.value = "off";

    }


    if (remaining > 0) {

        remainingElement.innerText =
            remaining + " detik";

    } else {

        remainingElement.innerText =
            "0 detik";

    }

}


// ======================================================
// SEND COMMAND
// ======================================================

function sendCommand(
    relay,
    action
) {

    if (
        !socket ||
        socket.readyState !== WebSocket.OPEN
    ) {

        alert(
            "WebSocket belum terhubung."
        );

        return;

    }


    const mode =
        document.getElementById(
            "relay" + relay + "Mode"
        ).value;


    const duration =
        parseInt(
            document.getElementById(
                "relay" + relay + "Duration"
            ).value
        );


    const data = {

        relay: relay,

        action: action,

        mode: mode,

        duration: duration

    };


    socket.send(
        JSON.stringify(data)
    );

}


// ======================================================
// TURN ON
// ======================================================

function turnOn(relay) {

    sendCommand(
        relay,
        "on"
    );

}


// ======================================================
// TURN OFF
// ======================================================

function turnOff(relay) {

    sendCommand(
        relay,
        "off"
    );

}


// ======================================================
// SELECT TOGGLE
// ======================================================

document
    .getElementById("relayAToggle")
    .addEventListener(
        "change",
        function() {

            if (this.value === "on") {

                turnOn("A");

            } else {

                turnOff("A");

            }

        }
    );


document
    .getElementById("relayBToggle")
    .addEventListener(
        "change",
        function() {

            if (this.value === "on") {

                turnOn("B");

            } else {

                turnOff("B");

            }

        }
    );


// ======================================================
// START
// ======================================================

connectWebSocket();

</script>

</body>

</html>

)rawliteral";


// ======================================================
// SET RELAY
// ======================================================

void setRelayA(bool state)
{
    relayAState = state;

    digitalWrite(
        RELAY_A_PIN,
        state ? HIGH : LOW
    );

    if (!state)
    {
        relayAOffTime = 0;
    }
}


void setRelayB(bool state)
{
    relayBState = state;

    digitalWrite(
        RELAY_B_PIN,
        state ? HIGH : LOW
    );

    if (!state)
    {
        relayBOffTime = 0;
    }
}


// ======================================================
// SEND RELAY STATUS
// ======================================================

void sendRelayStatus(uint8_t client = 255)
{
    JsonDocument doc;

    doc["relayA"] = relayAState;
    doc["relayB"] = relayBState;

    unsigned long now = millis();

    unsigned long remainingA = 0;
    unsigned long remainingB = 0;


    if (
        relayAState &&
        relayAOffTime > now
    )
    {
        remainingA =
            (relayAOffTime - now) / 1000;
    }


    if (
        relayBState &&
        relayBOffTime > now
    )
    {
        remainingB =
            (relayBOffTime - now) / 1000;
    }


    doc["remainingA"] = remainingA;
    doc["remainingB"] = remainingB;


    String output;

    serializeJson(
        doc,
        output
    );


    if (client == 255)
    {
        webSocket.broadcastTXT(output);
    }
    else
    {
        webSocket.sendTXT(
            client,
            output
        );
    }
}


// ======================================================
// WEBSOCKET EVENT
// ======================================================

void webSocketEvent(
    uint8_t num,
    WStype_t type,
    uint8_t* payload,
    size_t length
)
{

    switch (type)
    {

        // ==================================================
        // CLIENT CONNECT
        // ==================================================

        case WStype_CONNECTED:
        {

            IPAddress ip =
                webSocket.remoteIP(num);

            Serial.print(
                "WebSocket Client Connected: "
            );

            Serial.println(ip);

            sendRelayStatus(num);

            break;
        }


        // ==================================================
        // CLIENT DISCONNECT
        // ==================================================

        case WStype_DISCONNECTED:

            Serial.print(
                "WebSocket Client Disconnected: "
            );

            Serial.println(num);

            break;


        // ==================================================
        // RECEIVE MESSAGE
        // ==================================================

        case WStype_TEXT:
        {

            Serial.print(
                "WebSocket Message: "
            );

            Serial.println(
                (char*)payload
            );


            JsonDocument doc;

            DeserializationError error =
                deserializeJson(
                    doc,
                    payload,
                    length
                );


            if (error)
            {

                Serial.println(
                    "JSON parsing failed"
                );

                return;

            }


            const char* relay =
                doc["relay"];

            const char* action =
                doc["action"];

            const char* mode =
                doc["mode"];


            int duration =
                doc["duration"] | 0;


            // ==================================================
            // RELAY A
            // ==================================================

            if (
                relay != nullptr &&
                strcmp(relay, "A") == 0
            )
            {

                // ------------------------------------------
                // OFF
                // ------------------------------------------

                if (
                    action != nullptr &&
                    strcmp(action, "off") == 0
                )
                {

                    setRelayA(false);

                    Serial.println(
                        "Relay A -> OFF"
                    );

                }


                // ------------------------------------------
                // ON
                // ------------------------------------------

                else if (
                    action != nullptr &&
                    strcmp(action, "on") == 0
                )
                {

                    setRelayA(true);


                    // --------------------------------------
                    // MODE DURATION
                    // --------------------------------------

                    if (
                        mode != nullptr &&
                        strcmp(
                            mode,
                            "duration"
                        ) == 0 &&
                        duration > 0
                    )
                    {

                        relayAOffTime =
                            millis() +
                            (
                                (unsigned long)duration
                                * 1000UL
                            );


                        Serial.print(
                            "Relay A ON selama "
                        );

                        Serial.print(
                            duration
                        );

                        Serial.println(
                            " detik"
                        );

                    }


                    // --------------------------------------
                    // MODE MANUAL
                    // --------------------------------------

                    else
                    {

                        relayAOffTime = 0;

                        Serial.println(
                            "Relay A ON - Manual"
                        );

                    }

                }

            }


            // ==================================================
            // RELAY B
            // ==================================================

            if (
                relay != nullptr &&
                strcmp(relay, "B") == 0
            )
            {

                // ------------------------------------------
                // OFF
                // ------------------------------------------

                if (
                    action != nullptr &&
                    strcmp(action, "off") == 0
                )
                {

                    setRelayB(false);

                    Serial.println(
                        "Relay B -> OFF"
                    );

                }


                // ------------------------------------------
                // ON
                // ------------------------------------------

                else if (
                    action != nullptr &&
                    strcmp(action, "on") == 0
                )
                {

                    setRelayB(true);


                    // --------------------------------------
                    // MODE DURATION
                    // --------------------------------------

                    if (
                        mode != nullptr &&
                        strcmp(
                            mode,
                            "duration"
                        ) == 0 &&
                        duration > 0
                    )
                    {

                        relayBOffTime =
                            millis() +
                            (
                                (unsigned long)duration
                                * 1000UL
                            );


                        Serial.print(
                            "Relay B ON selama "
                        );

                        Serial.print(
                            duration
                        );

                        Serial.println(
                            " detik"
                        );

                    }


                    // --------------------------------------
                    // MODE MANUAL
                    // --------------------------------------

                    else
                    {

                        relayBOffTime = 0;

                        Serial.println(
                            "Relay B ON - Manual"
                        );

                    }

                }

            }


            // Kirim status terbaru
            sendRelayStatus();

            break;

        }


        default:

            break;

    }

}


// ======================================================
// WEB SERVER
// ======================================================

void handleRoot()
{
    server.send_P(
        200,
        "text/html",
        MAIN_HTML
    );
}


// ======================================================
// CHECK TIMER
// ======================================================

void checkTimers()
{

    unsigned long now =
        millis();


    // ==================================================
    // RELAY A
    // ==================================================

    if (
        relayAState &&
        relayAOffTime > 0 &&
        now >= relayAOffTime
    )
    {

        Serial.println(
            "Relay A timer selesai -> OFF"
        );

        setRelayA(false);

        sendRelayStatus();

    }


    // ==================================================
    // RELAY B
    // ==================================================

    if (
        relayBState &&
        relayBOffTime > 0 &&
        now >= relayBOffTime
    )
    {

        Serial.println(
            "Relay B timer selesai -> OFF"
        );

        setRelayB(false);

        sendRelayStatus();

    }

}


// ======================================================
// SETUP
// ======================================================

void setup()
{

    Serial.begin(115200);

    delay(1000);


    // ==================================================
    // RELAY
    // ==================================================

    pinMode(
        RELAY_A_PIN,
        OUTPUT
    );

    pinMode(
        RELAY_B_PIN,
        OUTPUT
    );


    // Pastikan relay OFF saat startup

    digitalWrite(
        RELAY_A_PIN,
        LOW
    );

    digitalWrite(
        RELAY_B_PIN,
        LOW
    );


    // ==================================================
    // WIFI
    // ==================================================

    Serial.println();

    Serial.println(
        "Connecting to WiFi..."
    );

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    while (
        WiFi.status() != WL_CONNECTED
    )
    {

        delay(500);

        Serial.print(".");

    }


    Serial.println();

    Serial.println(
        "WiFi Connected!"
    );

    Serial.print(
        "ESP32 IP Address: "
    );

    Serial.println(
        WiFi.localIP()
    );


    // ==================================================
    // WEB SERVER
    // ==================================================

    server.on(
        "/",
        HTTP_GET,
        handleRoot
    );

    server.begin();

    Serial.println(
        "HTTP Server started on port 80"
    );


    // ==================================================
    // WEBSOCKET
    // ==================================================

    webSocket.begin();

    webSocket.onEvent(
        webSocketEvent
    );

    Serial.println(
        "WebSocket started on port 81"
    );


    Serial.println();
    Serial.println(
        "================================"
    );
    Serial.println(
        " ESP32 RELAY CONTROL READY"
    );
    Serial.println(
        "================================"
    );
    Serial.println(
        "run on http://localhost:8180"
    );


}


void loop()
{

    server.handleClient();

    webSocket.loop();

    checkTimers();

    delay(10);

}