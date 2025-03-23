/*
 * Weather API Demo - 心知天气API使用示例
 * 
 * 本示例演示如何使用心知天气API库获取实时天气数据
 * 在使用前，请确保已正确配置WiFi连接和API密钥
 */

#include <Arduino.h>
#include <WiFi.h>
#include "weather.h"

// WiFi配置
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// 天气更新间隔(毫秒) - 设置为60秒，方便测试
// 注意：实际使用时建议增加间隔以避免超出API调用限制
#define DEMO_UPDATE_INTERVAL 60000

// 上次更新时间
unsigned long lastWeatherUpdateTime = 0;

void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial.println("\n\n=====================");
  Serial.println("心知天气API示例程序");
  Serial.println("=====================");
  
  // 连接到WiFi
  connectToWiFi();
  
  // 修改默认的更新间隔，便于测试
  // weather.h中定义的WEATHER_UPDATE_INTERVAL会被覆盖
  lastWeatherUpdateTime = millis() - DEMO_UPDATE_INTERVAL;
  
  // 初始化天气服务
  if (initWeatherService()) {
    Serial.println("天气服务初始化成功");
    
    // 立即更新一次天气数据
    if (updateWeatherData()) {
      displayWeatherData();
    } else {
      Serial.println("首次天气数据获取失败");
    }
  } else {
    Serial.println("天气服务初始化失败");
  }
}

void loop() {
  // 检查是否需要更新天气数据
  unsigned long currentTime = millis();
  if (currentTime - lastWeatherUpdateTime >= DEMO_UPDATE_INTERVAL) {
    Serial.println("\n检查天气更新...");
    
    if (checkAndUpdateWeather()) {
      Serial.println("天气数据已更新");
      displayWeatherData();
    } else {
      Serial.println("无需更新天气数据或更新失败");
    }
    
    lastWeatherUpdateTime = currentTime;
  }
  
  // 等待一段时间
  delay(1000);
}

/**
 * 连接到WiFi网络
 */
void connectToWiFi() {
  Serial.println("正在连接WiFi...");
  
  WiFi.begin(ssid, password);
  
  // 等待连接
  int attemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && attemptCount < 20) {
    delay(500);
    Serial.print(".");
    attemptCount++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi连接失败，请检查凭据");
  }
}

/**
 * 显示当前的天气数据
 */
void displayWeatherData() {
  // 获取天气数据
  WeatherData weather = getWeatherData();
  
  if (weather.is_valid) {
    Serial.println("\n当前天气信息:");
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
    Serial.print("风速: ");
    Serial.print(weather.wind_speed);
    Serial.println(" km/h");
    Serial.print("风向: ");
    Serial.println(weather.wind_direction);
    Serial.print("上次更新时间: ");
    Serial.print(weather.last_update / 1000);
    Serial.println(" 秒前");
  } else {
    Serial.println("无有效天气数据");
  }
} 