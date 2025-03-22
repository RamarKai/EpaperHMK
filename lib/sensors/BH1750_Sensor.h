#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include <Wire.h>
#include "../config/PinConfig.h"

class BH1750_Sensor {
public:
    BH1750_Sensor();
    bool begin();
    float readLightLevel();
    bool isReady() const { return initialized; }

private:
    TwoWire& i2c;
    bool initialized;
    unsigned long lastReadTime;
};

#endif // BH1750_SENSOR_H 