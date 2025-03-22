#ifndef LIGHT_SENSOR_SERVICE_H
#define LIGHT_SENSOR_SERVICE_H

#include "HomeSpan.h"
#include "../sensors/BH1750_Sensor.h"

struct DEV_LightSensor : Service::LightSensor {
    SpanCharacteristic *lightLevel;
    BH1750_Sensor& sensor;
    unsigned long lastReadTime;
    
    DEV_LightSensor(BH1750_Sensor& sensorRef);
    void loop() override;
};

#endif // LIGHT_SENSOR_SERVICE_H 