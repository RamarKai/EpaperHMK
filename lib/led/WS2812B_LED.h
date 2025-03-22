#ifndef WS2812B_LED_H
#define WS2812B_LED_H

#include <FastLED.h>
#include "../config/PinConfig.h"

class WS2812B_LED {
public:
    WS2812B_LED();
    void begin();
    void setColor(CRGB color);
    void setHSV(uint8_t hue, uint8_t saturation, uint8_t value);
    void setBrightness(uint8_t brightness);
    void turnOff();
    bool isOn() const { return state; }
    CRGB getCurrentColor() const { return currentColor; }

private:
    CRGB leds[NUM_LEDS];
    CRGB currentColor;
    bool state;
    bool initialized;
};

#endif // WS2812B_LED_H 