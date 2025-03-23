/**
 * @file weather.cpp
 * @brief 心知天气API接口实现
 * @details 用于从心知天气API获取实时天气数据并解析
 */

#include "weather.h"

// 全局天气数据对象
WeatherData currentWeather;

// API URL常量
const char* WEATHER_API_ENDPOINT = "https://api.seniverse.com/v3/weather/now.json";

/**
 * @brief 初始化天气服务
 * @details 配置WiFi连接(如果未连接)和初始化API参数
 * @return 初始化是否成功
 */
bool initWeatherService() {
    if (WEATHER_API_KEY[0] == '\0') { //|| WEATHER_API_UID[0] == '\0') {
        Serial.println("Weather API key or UID not set!");
        return false;
    }
    
    // 检查WiFi是否已连接
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, weather service requires network access!");
        return false;
    }
    
    Serial.println("Weather service initialized");
    return true;
}

/**
 * @brief 更新天气数据
 * @details 从心知天气API获取最新的天气数据
 * @return 更新是否成功
 */
bool updateWeatherData() {
    // 检查WiFi连接
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, cannot update weather!");
        return false;
    }
    
    // 创建HTTP客户端
    HTTPClient http;
    
    // 构造完整的API URL
    String url = String(WEATHER_API_ENDPOINT) + 
                 "?key=" + WEATHER_API_KEY + 
                 "&location=" + WEATHER_LOCATION + 
                 "&language=zh-Hans" +  // 使用简体中文
                 "&unit=c";             // 使用摄氏度
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // 开始HTTP请求
    http.begin(url);
    
    // 发送GET请求
    int httpCode = http.GET();
    
    // 检查HTTP响应
    if (httpCode == HTTP_CODE_OK) {
        // 获取响应内容
        String payload = http.getString();
        Serial.println("API Response:");
        Serial.println(payload);
        
        // 创建JSON文档
        JsonDocument doc;
        
        // 解析JSON响应
        DeserializationError error = deserializeJson(doc, payload);
        
        // 检查解析是否成功
        if (error) {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }
        
        // 从JSON提取天气数据
        JsonObject results_0 = doc["results"][0];
        JsonObject now = results_0["now"];
        
        // 更新天气数据结构
        currentWeather.city = results_0["location"]["name"].as<String>();
        currentWeather.weather = now["text"].as<String>();
        currentWeather.temperature = now["temperature"].as<float>();
        currentWeather.humidity = now["humidity"].as<int>();
        currentWeather.wind_speed = now["wind_speed"].as<float>();
        currentWeather.wind_direction = now["wind_direction"].as<String>();
        currentWeather.is_valid = true;
        currentWeather.last_update = millis();
        
        http.end();
        Serial.println("Weather data updated successfully");
        return true;
    } else {
        Serial.print("HTTP request failed with error code: ");
        Serial.println(httpCode);
        http.end();
        return false;
    }
}

/**
 * @brief 获取当前的天气数据
 * @details 返回最近一次获取的天气数据
 * @return WeatherData 天气数据结构体
 */
WeatherData getWeatherData() {
    return currentWeather;
}

/**
 * @brief 检查并更新天气数据
 * @details 如果距离上次更新时间已经超过WEATHER_UPDATE_INTERVAL，则重新获取天气数据
 * @return 是否进行了更新
 */
bool checkAndUpdateWeather() {
    unsigned long currentTime = millis();
    
    // 检查是否第一次调用或者已经超过更新间隔
    if (!currentWeather.is_valid || 
        (currentTime - currentWeather.last_update > WEATHER_UPDATE_INTERVAL)) {
        return updateWeatherData();
    }
    
    return false; // 未更新，因为未达到更新间隔
} 