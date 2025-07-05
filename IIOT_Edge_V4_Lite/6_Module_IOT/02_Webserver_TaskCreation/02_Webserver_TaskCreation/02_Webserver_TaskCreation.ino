
/**
 * @file 01_DigitalWrite_Example.ino
 * @author Mrityunjai Kumar (mrityunjai@sincgrid.com)
 * @brief
 * @version 0.1
 * @date 2022-09-20
 *
 * Copyright Sincgrid LLP
 *
 * Problem statement:- Turn on the led from api in same network.
 * <IP>/on   -->> To turn on the led
 * <IP>/off   -->> To turn off the led
 *
 */
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
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

void setup(void) {
    // pinMode(led, OUTPUT);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    xTaskCreate(
        MyServerTask,
        "MyServerTask",  // A name just for humans
        1024 * 8,     // The stack size
        NULL,         // Task parameter
        1,            // Priority
        NULL          // Task handle is not used here - simply pass NULL
    );

    Serial.println("HTTP server started");
}

long _millis = 0;
void loop(void) {
    delay(2);  // allow the cpu to switch to other tasks

    if (millis() - _millis > 4000) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        }
        _millis = millis();
    }
}
