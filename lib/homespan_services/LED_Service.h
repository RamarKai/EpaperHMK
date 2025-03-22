#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include "HomeSpan.h"
#include "../led/WS2812B_LED.h"

struct DEV_WS2812B : Service::LightBulb {
    SpanCharacteristic *power;
    SpanCharacteristic *brightness;
    SpanCharacteristic *hue;
    SpanCharacteristic *saturation;
    WS2812B_LED& led;
    
    DEV_WS2812B(WS2812B_LED& ledRef);
    boolean update() override;
};

#endif // LED_SERVICE_H 