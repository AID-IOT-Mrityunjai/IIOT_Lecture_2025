
// /**
//  * @file 01_DigitalWrite_Example.ino
//  * @author Mrityunjai Kumar (mrityunjai@sincgrid.com)
//  * @brief
//  * @version 0.1
//  * @date 2022-09-20
//  *
//  * Copyright Sincgrid LLP
//  *
//  * Problem statement:- Turn on the led from api in same network.
//  * <IP>/on   -->> To turn on the led
//  * <IP>/off   -->> To turn off the led
//  *
//  */
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESPmDNS.h>
#include <IIOT_V4_Pinout.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <myServer.h>


#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <SPI.h>
#include <WiFiClientSecure.h>

#include "WiFi.h"
#include "secrets.h"
#include "time.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

unsigned long getTime() {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        // Serial.println("Failed to obtain time");
        return (0);
    }
    time(&now);
    return now;
}

class AWS {
   private:
   public:
    void init();
    void thread();
    void connectAWS();
    void publishMessage();
};

String _topic_publish="iitj/topics/test2025/upload";
String _topic_sub = "iitj/topics/test2025";
String _unique_id;
String _base_topic;
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void AWS::init() {
    connectAWS();
}

long _last_hit_millis_aws = 0;
void AWS::thread() {
    client.loop();
}

void AWS::connectAWS() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Init time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    // Configure WiFiClientSecure to use the AWS IoT device credentials
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Create a message handler
    client.onMessage(messageHandler);

    Serial.print("Connecting to AWS IOT");

    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        delay(100);
    }
    // client.setWill()
    // LWT 
    if (!client.connected()) {
        log_e("AWS IoT Timeout!");
        return;
    }

    // Subscribe to a topic
    client.subscribe(_topic_sub);

    Serial.println("AWS IoT Connected!");
    publishMessage();
}

void AWS::publishMessage() {
    StaticJsonDocument<200> doc;
    doc["time"] = millis();
    doc["sensor_a0"] = "dummy";
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);  // print to client

    client.publish(_topic_publish, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
    log_v("incoming: ");
    Serial.println(topic + " - " + payload);
    // debugln("");
    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload);
    //  _millis_buzzer_trigger=millis();
}


AWS aws;
void setup() {
    Serial.begin(115200);
    aws.init();
}

void loop() {
    aws.thread();
}