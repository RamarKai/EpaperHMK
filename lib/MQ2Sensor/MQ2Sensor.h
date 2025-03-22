/**
 * @file MQ2Sensor.h
 * @brief MQ2气敏传感器驱动库
 * @details 提供MQ2气敏传感器的基本操作接口，包括初始化、数据读取、报警等功能
 */

#ifndef MQ2_SENSOR_H
#define MQ2_SENSOR_H

#include <Arduino.h>

class MQ2Sensor {
public:
    /**
     * @brief 构造函数
     * @param pin 传感器连接的模拟引脚
     */
    MQ2Sensor(uint8_t pin);

    /**
     * @brief 初始化传感器
     * @param warmupTime 预热时间(ms)，默认20000ms
     * @return 初始化是否成功
     */
    bool begin(uint32_t warmupTime = 20000);

    /**
     * @brief 读取传感器原始值
     * @return 0-4095的ADC值
     */
    uint16_t readRaw();

    /**
     * @brief 读取气体浓度值
     * @return 0-100的浓度值
     */
    uint8_t readConcentration();

    /**
     * @brief 设置报警阈值
     * @param threshold 报警阈值(0-100)
     */
    void setAlarmThreshold(uint8_t threshold);

    /**
     * @brief 获取报警状态
     * @return true表示超过报警阈值，false表示正常
     */
    bool isAlarm();

private:
    uint8_t _pin;                    // 传感器引脚
    uint8_t _alarmThreshold;         // 报警阈值
    uint16_t _rawValue;              // 原始值
    uint8_t _concentration;          // 浓度值
    bool _isWarmedUp;                // 预热状态
};

#endif // MQ2_SENSOR_H 