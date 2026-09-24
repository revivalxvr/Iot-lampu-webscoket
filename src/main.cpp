#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <time.h>


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
#define RELAY_C_PIN 27
#define RELAY_D_PIN 14


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
bool relayCState = false;
bool relayDState = false;


// ======================================================
// WITA TIME
// ======================================================

const long GMT_OFFSET_SEC = 8 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

const char* NTP_SERVER_1 = "pool.ntp.org";
const char* NTP_SERVER_2 = "time.nist.gov";


// ======================================================
// SCHEDULE
// ======================================================

enum ScheduleType
{
    SCHEDULE_NONE = 0,
    SCHEDULE_MORNING,
    SCHEDULE_AFTERNOON,
    SCHEDULE_NIGHT
};


ScheduleType relayASchedule = SCHEDULE_NONE;
ScheduleType relayBSchedule = SCHEDULE_NONE;
ScheduleType relayCSchedule = SCHEDULE_NONE;
ScheduleType relayDSchedule = SCHEDULE_NONE;


bool relayAAutoSchedule = false;
bool relayBAutoSchedule = false;
bool relayCAutoSchedule = false;
bool relayDAutoSchedule = false;


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

    <title>UDAWA Smart System</title>


    <style>

        * {
            box-sizing: border-box;
        }


        body {

            margin: 0;

            padding: 20px;

            min-height: 100vh;

            font-family:
                -apple-system,
                BlinkMacSystemFont,
                "Segoe UI",
                Roboto,
                Arial,
                sans-serif;

            background: #f4f7f5;

            color: #202124;
        }


        /* ==================================================
           MAIN CONTAINER
           ================================================== */

        .container {

            width: 100%;

            max-width: 780px;

            margin: 0 auto;

            background: #ffffff;

            border-radius: 30px;

            padding: 35px 45px 30px;

            box-shadow:
                0 10px 35px rgba(0, 0, 0, 0.08);
        }


        /* ==================================================
           HEADER
           ================================================== */

        header {

            text-align: center;

            margin-bottom: 35px;
        }


        .logo {

            display: flex;

            align-items: center;

            justify-content: center;

            gap: 12px;

            margin-bottom: 5px;
        }


        .logo-icon {

            font-size: 42px;

            line-height: 1;
        }


        .logo h1 {

            margin: 0;

            font-size: 36px;

            font-weight: 800;

            color: #202124;
        }


        .subtitle {

            margin: 5px 0 0;

            font-size: 18px;

            color: #777;
        }


        /* ==================================================
           SECTION
           ================================================== */

        .section {

            margin-top: 28px;
        }


        .section-title {

            margin: 0 0 14px;

            font-size: 22px;

            font-weight: 800;

            color: #202124;
        }


        /* ==================================================
           RELAY GRID
           ================================================== */

        .relay-grid {

            display: grid;

            grid-template-columns:
                repeat(2, 1fr);

            gap: 18px;
        }


        .relay-card {

            background: #f8faf8;

            border: 2px solid #e5eee6;

            border-radius: 22px;

            padding: 22px;

            transition:
                transform 0.2s ease,
                box-shadow 0.2s ease;
        }


        .relay-card:hover {

            transform: translateY(-2px);

            box-shadow:
                0 8px 20px rgba(0, 0, 0, 0.07);
        }


        /* ==================================================
           RELAY HEADER
           ================================================== */

        .relay-header {

            display: flex;

            justify-content: space-between;

            align-items: center;

            margin-bottom: 20px;
        }


        .plant-name {

            display: flex;

            align-items: center;

            gap: 8px;

            font-size: 20px;

            font-weight: 800;

            color: #287a32;
        }


        .plant-icon {

            font-size: 25px;
        }


        /* ==================================================
           BADGE
           ================================================== */

        .relay-badge {

            padding: 6px 12px;

            border-radius: 20px;

            font-size: 12px;

            font-weight: 800;

            letter-spacing: 0.5px;
        }


        .relay-badge.on {

            background: #d9f7df;

            color: #208538;
        }


        .relay-badge.off {

            background: #ffe0e0;

            color: #d83232;
        }


        /* ==================================================
           LAMP STATUS
           ================================================== */

        .lamp-status {

            display: flex;

            align-items: center;

            justify-content: center;

            gap: 12px;

            background: #ffffff;

            border-radius: 18px;

            padding: 18px;

            margin-bottom: 18px;

            border: 1px solid #eeeeee;
        }


        .lamp-dot {

            width: 20px;

            height: 20px;

            border-radius: 50%;

            background: #ed3d45;

            box-shadow:
                0 0 8px
                rgba(237, 61, 69, 0.25);
        }


        .lamp-dot.on {

            background: #38a849;

            box-shadow:
                0 0 12px
                rgba(56, 168, 73, 0.5);
        }


        .lamp-text {

            font-size: 19px;

            font-weight: 800;

            color: #e5363d;
        }


        .lamp-text.on {

            color: #2c963d;
        }


        /* ==================================================
           FIELD
           ================================================== */

        .field {

            margin-bottom: 15px;
        }


        .field label {

            display: block;

            margin-bottom: 7px;

            font-size: 14px;

            font-weight: 700;

            color: #555;
        }


        select {

            width: 100%;

            border: 2px solid #e1e6e2;

            border-radius: 12px;

            padding: 11px 13px;

            background: #ffffff;

            color: #333;

            font-size: 15px;

            outline: none;

            cursor: pointer;
        }


        select:focus {

            border-color: #34863d;
        }


        /* ==================================================
           BUTTON
           ================================================== */

        .button-group {

            display: grid;

            grid-template-columns:
                1fr 1fr;

            gap: 10px;

            margin-top: 18px;
        }


        .button-group button {

            border: none;

            border-radius: 13px;

            padding: 13px 10px;

            font-size: 16px;

            font-weight: 800;

            color: white;

            cursor: pointer;

            transition:
                transform 0.15s ease,
                opacity 0.15s ease;
        }


        .button-group button:hover {

            transform: translateY(-1px);

            opacity: 0.92;
        }


        .btn-on {

            background: #3da447;
        }


        .btn-off {

            background: #ed3838;
        }


        /* ==================================================
           REMAINING
           ================================================== */

        .remaining {

            display: flex;

            justify-content: space-between;

            align-items: center;

            margin-top: 15px;

            padding: 11px 14px;

            border-radius: 12px;

            background: #eef5ef;

            color: #555;

            font-size: 14px;
        }


        .remaining strong {

            color: #287a32;

            font-size: 15px;
        }


        /* ==================================================
           SCHEDULE STATUS
           ================================================== */

        .schedule-status {

            margin-top: 12px;

            padding: 10px 12px;

            border-radius: 10px;

            background: #f1f5ff;

            color: #52617a;

            font-size: 13px;

            text-align: center;
        }


        /* ==================================================
           CONNECTION
           ================================================== */

        .connection-box {

            display: flex;

            justify-content: center;

            align-items: center;

            gap: 8px;

            margin-top: 30px;

            font-size: 15px;

            font-weight: 700;

            color: #777;
        }


        .status-dot {

            width: 12px;

            height: 12px;

            border-radius: 50%;

            background: #999;
        }


        .connected .status-dot {

            background: #35b957;

            box-shadow:
                0 0 8px
                rgba(53, 185, 87, 0.5);
        }


        .connected #connectionText {

            color: #27953f;
        }


        /* ==================================================
           INFO
           ================================================== */

        .info-box {

            margin-top: 25px;

            padding: 18px;

            background: #f8f8f8;

            border-radius: 16px;

            color: #666;

            font-size: 13px;

            line-height: 1.6;
        }


        .info-box strong {

            color: #333;
        }


        /* ==================================================
           FOOTER
           ================================================== */

        footer {

            text-align: center;

            margin-top: 20px;

            color: #999;

            font-size: 12px;
        }


        /* ==================================================
           MOBILE
           ================================================== */

        @media (max-width: 650px) {

            body {

                padding: 10px;
            }


            .container {

                padding: 25px 18px;

                border-radius: 24px;
            }


            .logo h1 {

                font-size: 27px;
            }


            .logo-icon {

                font-size: 32px;
            }


            .subtitle {

                font-size: 15px;
            }


            .relay-grid {

                grid-template-columns: 1fr;
            }


            .section-title {

                font-size: 20px;
            }

        }

    </style>

</head>


<body>


<div class="container">


    <!-- ==================================================
         HEADER
         ================================================== -->

    <header>

        <div class="logo">

            <span class="logo-icon">
                🌱
            </span>

            <h1>
                UDAWA Smart System
            </h1>

        </div>


        <p class="subtitle">
            ESP32 WebSocket Control
        </p>

    </header>



    <!-- ==================================================
         TANAMAN
         ================================================== -->

    <div class="section">

        <h2 class="section-title">
            🌿 Kontrol Tanaman
        </h2>


        <div class="relay-grid">


            <!-- ==================================================
                 RELAY A - SELADA
                 ================================================== -->

            <article class="relay-card">


                <div class="relay-header">

                    <div class="plant-name">

                        <span class="plant-icon">
                         🥬
                        </span>

                        Selada

                    </div>


                    <span
                        id="relayABadge"
                        class="relay-badge off"
                    >
                        OFF
                    </span>

                </div>



                <!-- LAMP STATUS -->

                <div class="lamp-status">

                    <span
                        id="relayALampDot"
                        class="lamp-dot"
                    ></span>


                    <span
                        id="relayALampText"
                        class="lamp-text"
                    >
                        LAMPU MATI
                    </span>

                </div>



                <!-- STATUS -->

                <div class="field">

                    <label>
                        Status Lampu
                    </label>


                    <select id="relayAToggle">

                        <option value="off">
                            🔴 OFF
                        </option>


                        <option value="on">
                            🟢 ON
                        </option>

                    </select>

                </div>



                <!-- MODE -->

                <div class="field">

                    <label>
                        Mode Cahaya
                    </label>


                    <select id="relayAMode">

                        <option value="manual">
                             Manual
                        </option>


                        <option value="schedule">
                            🕐Jadwal Otomatis
                        </option>

                    </select>

                </div>



                <!-- SCHEDULE -->

                <div class="field">

                    <label>
                        Jadwal Lampu
                    </label>


                    <select id="relayASchedule">

                        <option value="1">
                            Siklus 1 — 06:00 - 11:00
                        </option>


                        <option value="2">
                            Siklus 2 — 12:00 - 17:00
                        </option>


                        <option value="3">
                            Siklus 3 — 18:00 - 00:00
                        </option>

                    </select>

                </div>



                <!-- BUTTON -->

                <div class="button-group">

                    <button
                        class="btn-on"
                        onclick="turnOn('A')"
                    >
                        💡 ON
                    </button>


                    <button
                        class="btn-off"
                        onclick="turnOff('A')"
                    >
                        🔴 OFF
                    </button>

                </div>



                <!-- STATUS JADWAL -->

                <div
                    id="relayAScheduleStatus"
                    class="schedule-status"
                >
                    Jadwal belum aktif
                </div>


            </article>



            <!-- ==================================================
                 RELAY B - Melon
                 ================================================== -->

            <article class="relay-card">


                <div class="relay-header">

                    <div class="plant-name">

                        <span class="plant-icon">
                            🍈
                        </span>

                        Melon

                    </div>


                    <span
                        id="relayBBadge"
                        class="relay-badge off"
                    >
                        OFF
                    </span>

                </div>



                <!-- LAMP STATUS -->

                <div class="lamp-status">

                    <span
                        id="relayBLampDot"
                        class="lamp-dot"
                    ></span>


                    <span
                        id="relayBLampText"
                        class="lamp-text"
                    >
                        LAMPU MATI
                    </span>

                </div>



                <!-- STATUS -->

                <div class="field">

                    <label>
                        Status Lampu
                    </label>


                    <select id="relayBToggle">

                        <option value="off">
                            🔴 OFF
                        </option>


                        <option value="on">
                            🟢 ON
                        </option>

                    </select>

                </div>



                <!-- MODE -->

                <div class="field">

                    <label>
                        Mode Cahaya
                    </label>


                    <select id="relayBMode">

                        <option value="manual">
                            Manual
                        </option>


                        <option value="schedule">
                             Jadwal Otomatis
                        </option>

                    </select>

                </div>



                <!-- SCHEDULE -->

                <div class="field">

                    <label>
                        Jadwal Lampu
                    </label>


                    <select id="relayBSchedule">

                        <option value="1">
                            Siklus 1 — 06:00 - 11:00
                        </option>


                        <option value="2">
                            Siklus 2 — 12:00 - 17:00
                        </option>


                        <option value="3">
                            Siklus 3 — 18:00 - 00:00
                        </option>

                    </select>

                </div>



                <!-- BUTTON -->

                <div class="button-group">

                    <button
                        class="btn-on"
                        onclick="turnOn('B')"
                    >
                        💡 ON
                    </button>


                    <button
                        class="btn-off"
                        onclick="turnOff('B')"
                    >
                        🔴 OFF
                    </button>

                </div>



                <!-- STATUS JADWAL -->

                <div
                    id="relayBScheduleStatus"
                    class="schedule-status"
                >
                    Jadwal belum aktif
                </div>


            </article>

            <!-- ==================================================
                 RELAY C - Tomat
                 ================================================== -->
            <article class="relay-card">


                <div class="relay-header">

                    <div class="plant-name">

                        <span class="plant-icon">
                            🍅
                        </span>

                        Tomat

                    </div>


                    <span
                        id="relayCBadge"
                        class="relay-badge off"
                    >
                        OFF
                    </span>

                </div>



                <!-- LAMP STATUS -->

                <div class="lamp-status">

                    <span
                        id="relayCLampDot"
                        class="lamp-dot"
                    ></span>


                    <span
                        id="relayCLampText"
                        class="lamp-text"
                    >
                        LAMPU MATI
                    </span>

                </div>



                <!-- STATUS -->

                <div class="field">

                    <label>
                        Status Lampu
                    </label>


                    <select id="relayCToggle">

                        <option value="off">
                            🔴 OFF
                        </option>


                        <option value="on">
                            🟢 ON
                        </option>

                    </select>

                </div>



                <!-- MODE -->

                <div class="field">

                    <label>
                        Mode Cahaya
                    </label>


                    <select id="relayCMode">

                        <option value="manual">
                            Manual
                        </option>


                        <option value="schedule">
                             Jadwal Otomatis
                        </option>

                    </select>

                </div>



                <!-- SCHEDULE -->

                <div class="field">

                    <label>
                        Jadwal Lampu
                    </label>


                    <select id="relayCSchedule">

                        <option value="1">
                            Siklus 1 — 06:00 - 11:00
                        </option>


                        <option value="2">
                            Siklus 2 — 12:00 - 17:00
                        </option>


                        <option value="3">
                            Siklus 3 — 18:00 - 00:00
                        </option>

                    </select>

                </div>



                <!-- BUTTON -->

                <div class="button-group">

                    <button
                        class="btn-on"
                        onclick="turnOn('C')"
                    >
                        💡 ON
                    </button>


                    <button
                        class="btn-off"
                        onclick="turnOff('C')"
                    >
                        🔴 OFF
                    </button>

                </div>



                <!-- STATUS JADWAL -->

                <div
                    id="relayCScheduleStatus"
                    class="schedule-status"
                >
                    Jadwal belum aktif
                </div>


            </article>

                <!-- ==================================================
                 RELAY D - Cabai
                 ================================================== -->
            <article class="relay-card">


                <div class="relay-header">

                    <div class="plant-name">

                        <span class="plant-icon">
                            🌶️
                        </span>

                        Cabai

                    </div>


                    <span
                        id="relayDBadge"
                        class="relay-badge off"
                    >
                        OFF
                    </span>

                </div>



                <!-- LAMP STATUS -->

                <div class="lamp-status">

                    <span
                        id="relayDLampDot"
                        class="lamp-dot"
                    ></span>


                    <span
                        id="relayDLampText"
                        class="lamp-text"
                    >
                        LAMPU MATI
                    </span>

                </div>



                <!-- STATUS -->

                <div class="field">

                    <label>
                        Status Lampu
                    </label>


                    <select id="relayDToggle">

                        <option value="off">
                            🔴 OFF
                        </option>


                        <option value="on">
                            🟢 ON
                        </option>

                    </select>

                </div>



                <!-- MODE -->

                <div class="field">

                    <label>
                        Mode Cahaya
                    </label>


                    <select id="relayDMode">

                        <option value="manual">
                            Manual
                        </option>


                        <option value="schedule">
                             Jadwal Otomatis
                        </option>

                    </select>

                </div>



                <!-- SCHEDULE -->

                <div class="field">

                    <label>
                        Jadwal Lampu
                    </label>


                    <select id="relayDSchedule">

                        <option value="1">
                            Siklus 1 — 06:00 - 11:00
                        </option>


                        <option value="2">
                            Siklus 2 — 12:00 - 17:00
                        </option>


                        <option value="3">
                            Siklus 3 — 18:00 - 00:00
                        </option>

                    </select>

                </div>



                <!-- BUTTON -->

                <div class="button-group">

                    <button
                        class="btn-on"
                        onclick="turnOn('D')"
                    >
                        💡 ON
                    </button>


                    <button
                        class="btn-off"
                        onclick="turnOff('D')"
                    >
                        🔴 OFF
                    </button>

                </div>



                <!-- STATUS JADWAL -->

                <div
                    id="relayDScheduleStatus"
                    class="schedule-status"
                >
                    Jadwal belum aktif
                </div>


            </article>


        </div>

    </div>



    <!-- ==================================================
         INFO
         ================================================== -->

    <div class="info-box">

        <strong>
            💡 Informasi Sistem
        </strong>

        <br><br>

        <strong>Manual:</strong>

        Lampu dikontrol langsung menggunakan tombol
        ON dan OFF.


        <br>

        <strong>Jadwal Otomatis:</strong>

        Lampu akan mengikuti jadwal WITA yang dipilih.


        <br>

        <strong>Siklus 1:</strong>
        06:00 - 11:00


        <br>

        <strong>Siklus 2:</strong>
        12:00 - 17:00


        <br>

        <strong>Siklus 3:</strong>
        18:00 - 00:00


        <br><br>

        Waktu sistem menggunakan
        <strong>WITA (UTC+8)</strong>.

    </div>



    <!-- ==================================================
         CONNECTION
         ================================================== -->

    <div
        id="connectionStatus"
        class="connection-box"
    >

        <span class="status-dot"></span>


        <span id="connectionText">
            Connecting...
        </span>

    </div>



    <footer>

        ESP32 + WebSocket + Relay Control

    </footer>


</div>



<script>

    let socket;


    // ======================================================
    // CONNECT WEBSOCKET
    // ======================================================

    function connectWebSocket() {

        socket = new WebSocket(
            "ws://" +
            window.location.hostname +
            ":8181"
        );


        socket.onopen = function() {

            console.log(
                "WebSocket Connected"
            );

            updateConnection(true);

        };


        socket.onclose = function() {

            console.log(
                "WebSocket Disconnected"
            );

            updateConnection(false);

            setTimeout(
                connectWebSocket,
                2000
            );

        };


        socket.onerror = function(error) {

            console.log(
                "WebSocket Error:",
                error
            );

        };


        socket.onmessage = function(event) {

            try {

                const data =
                    JSON.parse(
                        event.data
                    );

                updateRelay(data);

            }
            catch (error) {

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

            status.classList.add(
                "connected"
            );


            text.innerText =
                "WebSocket: Connected";

        }
        else {

            status.classList.remove(
                "connected"
            );


            text.innerText =
                "WebSocket: Disconnected";

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


        updateSingleRelay(
            "C",
            data.relayC,
            data.remainingC
        );
        updateSingleRelay(
            "D",
            data.relayD,
            data.remainingD
        );
        

    }



    // ======================================================
    // GET SCHEDULE NAME
    // ======================================================

    function getScheduleName(schedule) {

        if (schedule === "1") {

            return "Siklus 1 — 06:00 - 11:00";

        }


        if (schedule === "2") {

            return "Siklus 2 — 12:00 - 17:00";

        }


        if (schedule === "3") {

            return "Siklus 3 — 18:00 - 00:00";

        }


        return "Jadwal belum aktif";

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
                "relay" +
                relay +
                "Badge"
            );


        const toggle =
            document.getElementById(
                "relay" +
                relay +
                "Toggle"
            );


        if (state) {

            badge.innerText = "ON";

            badge.classList.remove(
                "off"
            );

            badge.classList.add(
                "on"
            );

            toggle.value = "on";

        }
        else {

            badge.innerText = "OFF";

            badge.classList.remove(
                "on"
            );

            badge.classList.add(
                "off"
            );

            toggle.value = "off";

        }


        const lampDot =
            document.getElementById(
                "relay" +
                relay +
                "LampDot"
            );


        const lampText =
            document.getElementById(
                "relay" +
                relay +
                "LampText"
            );


        if (state) {

            lampDot.classList.add("on");

            lampText.classList.add("on");

            lampText.innerText =
                "LAMPU MENYALA";

        }
        else {

            lampDot.classList.remove("on");

            lampText.classList.remove("on");

            lampText.innerText =
                "LAMPU MATI";

        }


        /*
         * Sistem sekarang tidak menggunakan
         * timer durasi.
         *
         * Nilai remaining tetap diterima
         * supaya struktur JSON lama tetap
         * kompatibel.
         */

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
            socket.readyState !==
            WebSocket.OPEN
        ) {

            alert(
                "WebSocket belum terhubung."
            );

            return;

        }


        const mode =
            document.getElementById(
                "relay" +
                relay +
                "Mode"
            ).value;


        const schedule =
            parseInt(
                document.getElementById(
                    "relay" +
                    relay +
                    "Schedule"
                ).value
            );


        const data = {

            relay: relay,

            action: action,

            mode: mode,

            schedule: schedule

        };


        socket.send(
            JSON.stringify(data)
        );


        /*
         * Update informasi jadwal
         * pada UI.
         */

        const scheduleStatus =
            document.getElementById(
                "relay" +
                relay +
                "ScheduleStatus"
            );


        if (
            action === "on" &&
            mode === "schedule"
        ) {

            scheduleStatus.innerText =
                "Jadwal aktif: " +
                getScheduleName(
                    String(schedule)
                );

        }
        else if (
            action === "off"
        ) {

            scheduleStatus.innerText =
                "Jadwal belum aktif";

        }
        else {

            scheduleStatus.innerText =
                "Mode Manual aktif";

        }

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
    // SELECT TOGGLE A
    // ======================================================

    document
        .getElementById(
            "relayAToggle"
        )
        .addEventListener(
            "change",
            function() {

                if (
                    this.value === "on"
                ) {

                    turnOn("A");

                }
                else {

                    turnOff("A");

                }

            }
        );



    // ======================================================
    // SELECT TOGGLE B
    // ======================================================

    document
        .getElementById(
            "relayBToggle"
        )
        .addEventListener(
            "change",
            function() {

                if (
                    this.value === "on"
                ) {

                    turnOn("B");

                }
                else {

                    turnOff("B");

                }

            }
        );
    // ======================================================
    // SELECT TOGGLE C
    // ======================================================
      document
        .getElementById(
            "relayCToggle"
        )
        .addEventListener(
            "change",
            function() {

                if (
                    this.value === "on"
                ) {

                    turnOn("C");

                }
                else {

                    turnOff("C");

                }

            }
        );
    // ======================================================
    // SELECT TOGGLE D
    // ======================================================
          document
        .getElementById(
            "relayDToggle"
        )
        .addEventListener(
            "change",
            function() {

                if (
                    this.value === "on"
                ) {

                    turnOn("D");

                }
                else {

                    turnOff("D");

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
// SET RELAY A
// ======================================================

void setRelayA(bool state)
{
    relayAState = state;

    digitalWrite(
        RELAY_A_PIN,
        state ? HIGH : LOW
    );
}


// ======================================================
// SET RELAY B
// ======================================================

void setRelayB(bool state)
{
    relayBState = state;

    digitalWrite(
        RELAY_B_PIN,
        state ? HIGH : LOW
    );
}
void setRelayC(bool state)
{
    relayCState = state;

    digitalWrite(
        RELAY_C_PIN,
        state ? HIGH : LOW
    );
}
void setRelayD(bool state)
{
    relayDState = state;

    digitalWrite(
        RELAY_D_PIN,
        state ? HIGH : LOW
    );
}

// ======================================================
// SEND RELAY STATUS
// ======================================================

void sendRelayStatus(
    uint8_t client = 255
)
{
    JsonDocument doc;

    doc["relayA"] = relayAState;
    doc["relayB"] = relayBState;
    doc["relayC"] = relayCState;
    doc["relayD"] = relayDState;


    /*
     * Tetap kirim remainingA/B agar
     * struktur response tetap sederhana
     * dan kompatibel dengan frontend.
     */

    doc["remainingA"] = 0;
    doc["remainingB"] = 0;
    doc["remainingC"] = 0;
    doc["remainingD"] = 0;
 


    /*
     * Kirim informasi schedule.
     */

    doc["scheduleA"] =
        (int)relayASchedule;

    doc["scheduleB"] =
        (int)relayBSchedule;

    doc["scheduleC"] =
        (int)relayCSchedule;

    doc["scheduleD"] =
        (int)relayDSchedule;


    doc["autoA"] =
        relayAAutoSchedule;

    doc["autoB"] =
        relayBAutoSchedule;

    doc["autoC"] =
        relayCAutoSchedule;

    doc["autoD"] =
        relayDAutoSchedule;

    String output;


    serializeJson(
        doc,
        output
    );


    if (client == 255)
    {
        webSocket.broadcastTXT(
            output
        );
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
// CHECK CURRENT SCHEDULE
// ======================================================

bool isScheduleActive(
    ScheduleType schedule
)
{
    struct tm timeinfo;


    if (!getLocalTime(&timeinfo))
    {
        Serial.println(
            "Gagal mendapatkan waktu WITA"
        );

        return false;
    }


    int hour =
        timeinfo.tm_hour;


    switch (schedule)
    {

        // ==================================================
        // PAGI
        // 06:00 - 11:00
        // ==================================================

        case SCHEDULE_MORNING:

            return (
                hour >= 6 &&
                hour < 11
            );


        // ==================================================
        // SIANG
        // 12:00 - 17:00
        // ==================================================

        case SCHEDULE_AFTERNOON:

            return (
                hour >= 12 &&
                hour < 17
            );


        // ==================================================
        // MALAM
        // 18:00 - 00:00
        // ==================================================

        case SCHEDULE_NIGHT:

            return (
                hour >= 18 &&
                hour < 24
            );


        default:

            return false;
    }
}


// ======================================================
// PRINT CURRENT WITA TIME
// ======================================================

void printCurrentTime()
{
    struct tm timeinfo;


    if (
        !getLocalTime(
            &timeinfo
        )
    )
    {
        Serial.println(
            "Waktu belum tersedia"
        );

        return;
    }


    Serial.printf(
        "WITA: %02d:%02d:%02d\n",

        timeinfo.tm_hour,

        timeinfo.tm_min,

        timeinfo.tm_sec
    );
}


// ======================================================
// CHECK SCHEDULES
// ======================================================

void checkSchedules()
{

    // ==================================================
    // RELAY A
    // ==================================================

    if (relayAAutoSchedule)
    {

        bool shouldBeOn =
            isScheduleActive(
                relayASchedule
            );


        if (
            relayAState !=
            shouldBeOn
        )
        {

            setRelayA(
                shouldBeOn
            );


            Serial.print(
                "Relay A Schedule -> "
            );


            if (shouldBeOn)
            {
                Serial.println(
                    "ON"
                );
            }
            else
            {
                Serial.println(
                    "OFF"
                );
            }


            sendRelayStatus();
        }
    }


    // ==================================================
    // RELAY B
    // ==================================================

    if (relayBAutoSchedule)
    {

        bool shouldBeOn =
            isScheduleActive(
                relayBSchedule
            );


        if (
            relayBState !=
            shouldBeOn
        )
        {

            setRelayB(
                shouldBeOn
            );


            Serial.print(
                "Relay B Schedule -> "
            );


            if (shouldBeOn)
            {
                Serial.println(
                    "ON"
                );
            }
            else
            {
                Serial.println(
                    "OFF"
                );
            }


            sendRelayStatus();
        }
    }


    if (relayCAutoSchedule)
    {

        bool shouldBeOn =
            isScheduleActive(
                relayCSchedule
            );


        if (
            relayCState !=
            shouldBeOn
        )
        {

            setRelayC(
                shouldBeOn
            );


            Serial.print(
                "Relay C Schedule -> "
            );


            if (shouldBeOn)
            {
                Serial.println(
                    "ON"
                );
            }
            else
            {
                Serial.println(
                    "OFF"
                );
            }


            sendRelayStatus();
        }
    }

    if (relayDAutoSchedule)
    {

        bool shouldBeOn =
            isScheduleActive(
                relayDSchedule
            );


        if (
            relayDState !=
            shouldBeOn
        )
        {

            setRelayD(
                shouldBeOn
            );


            Serial.print(
                "Relay D Schedule -> "
            );


            if (shouldBeOn)
            {
                Serial.println(
                    "ON"
                );
            }
            else
            {
                Serial.println(
                    "OFF"
                );
            }


            sendRelayStatus();
        }
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
                webSocket.remoteIP(
                    num
                );


            Serial.print(
                "WebSocket Client Connected: "
            );


            Serial.println(ip);


            sendRelayStatus(
                num
            );


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


            int schedule =
                doc["schedule"] | 0;



            // ==================================================
            // RELAY A
            // ==================================================

            if (
                relay != nullptr &&
                strcmp(
                    relay,
                    "A"
                ) == 0
            )
            {

                // ------------------------------------------
                // OFF
                // ------------------------------------------

                if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "off"
                    ) == 0
                )
                {

                    /*
                     * OFF membatalkan schedule.
                     */

                    relayAAutoSchedule =
                        false;


                    relayASchedule =
                        SCHEDULE_NONE;


                    setRelayA(
                        false
                    );


                    Serial.println(
                        "Relay A -> OFF"
                    );
                }


                // ------------------------------------------
                // ON
                // ------------------------------------------

                else if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "on"
                    ) == 0
                )
                {

                    // ==================================================
                    // MODE SCHEDULE
                    // ==================================================

                    if (
                        mode != nullptr &&
                        strcmp(
                            mode,
                            "schedule"
                        ) == 0 &&
                        schedule >= 1 &&
                        schedule <= 3
                    )
                    {

                        relayAAutoSchedule =
                            true;


                        relayASchedule =
                            (ScheduleType)
                            schedule;


                        /*
                         * Jangan hanya menyalakan relay.
                         *
                         * Cek dulu apakah saat ini
                         * berada di dalam jam schedule.
                         */

                        bool shouldBeOn =
                            isScheduleActive(
                                relayASchedule
                            );


                        setRelayA(
                            shouldBeOn
                        );


                        Serial.print(
                            "Relay A Schedule: "
                        );


                        if (schedule == 1)
                        {
                            Serial.println(
                                "06:00 - 11:00"
                            );
                        }
                        else if (
                            schedule == 2
                        )
                        {
                            Serial.println(
                                "12:00 - 17:00"
                            );
                        }
                        else if (
                            schedule == 3
                        )
                        {
                            Serial.println(
                                "18:00 - 00:00"
                            );
                        }


                        Serial.print(
                            "Relay A current state: "
                        );


                        if (shouldBeOn)
                        {
                            Serial.println(
                                "ON"
                            );
                        }
                        else
                        {
                            Serial.println(
                                "OFF"
                            );
                        }
                    }


                    // ==================================================
                    // MODE MANUAL
                    // ==================================================

                    else
                    {

                        relayAAutoSchedule =
                            false;


                        relayASchedule =
                            SCHEDULE_NONE;


                        setRelayA(
                            true
                        );


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
                strcmp(
                    relay,
                    "B"
                ) == 0
            )
            {

                // ------------------------------------------
                // OFF
                // ------------------------------------------

                if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "off"
                    ) == 0
                )
                {

                    /*
                     * OFF membatalkan schedule.
                     */

                    relayBAutoSchedule =
                        false;


                    relayBSchedule =
                        SCHEDULE_NONE;


                    setRelayB(
                        false
                    );


                    Serial.println(
                        "Relay B -> OFF"
                    );
                }


                // ------------------------------------------
                // ON
                // ------------------------------------------

                else if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "on"
                    ) == 0
                )
                {

                    // ==================================================
                    // MODE SCHEDULE
                    // ==================================================

                    if (
                        mode != nullptr &&
                        strcmp(
                            mode,
                            "schedule"
                        ) == 0 &&
                        schedule >= 1 &&
                        schedule <= 3
                    )
                    {

                        relayBAutoSchedule =
                            true;


                        relayBSchedule =
                            (ScheduleType)
                            schedule;


                        /*
                         * Cek apakah sekarang
                         * berada dalam waktu schedule.
                         */

                        bool shouldBeOn =
                            isScheduleActive(
                                relayBSchedule
                            );


                        setRelayB(
                            shouldBeOn
                        );


                        Serial.print(
                            "Relay B Schedule: "
                        );


                        if (schedule == 1)
                        {
                            Serial.println(
                                "06:00 - 11:00"
                            );
                        }
                        else if (
                            schedule == 2
                        )
                        {
                            Serial.println(
                                "12:00 - 17:00"
                            );
                        }
                        else if (
                            schedule == 3
                        )
                        {
                            Serial.println(
                                "18:00 - 00:00"
                            );
                        }


                        Serial.print(
                            "Relay B current state: "
                        );


                        if (shouldBeOn)
                        {
                            Serial.println(
                                "ON"
                            );
                        }
                        else
                        {
                            Serial.println(
                                "OFF"
                            );
                        }
                    }


                    // ==================================================
                    // MODE MANUAL
                    // ==================================================

                    else
                    {

                        relayBAutoSchedule =
                            false;


                        relayBSchedule =
                            SCHEDULE_NONE;


                        setRelayB(
                            true
                        );


                        Serial.println(
                            "Relay B ON - Manual"
                        );
                    }
                }
            }
            // ==================================================
            // RELAY C
            // ==================================================
            if (
                relay != nullptr &&
                strcmp(
                    relay,
                    "C"
                ) == 0
            )
            {

                // ------------------------------------------
                // OFF
                // ------------------------------------------

                if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "off"
                    ) == 0
                )
                {

                    /*
                     * OFF membatalkan schedule.
                     */

                    relayCAutoSchedule =
                        false;


                    relayCSchedule =
                        SCHEDULE_NONE;


                    setRelayC(
                        false
                    );


                    Serial.println(
                        "Relay C -> OFF"
                    );
                }


                // ------------------------------------------
                // ON
                // ------------------------------------------

                else if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "on"
                    ) == 0
                )
                {

                    // ==================================================
                    // MODE SCHEDULE
                    // ==================================================

                    if (
                        mode != nullptr &&
                        strcmp(
                            mode,
                            "schedule"
                        ) == 0 &&
                        schedule >= 1 &&
                        schedule <= 3
                    )
                    {

                        relayCAutoSchedule =
                            true;


                        relayCSchedule =
                            (ScheduleType)
                            schedule;


                        /*
                         * Cek apakah sekarang
                         * berada dalam waktu schedule.
                         */

                        bool shouldBeOn =
                            isScheduleActive(
                                relayCSchedule
                            );


                        setRelayC(
                            shouldBeOn
                        );


                        Serial.print(
                            "Relay C Schedule: "
                        );


                        if (schedule == 1)
                        {
                            Serial.println(
                                "06:00 - 11:00"
                            );
                        }
                        else if (
                            schedule == 2
                        )
                        {
                            Serial.println(
                                "12:00 - 17:00"
                            );
                        }
                        else if (
                            schedule == 3
                        )
                        {
                            Serial.println(
                                "18:00 - 00:00"
                            );
                        }


                        Serial.print(
                            "Relay C current state: "
                        );


                        if (shouldBeOn)
                        {
                            Serial.println(
                                "ON"
                            );
                        }
                        else
                        {
                            Serial.println(
                                "OFF"
                            );
                        }
                    }


                    // ==================================================
                    // MODE MANUAL
                    // ==================================================

                    else
                    {

                        relayCAutoSchedule =
                            false;


                        relayCSchedule =
                            SCHEDULE_NONE;


                        setRelayC(
                            true
                        );


                        Serial.println(
                            "Relay C ON - Manual"
                        );
                    }
                }
            }
            // ==================================================
            // RELAY D
            // ==================================================
            if (
                relay != nullptr &&
                strcmp(
                    relay,
                    "D"
                ) == 0
            )
            {

                // ------------------------------------------
                // OFF
                // ------------------------------------------

                if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "off"
                    ) == 0
                )
                {

                    /*
                     * OFF membatalkan schedule.
                     */

                    relayDAutoSchedule =
                        false;


                    relayDSchedule =
                        SCHEDULE_NONE;


                    setRelayD(
                        false
                    );


                    Serial.println(
                        "Relay D -> OFF"
                    );
                }


                // ------------------------------------------
                // ON
                // ------------------------------------------

                else if (
                    action != nullptr &&
                    strcmp(
                        action,
                        "on"
                    ) == 0
                )
                {

                    // ==================================================
                    // MODE SCHEDULE
                    // ==================================================

                    if (
                        mode != nullptr &&
                        strcmp(
                            mode,
                            "schedule"
                        ) == 0 &&
                        schedule >= 1 &&
                        schedule <= 3
                    )
                    {

                        relayDAutoSchedule =
                            true;


                        relayDSchedule =
                            (ScheduleType)
                            schedule;


                        /*
                         * Cek apakah sekarang
                         * berada dalam waktu schedule.
                         */

                        bool shouldBeOn =
                            isScheduleActive(
                                relayDSchedule
                            );


                        setRelayD(
                            shouldBeOn
                        );


                        Serial.print(
                            "Relay D Schedule: "
                        );


                        if (schedule == 1)
                        {
                            Serial.println(
                                "06:00 - 11:00"
                            );
                        }
                        else if (
                            schedule == 2
                        )
                        {
                            Serial.println(
                                "12:00 - 17:00"
                            );
                        }
                        else if (
                            schedule == 3
                        )
                        {
                            Serial.println(
                                "18:00 - 00:00"
                            );
                        }


                        Serial.print(
                            "Relay D current state: "
                        );


                        if (shouldBeOn)
                        {
                            Serial.println(
                                "ON"
                            );
                        }
                        else
                        {
                            Serial.println(
                                "OFF"
                            );
                        }
                    }


                    // ==================================================
                    // MODE MANUAL
                    // ==================================================

                    else
                    {

                        relayDAutoSchedule =
                            false;


                        relayDSchedule =
                            SCHEDULE_NONE;


                        setRelayD(
                            true
                        );


                        Serial.println(
                            "Relay D ON - Manual"
                        );
                    }
                }
            }

            // ==================================================
            // SEND UPDATED STATUS
            // ==================================================

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
// SETUP
// ======================================================

void setup()
{

    Serial.begin(
        115200
    );


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
    pinMode(
        RELAY_C_PIN,
        OUTPUT
    );
    pinMode(
        RELAY_D_PIN,
        OUTPUT
    );


    /*
     * Pastikan relay OFF saat startup.
     */

    digitalWrite(
        RELAY_A_PIN,
        LOW
    );


    digitalWrite(
        RELAY_B_PIN,
        LOW
    );
    digitalWrite(
        RELAY_C_PIN,
        LOW
    );
    digitalWrite(
        RELAY_D_PIN,
        LOW
    );


    relayAState = false;

    relayBState = false;

    relayCState = false;

    relayDState = false;



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
        WiFi.status() !=
        WL_CONNECTED
    )
    {

        delay(500);

        Serial.print(
            "."
        );
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
    // NTP TIME
    // ==================================================

    Serial.println();

    Serial.println(
        "Synchronizing WITA time..."
    );


    configTime(
        GMT_OFFSET_SEC,
        DAYLIGHT_OFFSET_SEC,
        NTP_SERVER_1,
        NTP_SERVER_2
    );


    struct tm timeinfo;


    /*
     * Tunggu sampai waktu tersedia.
     */

    int retry = 0;


    while (
        !getLocalTime(
            &timeinfo
        ) &&
        retry < 20
    )
    {

        Serial.println(
            "Waiting for NTP..."
        );


        delay(500);


        retry++;
    }


    if (
        getLocalTime(
            &timeinfo
        )
    )
    {

        Serial.println(
            "Time synchronized!"
        );


        Serial.printf(

            "WITA Time: "
            "%02d:%02d:%02d\n",

            timeinfo.tm_hour,

            timeinfo.tm_min,

            timeinfo.tm_sec
        );
    }
    else
    {

        Serial.println(
            "Failed to synchronize time"
        );
    }



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
        " UDAWA SMART SYSTEM READY"
    );


    Serial.println(
        "================================"
    );


    Serial.println(
        "Timezone : WITA (UTC+8)"
    );


    Serial.println(
        "Schedule :"
    );


    Serial.println(
        "06:00 - 11:00"
    );


    Serial.println(
        "12:00 - 17:00"
    );


    Serial.println(
        "18:00 - 00:00"
    );


    Serial.println();


    Serial.println(
        "run on http://localhost:8180"
    );
}


// ======================================================
// LOOP
// ======================================================

void loop()
{

    server.handleClient();


    webSocket.loop();


    /*
     * Periksa schedule secara berkala.
     */

    checkSchedules();


    /*
     * Tidak perlu delay terlalu kecil karena
     * schedule hanya membutuhkan ketelitian
     * sampai hitungan detik. niga
     */

    delay(500);
}