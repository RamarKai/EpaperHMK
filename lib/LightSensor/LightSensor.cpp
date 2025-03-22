#include "LightSensor.h"

// 构造函数
LightSensor::LightSensor(uint8_t address) : sensor(address) {
    initialized = false;
}

// 初始化传感器
bool LightSensor::begin(TwoWire* wire) {
    // 使用默认的连续高分辨率模式初始化
    initialized = sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, wire);
    return initialized;
}

// 读取光照强度
float LightSensor::readLightLevel() {
    // 检查是否已初始化
    if (!initialized) {
        return -1;
    }
    
    // 读取并返回光照值
    return sensor.readLightLevel();
}

// 设置测量模式
bool LightSensor::setMode(BH1750::Mode mode) {
    if (!initialized) {
        return false;
    }
    return sensor.configure(mode);
}

// 设置测量时间寄存器值
bool LightSensor::setMTreg(uint8_t mtreg) {
    if (!initialized) {
        return false;
    }
    return sensor.setMTreg(mtreg);
}

// 检查是否可以进行新的测量
bool LightSensor::isReady(bool maxWait) {
    if (!initialized) {
        return false;
    }
    return sensor.measurementReady(maxWait);
} 