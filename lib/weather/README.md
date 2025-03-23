# 心知天气API库

该库提供了与心知天气API的集成，允许ESP32设备获取实时天气数据。

## 功能特点

- 获取城市实时天气信息
- 支持温度、湿度、风速等多种天气数据
- 自动缓存天气数据，减少API调用次数
- 非阻塞式设计，避免影响主程序运行

## 安装

将`weather`文件夹放在项目的`lib`目录下，然后在您的主程序中包含头文件：

```cpp
#include "weather.h"
```

## 配置

使用前需要在`weather.h`文件中设置您的心知天气API密钥和用户ID：

```cpp
#define WEATHER_API_KEY "YOUR_API_KEY"    // 填入您的心知天气API密钥
#define WEATHER_API_UID "YOUR_USER_ID"    // 填入您的心知天气用户ID
```

还可以设置天气更新的间隔时间和默认的城市位置：

```cpp
#define WEATHER_UPDATE_INTERVAL 600000    // 更新间隔，单位为毫秒，默认10分钟
#define WEATHER_LOCATION "beijing"        // 默认城市，可以是城市名称、ID或经纬度
```

## 使用方法

### 初始化

在您的`setup()`函数中初始化天气服务：

```cpp
void setup() {
    // 初始化WiFi连接（这里假设已完成WiFi连接）
    
    // 初始化天气服务
    if (!initWeatherService()) {
        Serial.println("Failed to initialize weather service!");
    }
}
```

### 获取天气数据

在您的`loop()`函数中定期检查并更新天气数据：

```cpp
void loop() {
    // 检查并更新天气数据
    if (checkAndUpdateWeather()) {
        // 天气数据已更新，可以进行处理
        WeatherData weather = getWeatherData();
        
        Serial.print("城市: ");
        Serial.println(weather.city);
        Serial.print("天气: ");
        Serial.println(weather.weather);
        Serial.print("温度: ");
        Serial.print(weather.temperature);
        Serial.println("°C");
        Serial.print("湿度: ");
        Serial.print(weather.humidity);
        Serial.println("%");
    }
    
    // 其他代码...
}
```

### 手动更新天气数据

如果需要立即更新天气数据，可以调用`updateWeatherData()`函数：

```cpp
if (updateWeatherData()) {
    Serial.println("天气数据已成功更新");
} else {
    Serial.println("天气数据更新失败");
}
```

## 数据结构

天气数据存储在`WeatherData`结构体中：

```cpp
struct WeatherData {
    String city;          // 城市名称
    String weather;       // 天气状况描述
    float temperature;    // 温度(摄氏度)
    int humidity;         // 湿度(%)
    float wind_speed;     // 风速(km/h)
    String wind_direction;// 风向
    bool is_valid;        // 数据是否有效
    unsigned long last_update; // 上次更新时间
};
```

## 错误处理

库中的函数通常返回布尔值，表示操作是否成功：

- `initWeatherService()`: 返回`true`表示初始化成功
- `updateWeatherData()`: 返回`true`表示更新成功
- `checkAndUpdateWeather()`: 返回`true`表示已进行更新，`false`表示未更新

如果数据无效，`WeatherData.is_valid`字段将为`false`。

## 内存占用

该库使用ArduinoJson库来解析API响应，根据天气数据复杂度，可能需要调整JSON文档的大小。

## 注意事项

- 使用该库需要稳定的WiFi连接
- 请注意心知天气API的使用限制和速率限制
- API密钥不应在公开场合分享

## 心知天气API文档

更多关于心知天气API的信息，请访问:
- [心知天气API文档](https://seniverse.yuque.com/hyper_data/api_v3/nyiu3t)
- [天气实况接口](https://seniverse.yuque.com/hyper_data/api_v3/sl6gvt) 