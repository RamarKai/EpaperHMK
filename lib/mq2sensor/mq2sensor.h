#ifndef MQ2SENSOR_H
#define MQ2SENSOR_H

#include "HomeSpan.h"
#include <Arduino.h>

#define MQ2_PIN 34

extern int gasLevel;

int readMQ2Sensor();

int convertToAirQuality(int reading);

void initMQ2Sensor();

struct DEV_AirQualitySensor : Service::AirQualitySensor {
    SpanCharacteristic *airQuality;
    unsigned long lastReadTime = 0;

    DEV_AirQualitySensor();

    void loop();
};

#endif