#ifndef DHT11_H // 防止头文件被重复包含的宏定义开始
#define DHT11_H // 定义宏以标记该头文件已被包含

#include <DHT.h>      // 包含DHT传感器库
#include "HomeSpan.h" // 包含HomeSpan库

#define DHT_PIN 4      // 定义DHT11传感器连接的GPIO引脚为GPIO4
#define DHT_TYPE DHT11 // 定义使用的传感器类型为DHT11

// 初始化DHT11传感器
bool initDHT11();

// 读取DHT11传感器的温度数据
float readTemperature();

// 读取DHT11传感器的湿度数据
float readHumidity();

// 检查DHT11传感器是否正常工作
bool isDHT11Working();

// 定义温度传感器结构体
struct DEV_TemperatureSensor : Service::TemperatureSensor
{
    SpanCharacteristic *currentTemperature;
    unsigned long lastReadTime = 0;

    DEV_TemperatureSensor();
    void loop();
};

// 定义湿度传感器结构体
struct DEV_HumiditySensor : Service::HumiditySensor
{
    SpanCharacteristic *currentHumidity;
    unsigned long lastReadTime = 0;

    DEV_HumiditySensor();
    void loop();
};

// 外部声明，用于在其他文件中访问
extern DHT dht;           // 声明外部DHT对象
extern float temperature; // 声明外部温度变量
extern float humidity;    // 声明外部湿度变量
extern bool dht11_ok;     // 声明外部DHT11状态变量

#endif // 防止头文件被重复包含的宏定义结束