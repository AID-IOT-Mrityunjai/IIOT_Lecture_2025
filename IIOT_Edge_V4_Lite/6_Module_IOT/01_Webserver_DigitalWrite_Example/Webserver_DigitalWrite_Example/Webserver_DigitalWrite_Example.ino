
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
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Ws2812b_led.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

WebServer server(80);
ws2812b_led led;
void handleRoot() {
    server.send(200, "text/plain", "hello from esp32!");
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}
static CRGB xledColor;

void setup(void) {
    // pinMode(led, OUTPUT);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");
    led.init();

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

    server.on("/", handleRoot);

    server.on("/inline", []() {
        server.send(200, "text/plain", "this works as well");
    });

    server.on("/on", []() {
        server.send(200, "text/plain", "led green");
        xledColor = CRGB::Green;
        xQueueSend(queueHandleIdWsLed, &xledColor, 0);
    });

    server.on("/off", []() {
        server.send(200, "text/plain", "led red");
        xledColor = CRGB::Red;
        xQueueSend(queueHandleIdWsLed, &xledColor, 0);
    });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

long _millis = 0;
void loop(void) {
    server.handleClient();
    delay(2);  // allow the cpu to switch to other tasks

    if (millis() - _millis > 4000) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        }
        _millis=millis();
    }
}
