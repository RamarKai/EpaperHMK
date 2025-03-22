#ifndef AIR_QUALITY_SERVICE_H
#define AIR_QUALITY_SERVICE_H

#include "HomeSpan.h"
#include "../sensors/MQ2_Sensor.h"

struct DEV_AirQualitySensor : Service::AirQualitySensor {
    SpanCharacteristic *airQuality;
    MQ2_Sensor& sensor;
    
    DEV_AirQualitySensor(MQ2_Sensor& sensorRef);
    void loop() override;
};

#endif // AIR_QUALITY_SERVICE_H 