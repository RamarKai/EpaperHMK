#include "mq2sensor.h"

int gasLevel = 0;

int readMQ2Sensor() {
    int reading = analogRead(MQ2_PIN);
    return reading;
}

int convertToAirQuality(int reading) {
    int quality;
    if(reading < 1000) quality = 1;
    else if(reading < 2000) quality = 2;
    else if(reading < 3000) quality = 3;
    else if(reading < 4000) quality = 4;
    else quality = 5;
    
    return quality;
}

void initMQ2Sensor() {
    pinMode(MQ2_PIN, INPUT);
    Serial.println("MQ2 sensor initialized");
}

DEV_AirQualitySensor::DEV_AirQualitySensor() : Service::AirQualitySensor() {
    airQuality = new Characteristic::AirQuality(0);
}

void DEV_AirQualitySensor::loop() {
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= 500) {
        int reading = readMQ2Sensor();
        gasLevel = reading;
        
        int quality = convertToAirQuality(reading);
        
        if(quality != airQuality->getVal()) {
            airQuality->setVal(quality);
        }
        
        lastReadTime = currentTime;
    }
}
