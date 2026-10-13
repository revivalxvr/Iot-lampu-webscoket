#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <time.h>

#include "pages/melon.h"
#include "pages/tomat.h"
#include "pages/selada.h"
#include "pages/cabai.h"


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

void setRelayA(bool state)
{
    relayAState = state;

    digitalWrite(
        RELAY_A_PIN,
        state ? HIGH : LOW
    );
}

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

void sendRelayStatus(
    uint8_t client = 255
)
{
    JsonDocument doc;

    doc["relayA"] = relayAState;
    doc["relayB"] = relayBState;
    doc["relayC"] = relayCState;
    doc["relayD"] = relayDState;

    doc["remainingA"] = 0;
    doc["remainingB"] = 0;
    doc["remainingC"] = 0;
    doc["remainingD"] = 0;
 
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

void handleMelon()
{
    server.send_P(200, "text/html", MELON_HTML);
}

void handleTomat()
{
    server.send_P(200, "text/html", TOMAT_HTML);
}


void handleSelada()
{
    server.send_P(200, "text/html", SELADA_HTML);
}

void handleCabai()
{
    server.send_P(200, "text/html", CABAI_HTML);
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

    // Halaman utama diarahkan ke halaman Melon.
    server.on("/", HTTP_GET, handleMelon);
    server.on("/melon", HTTP_GET, handleMelon);
    server.on("/tomat", HTTP_GET, handleTomat);
    server.on("/selada", HTTP_GET, handleSelada);
    server.on("/cabai", HTTP_GET, handleCabai);
    // Alias lama dipertahankan agar URL sebelumnya tetap berfungsi.
    server.on("/lainnya", HTTP_GET, handleSelada);


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