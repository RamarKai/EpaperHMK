/**
 * @file weather.h
 * @brief 心知天气API接口封装
 * @details 用于从心知天气API获取实时天气数据
 */

#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 心知天气API密钥
#define WEATHER_API_KEY "SWL_pIpiyT3moGyF5"    // 填入您的心知天气API密钥
//#define WEATHER_API_UID ""    // 填入您的心知天气用户ID

// 天气接口请求间隔(毫秒) - 默认10分钟
#define WEATHER_UPDATE_INTERVAL 600000

// 城市代码或经纬度坐标，默认为北京
#define WEATHER_LOCATION "fujianfuzhou"

// 天气数据结构体
struct WeatherData {
    String city;          // 城市名称
    String weather;       // 天气状况描述
    float temperature;    // 温度(摄氏度)
    int humidity;         // 湿度(%)
    float wind_speed;     // 风速(km/h)
    String wind_direction;// 风向
    bool is_valid;        // 数据是否有效
    unsigned long last_update; // 上次更新时间

    // 构造函数，设置默认值
    WeatherData() : 
        city("Unknown"), 
        weather("Unknown"), 
        temperature(0), 
        humidity(0), 
        wind_speed(0), 
        wind_direction("Unknown"), 
        is_valid(false), 
        last_update(0) {}
};

/**
 * @brief 初始化天气服务
 * @details 配置WiFi连接和初始化API参数
 * @return 初始化是否成功
 */
bool initWeatherService();

/**
 * @brief 更新天气数据
 * @details 从心知天气API获取最新的天气数据
 * @return 更新是否成功
 */
bool updateWeatherData();

/**
 * @brief 获取当前的天气数据
 * @details 返回最近一次获取的天气数据
 * @return WeatherData 天气数据结构体
 */
WeatherData getWeatherData();

/**
 * @brief 检查并更新天气数据
 * @details 如果距离上次更新时间已经超过WEATHER_UPDATE_INTERVAL，则重新获取天气数据
 * @return 是否进行了更新
 */
bool checkAndUpdateWeather();

// 外部天气数据对象
extern WeatherData currentWeather;

#endif // WEATHER_H 