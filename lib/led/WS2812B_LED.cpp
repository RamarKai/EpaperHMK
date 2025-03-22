#include "WS2812B_LED.h"

WS2812B_LED::WS2812B_LED() : 
    currentColor(CRGB::Black),
    state(false),
    initialized(false) {
}

void WS2812B_LED::begin() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255);
    turnOff();
    initialized = true;
}

void WS2812B_LED::setColor(CRGB color) {
    if (!initialized) return;
    
    currentColor = color;
    leds[0] = currentColor;
    FastLED.show();
    state = (color != CRGB::Black);
}

void WS2812B_LED::setHSV(uint8_t hue, uint8_t saturation, uint8_t value) {
    if (!initialized) return;
    
    CHSV hsv(hue, saturation, value);
    hsv2rgb_rainbow(hsv, currentColor);
    leds[0] = currentColor;
    FastLED.show();
    state = (value > 0);
}

void WS2812B_LED::setBrightness(uint8_t brightness) {
    if (!initialized) return;
    FastLED.setBrightness(brightness);
    FastLED.show();
}

void WS2812B_LED::turnOff() {
    if (!initialized) return;
    
    currentColor = CRGB::Black;
    leds[0] = currentColor;
    FastLED.show();
    state = false;
} 