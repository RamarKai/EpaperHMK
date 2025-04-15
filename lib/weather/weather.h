#ifndef WEATHER_H // 防止头文件被重复包含的宏定义开始
#define WEATHER_H // 定义宏以标记该头文件已被包含

#include <Arduino.h>          // 包含Arduino核心库
#include <WiFi.h>             // 包含WiFi功能库
#include <WiFiClientSecure.h> // 包含安全WiFi客户端库，用于HTTPS连接
#include <HTTPClient.h>       // 包含HTTP客户端库，用于发送网络请求
#include <ArduinoJson.h>      // 包含ArduinoJson库，用于解析JSON数据

#define WEATHER_API_KEY "SWL_pIpiyT3moGyF5" // 定义心知天气API密钥

#define WEATHER_UPDATE_INTERVAL 600000 // 定义天气数据更新间隔为600000毫秒(10分钟)

#define WEATHER_LOCATION "fujianfuzhou" // 定义天气查询位置为"福建福州"

struct WeatherData
{                              // 定义天气数据结构体
    String city;               // 城市名称
    String weather;            // 天气情况描述
    float temperature;         // 温度，单位摄氏度
    int humidity;              // 湿度，单位百分比
    float wind_speed;          // 风速
    String wind_direction;     // 风向
    bool is_valid;             // 数据是否有效的标志
    unsigned long last_update; // 上次更新的时间戳

    WeatherData() :                            // 结构体构造函数，初始化所有成员
                    city("Unknown"),           // 初始化城市为"Unknown"
                    weather("Unknown"),        // 初始化天气为"Unknown"
                    temperature(0),            // 初始化温度为0
                    humidity(0),               // 初始化湿度为0
                    wind_speed(0),             // 初始化风速为0
                    wind_direction("Unknown"), // 初始化风向为"Unknown"
                    is_valid(false),           // 初始化数据有效标志为false
                    last_update(0)
    {
    } // 初始化上次更新时间为0
};

bool initWeatherService(); // 声明初始化天气服务的函数

bool updateWeatherData(); // 声明更新天气数据的函数

WeatherData getWeatherData(); // 声明获取天气数据的函数

bool checkAndUpdateWeather(); // 声明检查并更新天气数据的函数

extern WeatherData currentWeather; // 声明外部当前天气数据变量

#endif // 防止头文件被重复包含的宏定义结束