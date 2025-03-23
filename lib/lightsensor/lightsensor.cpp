#include "lightsensor.h"

TwoWire I2C_BH1750 = TwoWire(1);

float lightLevel = 0;
bool bh1750_ok = false;

void initBH1750() {
    I2C_BH1750.begin(BH1750_SDA, BH1750_SCL);
    
    I2C_BH1750.beginTransmission(0x23);
    I2C_BH1750.write(0x01);
    if(I2C_BH1750.endTransmission() == 0) {
        delay(10);
        I2C_BH1750.beginTransmission(0x23);
        I2C_BH1750.write(0x10);
        if(I2C_BH1750.endTransmission() == 0) {
            Serial.println("BH1750 initialized successfully");
            bh1750_ok = true;
        } else {
            Serial.println("Error configuring BH1750");
            bh1750_ok = false;
        }
    } else {
        Serial.println("Error initializing BH1750");
        bh1750_ok = false;
    }
}

float readBH1750() {
    uint16_t value = 0;
    
    I2C_BH1750.beginTransmission(0x23);
    I2C_BH1750.write(0x10);
    if(I2C_BH1750.endTransmission() != 0) {
        return -1;
    }
    
    delay(120);
    
    if(I2C_BH1750.requestFrom(0x23, 2) == 2) {
        value = I2C_BH1750.read();
        value <<= 8;
        value |= I2C_BH1750.read();
        float lux = value / 1.2;
        return lux;
    }
    return -1;
}

DEV_LightSensor::DEV_LightSensor() : Service::LightSensor() {
    lightLevel = new Characteristic::CurrentAmbientLightLevel(0.0);
    lightLevel->setRange(0, 100000);
}

void DEV_LightSensor::loop() {
    if(!bh1750_ok) return;
    
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= 1000) {
        float lux = readBH1750();
        if(lux >= 0) {
            if(lux != lightLevel->getVal()) {
                lightLevel->setVal(lux);
                ::lightLevel = lux;
            }
        }
        lastReadTime = currentTime;
    }
}
