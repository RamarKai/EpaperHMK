#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H

#include "../config/PinConfig.h"

class MQ2_Sensor {
public:
    MQ2_Sensor();
    void begin();
    int readGasLevel();
    int getAirQuality() const;

private:
    bool initialized;
};

#endif // MQ2_SENSOR_H 