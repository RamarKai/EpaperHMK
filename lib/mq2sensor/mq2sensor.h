/**
 * @file mq2sensor.h
 * @brief MQ2气体传感器相关功能声明
 * @details 包含MQ2气体传感器读取和HomeSpan空气质量服务声明
 */

#ifndef MQ2SENSOR_H    // 头文件保护宏定义开始
#define MQ2SENSOR_H    // 定义头文件保护宏

#include "HomeSpan.h"   // 包含HomeSpan库，提供HomeKit功能支持
#include <Arduino.h>    // 包含Arduino基础库

// MQ2传感器引脚定义
#define MQ2_PIN 34      // MQ2气体传感器模拟输入引脚

// 全局变量声明
extern int gasLevel;    // 气体浓度全局变量，用于在不同组件间共享气体浓度数据

/**
 * @brief 读取MQ2传感器的气体浓度
 * @details 通过模拟引脚读取MQ2传感器数据
 * @return int 返回气体浓度原始值(0-4095)
 */
int readMQ2Sensor();

/**
 * @brief 将MQ2原始数据转换为空气质量等级
 * @details 根据传感器数值范围划分空气质量等级
 * @param reading MQ2传感器的原始读数
 * @return int 空气质量等级(1-5)，1为最好，5为最差
 */
int convertToAirQuality(int reading);

/**
 * @brief 初始化MQ2传感器
 * @details 设置MQ2传感器的引脚模式
 */
void initMQ2Sensor();

/**
 * @brief HomeSpan空气质量传感器服务类
 * @details 实现了HomeKit空气质量传感器服务，定期读取气体数据并更新HomeKit特征值
 */
struct DEV_AirQualitySensor : Service::AirQualitySensor {
    SpanCharacteristic *airQuality;          // 空气质量特征
    unsigned long lastReadTime = 0;          // 上次读取时间，用于控制读取频率

    /**
     * @brief 构造函数，初始化空气质量传感器服务
     */
    DEV_AirQualitySensor();

    /**
     * @brief HomeSpan循环函数，定期更新空气质量数据
     * @details 读取MQ2传感器数据，并将其转换为HomeKit空气质量等级
     */
    void loop();
};

#endif  // MQ2SENSOR_H    // 头文件保护宏定义结束 