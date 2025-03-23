#ifndef LIGHTSENSOR_H          // 防止头文件被重复包含的宏定义开始
#define LIGHTSENSOR_H          // 定义宏以标记该头文件已被包含

#include "HomeSpan.h"          // 包含HomeSpan库，用于HomeKit集成
#include <Wire.h>              // 包含Wire库，用于I2C通信

#define BH1750_SDA 4           // 定义BH1750光线传感器的SDA引脚为GPIO4
#define BH1750_SCL 5           // 定义BH1750光线传感器的SCL引脚为GPIO5

extern float lightLevel;       // 声明外部变量lightLevel，用于存储光照强度值
extern bool bh1750_ok;         // 声明外部变量bh1750_ok，标记BH1750传感器初始化状态
extern TwoWire I2C_BH1750;     // 声明外部变量I2C_BH1750，用于BH1750的I2C通信

void initBH1750();             // 声明初始化BH1750传感器的函数

float readBH1750();            // 声明读取BH1750传感器光照值的函数

struct DEV_LightSensor : Service::LightSensor {  // 定义LightSensor服务结构体，继承自HomeSpan的LightSensor服务
    SpanCharacteristic *lightLevel;              // 指向光照级别特性的指针
    unsigned long lastReadTime = 0;              // 记录上次读取传感器的时间

    DEV_LightSensor();                           // 构造函数声明

    void loop();                                 // 循环函数声明，用于定期更新传感器数据
};

#endif                         // 防止头文件被重复包含的宏定义结束