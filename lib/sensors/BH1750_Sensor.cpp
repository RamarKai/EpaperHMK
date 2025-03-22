#include "BH1750_Sensor.h"

BH1750_Sensor::BH1750_Sensor() : 
    i2c(Wire),
    initialized(false),
    lastReadTime(0) {
}

bool BH1750_Sensor::begin() {
    i2c.begin(BH1750_SDA, BH1750_SCL);
    
    // 初始化BH1750
    i2c.beginTransmission(BH1750_ADDR);
    i2c.write(0x01);  // 上电
    if(i2c.endTransmission() != 0) {
        return false;
    }
    
    delay(10);
    i2c.beginTransmission(BH1750_ADDR);
    i2c.write(0x10);  // 设置连续高分辨率模式
    if(i2c.endTransmission() != 0) {
        return false;
    }
    
    initialized = true;
    return true;
}

float BH1750_Sensor::readLightLevel() {
    if(!initialized) return -1;
    
    uint16_t value = 0;
    i2c.beginTransmission(BH1750_ADDR);
    i2c.write(0x10);  // 连续高分辨率模式
    if(i2c.endTransmission() != 0) {
        return -1;
    }
    
    delay(120);  // 等待测量完成
    
    if(i2c.requestFrom(BH1750_ADDR, 2) == 2) {
        value = i2c.read();
        value <<= 8;
        value |= i2c.read();
        float lux = value / 1.2;
        return lux;
    }
    return -1;
} 