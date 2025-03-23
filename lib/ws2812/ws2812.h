#ifndef WS2812_H
#define WS2812_H

#include "HomeSpan.h"
#include <FastLED.h>

#define LED_PIN 2
#define NUM_LEDS 1

extern CRGB leds[NUM_LEDS];

extern bool ledState;
extern CRGB currentColor;

void initWS2812B();

void setLEDColor(CRGB color, bool update = true);

void setLEDState(bool on, bool update = true);

struct DEV_WS2812B : Service::LightBulb {
    SpanCharacteristic *power;
    SpanCharacteristic *brightness;
    SpanCharacteristic *hue;
    SpanCharacteristic *saturation;
    
    DEV_WS2812B();

    boolean update();
};

#endif