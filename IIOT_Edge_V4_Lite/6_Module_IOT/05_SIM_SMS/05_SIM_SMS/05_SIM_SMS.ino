

#include <IIOT_V4_Pinout.h>
#include <SPI.h>
#include <Wire.h>

#define SerialSim Serial1
#define TINY_GSM_MODEM_A7672X
#define TEST_SIM 0
#include <TinyGSM.h>
#define TINY_GSM_MODEM_SIM7600
#include <StreamDebugger.h>
#define SerialMon Serial
#define SerialAT Serial1
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
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

// set GSM PIN, if any
#define GSM_PIN ""

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
    String imei = modem.getIMEI();
    modem.sendSMS("+9187", String("Hello from ") + imei);

#endif  // TEST_SIM
}

void loop() {
#if defined(TEST_SIM) && TEST_SIM == 1
    if (SerialSim.available())
        Serial.write(SerialSim.read());
    if (Serial.available())
        SerialSim.write(Serial.read());
#else
#endif
}