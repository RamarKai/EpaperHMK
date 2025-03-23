#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "HomeSpan.h"
#include <Wire.h>

#define BH1750_SDA 4
#define BH1750_SCL 5

extern float lightLevel;
extern bool bh1750_ok;
extern TwoWire I2C_BH1750;

void initBH1750();

float readBH1750();

struct DEV_LightSensor : Service::LightSensor {
    SpanCharacteristic *lightLevel;
    unsigned long lastReadTime = 0;

    DEV_LightSensor();

    void loop();
};

#endif