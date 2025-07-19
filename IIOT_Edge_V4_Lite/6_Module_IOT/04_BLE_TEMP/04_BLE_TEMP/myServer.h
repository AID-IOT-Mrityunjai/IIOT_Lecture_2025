#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <Ws2812b_led.h>

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

void MyServerTask(void* pvParameters) {
    led.init();

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
    long _millis_last = millis();
    TaskHandle_t myTaskHandle = xTaskGetCurrentTaskHandle();

    while (1) {
        server.handleClient();
        if (millis() - _millis_last > 2000) {
            // query the remaining free memory in the reserved area,
            //  The smaller the returned number the closer the task has come to overflowing its stack.
            log_v("TASK: %s : %d ", pcTaskGetName(myTaskHandle), uxTaskGetStackHighWaterMark(myTaskHandle));
            _millis_last = millis();
        }
    }
}