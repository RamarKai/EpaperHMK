/**
 * @file mq2sensor.cpp
 * @brief MQ2气体传感器相关功能实现
 * @details 包含MQ2气体传感器读取和HomeSpan空气质量服务实现
 */

#include "mq2sensor.h"    // 包含对应的头文件

// 全局变量定义
int gasLevel = 0;       // 气体浓度全局变量，用于在不同组件间共享气体浓度数据

/**
 * @brief 读取MQ2传感器的气体浓度
 * @details 通过模拟引脚读取MQ2传感器数据
 * @return int 返回气体浓度原始值(0-4095)
 */
int readMQ2Sensor() {
    // 读取MQ2传感器的模拟值
    int reading = analogRead(MQ2_PIN);
    return reading;
}

/**
 * @brief 将MQ2原始数据转换为空气质量等级
 * @details 根据传感器数值范围划分空气质量等级
 * @param reading MQ2传感器的原始读数
 * @return int 空气质量等级(1-5)，1为最好，5为最差
 */
int convertToAirQuality(int reading) {
    // 根据气体浓度确定空气质量等级
    int quality;
    if(reading < 1000) quality = 1;      // EXCELLENT - 优
    else if(reading < 2000) quality = 2;  // GOOD - 良
    else if(reading < 3000) quality = 3;  // FAIR - 一般
    else if(reading < 4000) quality = 4;  // INFERIOR - 差
    else quality = 5;                     // POOR - 极差
    
    return quality;
}

/**
 * @brief 初始化MQ2传感器
 * @details 设置MQ2传感器的引脚模式
 */
void initMQ2Sensor() {
    // 初始化MQ2传感器引脚为输入模式
    pinMode(MQ2_PIN, INPUT);
    Serial.println("MQ2 sensor initialized");
}

/**
 * @brief DEV_AirQualitySensor类构造函数，初始化空气质量传感器服务
 */
DEV_AirQualitySensor::DEV_AirQualitySensor() : Service::AirQualitySensor() {
    airQuality = new Characteristic::AirQuality(0);  // 创建空气质量特征
}

/**
 * @brief DEV_AirQualitySensor类的loop方法实现，定期更新空气质量数据
 * @details 读取MQ2传感器数据，并将其转换为HomeKit空气质量等级
 */
void DEV_AirQualitySensor::loop() {
    // 每500毫秒读取一次，控制读取频率
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= 500) {
        // 读取MQ2传感器数值
        int reading = readMQ2Sensor();
        // 更新全局变量，用于OLED显示等
        gasLevel = reading;
        
        // 转换为空气质量等级
        int quality = convertToAirQuality(reading);
        
        // 只有在空气质量等级发生变化时才更新HomeKit特征值，减少无谓通信
        if(quality != airQuality->getVal()) {
            airQuality->setVal(quality);  // 更新HomeKit特征值
        }
        
        lastReadTime = currentTime; // 更新最后读取时间
    }
}
