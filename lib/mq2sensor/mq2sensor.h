#ifndef MQ2SENSOR_H           // 防止头文件被重复包含的宏定义开始
#define MQ2SENSOR_H           // 定义宏以标记该头文件已被包含

#include "HomeSpan.h"         // 包含HomeSpan库，用于HomeKit集成
#include <Arduino.h>          // 包含Arduino核心库

#define MQ2_PIN 34            // 定义MQ2气体传感器连接的模拟输入引脚为GPIO34

extern int gasLevel;          // 声明外部变量gasLevel，用于存储气体浓度值

int readMQ2Sensor();          // 声明读取MQ2传感器模拟值的函数

int convertToAirQuality(int reading);  // 声明将传感器原始值转换为空气质量等级的函数

void initMQ2Sensor();         // 声明初始化MQ2传感器的函数

struct DEV_AirQualitySensor : Service::AirQualitySensor {  // 定义AirQualitySensor服务结构体，继承自HomeSpan的AirQualitySensor服务
    SpanCharacteristic *airQuality;                       // 指向空气质量特性的指针
    unsigned long lastReadTime = 0;                       // 记录上次读取传感器的时间

    DEV_AirQualitySensor();                              // 构造函数声明

    void loop();                                         // 循环函数声明，用于定期更新传感器数据
};

#endif                         // 防止头文件被重复包含的宏定义结束