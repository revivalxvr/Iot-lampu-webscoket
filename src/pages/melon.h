#pragma once

// HTML halaman Melon. Disimpan di flash (PROGMEM), kompatibel dengan Wokwi.
const char MELON_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="id">

<head>

    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1.0"
    >

    <title>UDAWA Smart System - Melon</title>


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



        /* NAVIGASI HALAMAN TANAMAN */
        .nav-menu {
            max-width: 320px;
            margin: 18px auto 0;
            text-align: left;
        }

        .nav-menu label {
            display: block;
            margin-bottom: 7px;
            font-size: 14px;
            font-weight: 700;
            color: #555;
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

        <div class="nav-menu">
            <label for="plantNavigation">Pilih Tanaman</label>
            <select id="plantNavigation" onchange="if (this.value) window.location.href=this.value;">
                <option value="/melon" selected>🍈 Melon</option>
                <option value="/tomat" >🍅 Tomat</option>
                <option value="/selada" >🥬 Selada</option>
                <option value="/cabai" >🌶️ Cabai</option>

            </select>
        </div>

    </header>



    <!-- ==================================================
         TANAMAN
         ================================================== -->

    <div class="section">

        <h2 class="section-title">
            🌿 Kontrol Tanaman
        </h2>


        <div class="relay-grid">

            <article class="relay-card">


                <div class="relay-header">

                    <div class="plant-name">

                        <span class="plant-icon">
                         🍈
                        </span>

                        Melon

                    </div>

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
                            Jadwal Otomatis
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
                        id="btnOnA"
                        class="btn-on"
                        onclick="turnOn('A')"
                    >
                        💡 ON
                    </button>


                    <button
                        id="btnOffA"
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

        const toggle =
            document.getElementById(
                "relay" +
                relay +
                "Toggle"
            );

        // Halaman hanya memuat kartu relay yang dipilih.
        if (!toggle) {
            return;
        }

        if (state) {
            toggle.value = "on";
        }
        else {
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

        if (!lampDot || !lampText) return;


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



    // Pasang listener hanya untuk kontrol yang tersedia pada halaman ini.
    ["A", "B", "C", "D"].forEach(function(relay) {
        const toggle = document.getElementById("relay" + relay + "Toggle");
        if (!toggle) return;

        toggle.addEventListener("change", function() {
            if (this.value === "on") {
                turnOn(relay);
            } else {
                turnOff(relay);
            }
        });
    });


    // ======================================================
    // HANDLE VISIBILITY & TEXT TOMBOL SESUAI MODE CAHAYA
    // ======================================================

    function handleModeChange(relay) {
        const modeSelect = document.getElementById("relay" + relay + "Mode");
        const btnOff = document.getElementById("btnOff" + relay);
        const btnOn = document.getElementById("btnOn" + relay);

        if (!modeSelect || !btnOff || !btnOn) return;

        if (modeSelect.value === "schedule") {
            btnOff.style.display = "none";
            btnOn.innerHTML = "💾 Save";
        } else {
            btnOff.style.display = "";
            btnOn.innerHTML = "💡 ON";
        }
    }

    ["A", "B", "C", "D"].forEach(function(relay) {
        const modeSelect = document.getElementById("relay" + relay + "Mode");
        if (!modeSelect) return;

        // Jalankan pengecekan kondisi awal saat halaman dimuat
        handleModeChange(relay);

        // Tambahkan event listener saat pilihan Mode Cahaya berubah
        modeSelect.addEventListener("change", function() {
            handleModeChange(relay);
        });
    });


    // ======================================================
    // START
    // ======================================================

    connectWebSocket();

</script>


</body>

</html>

)rawliteral";