#include <Arduino.h>
#include <IIOT_V4_Pinout.h>
#include <SPI.h>
#include <Wire.h>

#define SerialSim Serial1
#define TEST_SIM 0
#define TINY_GSM_MODEM_SIM7600
#include <TinyGSM.h>
#include <StreamDebugger.h>
#include <ArduinoHttpClient.h>
#include <TinyGsmClient.h>
#define SerialMon Serial
#define SerialAT Serial1
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
TinyGsmClient client(modem);
const char server[] = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";
const int port = 80;
HttpClient http(client, server, port);
// #define TINY_GSM_RX_BUFFER 650
// Your GPRS credentials, if any
const char apn[] = "airtelgprs.com";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details
// set GSM PIN, if any
#define GSM_PIN ""

void setup() {
    // BUTTON
    Serial.begin(115200);
    pinMode(PIN_BUTTON_ON_BOARD, INPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    pinMode(PIN_GSM_PWR_KEY, OUTPUT);
    digitalWrite(PIN_GSM_PWR_KEY, LOW);
    delay(1000);
    digitalWrite(PIN_GSM_PWR_KEY, HIGH);
    delay(10);
    SerialSim.begin(
        115200,
        SERIAL_8N1,
        PIN_SIM_TX,
        PIN_SIM_RX,
        false);
#if defined(TEST_SIM) && TEST_SIM == 1
    for (int _x = 0; _x < 10; _x++) {
        SerialSim.write("AT\r\n");
        if (SerialSim.available()) {
            String x = SerialSim.readStringUntil('\r');
            // Serial.println(x);
            log_w("%s", x);
            if (x.indexOf("OK") > 0) {
                log_w("SIM module Found");
                break;
            }
            delay(200);
        } else {
            delay(2000);
        }
        log_w("sim...");
    }
#else

    // Your GPRS credentials, if any
    const char apn[] = "airtelgprs.com";
    const char gprsUser[] = "";
    const char gprsPass[] = "";
    log_v("Initializing modem...");
    modem.restart();
    String modemInfo = modem.getModemInfo();
    log_w("Info");
    SerialMon.println(modemInfo);
    log_w("Modem Info: %s", String(modemInfo));
    if (GSM_PIN && modem.getSimStatus() != 3) {
        modem.simUnlock(GSM_PIN);
    }
    String imei = modem.getIMEI();
    // modem.sendSMS("+9187", String("Hello from ") + imei);
#endif  // TEST_SIM
}

void loop() {
#if defined(TEST_SIM) && TEST_SIM == 1
    if (SerialSim.available())
        Serial.write(SerialSim.read());
    if (Serial.available())
        SerialSim.write(Serial.read());
#else

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected()) {
        SerialMon.println("Network connected");
    }

    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected()) {
        SerialMon.println("GPRS connected");
    }

    SerialMon.print(F("Performing HTTP GET request... "));
    int err = http.get(resource);
    if (err != 0) {
        SerialMon.println(F("failed to connect"));
        delay(10000);
        return;
    }

    int status = http.responseStatusCode();
    SerialMon.print(F("Response status code: "));
    SerialMon.println(status);
    if (!status) {
        delay(10000);
        return;
    }

    SerialMon.println(F("Response Headers:"));
    while (http.headerAvailable()) {
        String headerName = http.readHeaderName();
        String headerValue = http.readHeaderValue();
        SerialMon.println("    " + headerName + " : " + headerValue);
    }

    int length = http.contentLength();
    if (length >= 0) {
        SerialMon.print(F("Content length is: "));
        SerialMon.println(length);
    }
    if (http.isResponseChunked()) {
        SerialMon.println(F("The response is chunked"));
    }

    String body = http.responseBody();
    SerialMon.println(F("Response:"));
    SerialMon.println(body);

    SerialMon.print(F("Body length is: "));
    SerialMon.println(body.length());

    // Shutdown

    http.stop();
    SerialMon.println(F("Server disconnected"));
    modem.gprsDisconnect();
    SerialMon.println(F("GPRS disconnected"));
#endif //TEST_SIM

}