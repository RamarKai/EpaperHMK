#ifndef DHT11_HOMEKIT_H        // 防止头文件被重复包含的宏定义开始
#define DHT11_HOMEKIT_H        // 定义宏以标记该头文件已被包含

#include "HomeSpan.h"          // 包含HomeSpan库
#include "dht11.h"             // 包含DHT11传感器库

// DHT11温度传感器的HomeKit服务类
struct DEV_TemperatureSensor : Service::TemperatureSensor {
    SpanCharacteristic *currentTemperature;  // HomeKit温度特性
    
    // 构造函数
    DEV_TemperatureSensor() : Service::TemperatureSensor() {
        // 创建当前温度特性，初始值为0，最小值-50，最大值100，步长0.1
        currentTemperature = new Characteristic::CurrentTemperature(0);
        currentTemperature->setRange(-50, 100);  // 设置温度范围
        currentTemperature->setUnit(0);          // 设置单位为摄氏度
        
        Serial.println("HomeKit Temperature Sensor Service initialized");  // 打印初始化成功信息
    }
    
    // 更新函数，由HomeSpan框架定期调用
    void loop() {
        // 如果DHT11传感器正常工作，则更新温度值
        if (dht11_ok && isDHT11Working()) {
            currentTemperature->setVal(temperature);  // 更新HomeKit温度值
        }
    }
};

// DHT11湿度传感器的HomeKit服务类
struct DEV_HumiditySensor : Service::HumiditySensor {
    SpanCharacteristic *currentRelativeHumidity;  // HomeKit湿度特性
    
    // 构造函数
    DEV_HumiditySensor() : Service::HumiditySensor() {
        // 创建当前相对湿度特性，初始值为0，最小值0，最大值100，步长0.1
        currentRelativeHumidity = new Characteristic::CurrentRelativeHumidity(0);
        
        Serial.println("HomeKit Humidity Sensor Service initialized");  // 打印初始化成功信息
    }
    
    // 更新函数，由HomeSpan框架定期调用
    void loop() {
        // 如果DHT11传感器正常工作，则更新湿度值
        if (dht11_ok && isDHT11Working()) {
            currentRelativeHumidity->setVal(humidity);  // 更新HomeKit湿度值
        }
    }
};

#endif                      // 防止头文件被重复包含的宏定义结束 