#include "AirQuality_Service.h"

DEV_AirQualitySensor::DEV_AirQualitySensor(MQ2_Sensor& sensorRef) : Service::AirQualitySensor(), sensor(sensorRef) {
    airQuality = new Characteristic::AirQuality(0);
}

void DEV_AirQualitySensor::loop() {
    int quality = sensor.getAirQuality();
    if(quality >= 0 && quality != airQuality->getVal()) {
        airQuality->setVal(quality);
    }
} 