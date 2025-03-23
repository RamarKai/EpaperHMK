/**
 * @file lightsensor.h
 * @brief BH1750光照传感器相关功能声明
 * @details 包含BH1750传感器初始化、读取和HomeSpan服务声明
 */

#ifndef LIGHTSENSOR_H    // 防止头文件重复包含
#define LIGHTSENSOR_H

#include "HomeSpan.h"    // 包含HomeSpan库，提供HomeKit功能支持
#include <Wire.h>        // 包含Wire库，用于I2C通信

// BH1750相关引脚定义
#define BH1750_SDA 4     // BH1750的SDA引脚
#define BH1750_SCL 5     // BH1750的SCL引脚

// 外部全局变量声明
extern float lightLevel;   // 光照强度全局变量，用于在不同组件间共享光照数据
extern bool bh1750_ok;    // BH1750状态标志，指示传感器是否正常工作
extern TwoWire I2C_BH1750; // BH1750使用的I2C实例

/**
 * @brief 初始化BH1750光照传感器
 * @details 配置I2C接口并设置BH1750为连续高分辨率模式
 * @return 无返回值，但会设置全局的bh1750_ok标志
 */
void initBH1750();

/**
 * @brief 读取BH1750传感器的光照强度
 * @details 通过I2C读取BH1750传感器数据并转换为光照强度（lux）
 * @return float 返回光照强度值（lux），出错时返回-1
 */
float readBH1750();

/**
 * @brief HomeSpan光照传感器服务类
 * @details 实现了HomeKit光照传感器服务，定期读取光照数据并更新HomeKit特征值
 */
struct DEV_LightSensor : Service::LightSensor {
    SpanCharacteristic *lightLevel;          // 光照级别特征
    unsigned long lastReadTime = 0;          // 上次读取时间，用于控制读取频率

    /**
     * @brief 构造函数，初始化光照传感器服务
     */
    DEV_LightSensor();

    /**
     * @brief HomeSpan循环函数，定期更新光照数据
     * @details 每秒读取一次光照数据，并在数值变化时更新HomeKit特征值
     */
    void loop();
};

#endif // LIGHTSENSOR_H 