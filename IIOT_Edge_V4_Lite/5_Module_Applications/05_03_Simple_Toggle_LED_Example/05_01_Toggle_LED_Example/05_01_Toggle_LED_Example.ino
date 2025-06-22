
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
#include <FastLED.h>
// #include <Ws2812b_led.h>
#include <IIOT_V4_Pinout.h>

#define NUM_LEDS 1
#define STATUS_LED (leds[0])
CRGB leds[NUM_LEDS];

void setup() {
    FastLED.addLeds<WS2812B, PIN_LED_WS2812_DATA, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
    FastLED.setBrightness(10);
}

void loop() {

    STATUS_LED= CRGB::Blue;
    FastLED.show();
    delay(2000);
    STATUS_LED= CRGB::Blue;
    FastLED.show();
    delay(2000);
    STATUS_LED = CRGB::Green;
    FastLED.show();
    delay(2000);
}