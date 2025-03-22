#include "LightSensor_Service.h"

DEV_LightSensor::DEV_LightSensor(BH1750_Sensor& sensorRef) : Service::LightSensor(), sensor(sensorRef) {
    lightLevel = new Characteristic::CurrentAmbientLightLevel(0.0);
    lightLevel->setRange(0, 100000);
    lastReadTime = 0;
}

void DEV_LightSensor::loop() {
    if(!sensor.isReady()) return;
    
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= 1000) {
        float lux = sensor.readLightLevel();
        if(lux >= 0 && lux != lightLevel->getVal()) {
            lightLevel->setVal(lux);
        }
        lastReadTime = currentTime;
    }
} 