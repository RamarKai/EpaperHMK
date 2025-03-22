#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "../config/PinConfig.h"

class OLED_Display {
public:
    OLED_Display();
    bool begin();
    void updateDisplay(float lightLevel, int gasLevel, bool ledState, uint8_t r, uint8_t g, uint8_t b);
    void clear();

private:
    Adafruit_SSD1306 display;
    TwoWire& i2c;
    bool initialized;
};

#endif // OLED_DISPLAY_H 