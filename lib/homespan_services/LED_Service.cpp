#include "LED_Service.h"

DEV_WS2812B::DEV_WS2812B(WS2812B_LED& ledRef) : Service::LightBulb(), led(ledRef) {
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
        
        led.setHSV(h * 255/360, s * 255/100, v * 255/100);
    } else {
        led.turnOff();
    }
    
    return true;
} 