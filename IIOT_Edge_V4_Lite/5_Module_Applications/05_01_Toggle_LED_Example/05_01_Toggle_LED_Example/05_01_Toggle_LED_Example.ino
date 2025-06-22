
/**
 * @file 01_DigitalWrite_Example.ino
 * @author Mrityunjai Kumar (mrityunjai@sincgrid.com)
 * @brief
 * @version 0.1
 * @date 2022-09-20
 *
 * Copyright Sincgrid LLP
 *
 * Problem statement:- Toggle led
 *
 */

#include <Arduino.h>
#include <Ws2812b_led.h>
ws2812b_led led;
void setup() {
    led.init();
}

// long _stage_millis=0;
static CRGB xledColor;

void loop() {
    // put your main code here, to run repeatedly:
    // if(millis()-_stage_millis>2000){

    // }
    delay(2000);
    xledColor = CRGB::Blue;
    xQueueSend(queueHandleIdWsLed, &xledColor, 0);
    delay(2000);
    xledColor = CRGB::Purple;
    xQueueSend(queueHandleIdWsLed, &xledColor, 0);
    delay(2000);
    xledColor = CRGB::Green;
    xQueueSend(queueHandleIdWsLed, &xledColor, 0);

    delay(2000);
}