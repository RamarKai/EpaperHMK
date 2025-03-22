#include "MQ2_Sensor.h"
#include <Arduino.h>

MQ2_Sensor::MQ2_Sensor() : initialized(false) {
}

void MQ2_Sensor::begin() {
    pinMode(MQ2_PIN, INPUT);
    initialized = true;
}

int MQ2_Sensor::readGasLevel() {
    if (!initialized) return -1;
    return analogRead(MQ2_PIN);
}

int MQ2_Sensor::getAirQuality() const {
    if (!initialized) return -1;
    
    int reading = analogRead(MQ2_PIN);
    
    // 根据气体浓度确定空气质量等级
    if(reading < 1500) return 1;      // EXCELLENT
    else if(reading < 2000) return 2;  // GOOD
    else if(reading < 3000) return 3;  // FAIR
    else if(reading < 4000) return 4;  // INFERIOR
    else return 5;                     // POOR
} 