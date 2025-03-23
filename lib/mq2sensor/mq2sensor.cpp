#include "mq2sensor.h"                  // 包含MQ2气体传感器的头文件

int gasLevel = 0;                       // 定义全局变量存储气体浓度值，初始化为0

int readMQ2Sensor() {                   // 读取MQ2传感器数据的函数
    int reading = analogRead(MQ2_PIN);  // 从定义的引脚读取模拟值
    return reading;                     // 返回读取的原始值
}

int convertToAirQuality(int reading) {  // 将传感器原始值转换为HomeKit空气质量等级的函数
    int quality;                        // 定义变量存储空气质量等级
    if(reading < 1000) quality = 1;     // 小于1000表示空气质量优，对应HomeKit的EXCELLENT(1)
    else if(reading < 2000) quality = 2; // 小于2000表示空气质量良，对应HomeKit的GOOD(2)
    else if(reading < 3000) quality = 3; // 小于3000表示空气质量中，对应HomeKit的FAIR(3)
    else if(reading < 4000) quality = 4; // 小于4000表示空气质量差，对应HomeKit的INFERIOR(4)
    else quality = 5;                   // 大于或等于4000表示空气质量很差，对应HomeKit的POOR(5)
    
    return quality;                     // 返回空气质量等级
}

void initMQ2Sensor() {                  // 初始化MQ2传感器的函数
    pinMode(MQ2_PIN, INPUT);            // 设置MQ2传感器引脚为输入模式
    Serial.println("MQ2 sensor initialized");  // 打印初始化成功信息
}

DEV_AirQualitySensor::DEV_AirQualitySensor() : Service::AirQualitySensor() {  // AirQualitySensor服务的构造函数
    airQuality = new Characteristic::AirQuality(0);  // 创建空气质量特性并设置初始值为0
}

void DEV_AirQualitySensor::loop() {     // 周期性执行的循环函数
    unsigned long currentTime = millis();  // 获取当前时间
    if(currentTime - lastReadTime >= 500) {  // 检查是否距离上次读取已经过去了500毫秒
        int reading = readMQ2Sensor();    // 读取传感器值
        gasLevel = reading;               // 更新全局变量的气体浓度值
        
        int quality = convertToAirQuality(reading);  // 将读数转换为空气质量等级
        
        if(quality != airQuality->getVal()) {  // 检查值是否发生变化
            airQuality->setVal(quality);       // 更新HomeKit的空气质量特性值
        }
        
        lastReadTime = currentTime;        // 更新上次读取时间
    }
}
