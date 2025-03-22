/**
 * @file MQ2Sensor.cpp
 * @brief MQ2气敏传感器驱动库实现
 */

#include "MQ2Sensor.h"

MQ2Sensor::MQ2Sensor(uint8_t pin) : _pin(pin) {
    _alarmThreshold = 50;    // 默认报警阈值50%
    _rawValue = 0;
    _concentration = 0;
    _isWarmedUp = false;
}

bool MQ2Sensor::begin(uint32_t warmupTime) {
    // 配置引脚为输入模式
    pinMode(_pin, INPUT);
    
    // 等待预热
    delay(warmupTime);
    _isWarmedUp = true;
    
    return true;
}

uint16_t MQ2Sensor::readRaw() {
    if (!_isWarmedUp) {
        return 0;
    }
    
    // 读取多次取平均值，提高精度
    uint32_t sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += analogRead(_pin);
        delay(1);
    }
    
    _rawValue = sum / 10;
    return _rawValue;
}

uint8_t MQ2Sensor::readConcentration() {
    if (!_isWarmedUp) {
        return 0;
    }
    
    // 读取原始值
    readRaw();
    
    // 将ADC值映射到0-100的范围
    // 注意：这里的映射关系需要根据实际传感器特性进行调整
    _concentration = map(_rawValue, 0, 4095, 0, 100);
    
    return _concentration;
}

void MQ2Sensor::setAlarmThreshold(uint8_t threshold) {
    if (threshold > 100) {
        threshold = 100;
    }
    _alarmThreshold = threshold;
}

bool MQ2Sensor::isAlarm() {
    if (!_isWarmedUp) {
        return false;
    }
    
    return _concentration >= _alarmThreshold;
} 