#include "ws2812.h"

CRGB leds[NUM_LEDS];

bool ledState = false;
CRGB currentColor = CRGB::Black;

void initWS2812B() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255);
    FastLED.show();
    Serial.println("WS2812B initialized");
}

void setLEDColor(CRGB color, bool update) {
    currentColor = color;
    leds[0] = color;
    if (update) {
        FastLED.show();
    }
    ledState = (color != CRGB::Black);
}

void setLEDState(bool on, bool update) {
    if (on) {
        if (currentColor == CRGB::Black) {
            currentColor = CRGB::White;
        }
        leds[0] = currentColor;
    } else {
        leds[0] = CRGB::Black;
    }
    
    if (update) {
        FastLED.show();
    }
    
    ledState = on;
}

DEV_WS2812B::DEV_WS2812B() : Service::LightBulb() {
    power = new Characteristic::On();
    
    brightness = new Characteristic::Brightness(100);
    brightness->setRange(0, 100);
    
    hue = new Characteristic::Hue(0);
    
    saturation = new Characteristic::Saturation(0);
}

boolean DEV_WS2812B::update() {
    boolean isOn = power->getNewVal();
    
    if(isOn) {
        float h = hue->getNewVal();
        float s = saturation->getNewVal();
        float v = brightness->getNewVal();
        
        CHSV hsv(h * 255/360, s * 255/100, v * 255/100);
        hsv2rgb_rainbow(hsv, currentColor);
    } else {
        currentColor = CRGB::Black;
    }
    
    leds[0] = currentColor;
    FastLED.show();
    
    ledState = isOn;
    
    return(true);
}