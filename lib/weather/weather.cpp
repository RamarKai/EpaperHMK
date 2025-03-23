#include "weather.h"                  // 包含天气服务的头文件

WeatherData currentWeather;             // 定义全局变量存储当前天气数据

const char* WEATHER_API_ENDPOINT = "https://api.seniverse.com/v3/weather/now.json";  // 定义心知天气API接口地址

bool initWeatherService() {             // 初始化天气服务的函数
    if (WEATHER_API_KEY[0] == '\0') {   // 检查API密钥是否为空
        Serial.println("Weather API key or UID not set!");  // 如果API密钥为空则打印错误信息
        return false;                   // 返回false表示初始化失败
    }
    
    if (WiFi.status() != WL_CONNECTED) {  // 检查WiFi是否已连接
        Serial.println("WiFi not connected, weather service requires network access!");  // 如果WiFi未连接则打印错误信息
        return false;                     // 返回false表示初始化失败
    }
    
    Serial.println("Weather service initialized");  // 打印初始化成功信息
    return true;                         // 返回true表示初始化成功
}

bool updateWeatherData() {              // 更新天气数据的函数
    if (WiFi.status() != WL_CONNECTED) {  // 检查WiFi是否已连接
        Serial.println("WiFi not connected, cannot update weather!");  // 如果WiFi未连接则打印错误信息
        return false;                     // 返回false表示更新失败
    }
    
    HTTPClient http;                     // 创建HTTP客户端对象
    
    String url = String(WEATHER_API_ENDPOINT) + 
                 "?key=" + WEATHER_API_KEY + 
                 "&location=" + WEATHER_LOCATION + 
                 "&language=zh-Hans" +
                 "&unit=c";                // 构建完整的API请求URL，包含API密钥、位置等参数
    
    Serial.print("Requesting URL: ");     // 打印将要请求的URL信息
    Serial.println(url);                  // 打印完整的请求URL
    
    http.begin(url);                      // 初始化HTTP请求
    
    int httpCode = http.GET();            // 发送GET请求并获取响应码
    
    if (httpCode == HTTP_CODE_OK) {       // 如果HTTP请求成功(返回200)
        String payload = http.getString(); // 获取响应内容
        Serial.println("API Response:");   // 打印响应标题
        Serial.println(payload);           // 打印完整的响应内容
        
        JsonDocument doc;                  // 创建JSON文档对象
        
        DeserializationError error = deserializeJson(doc, payload);  // 解析JSON响应
        
        if (error) {                       // 如果JSON解析失败
            Serial.print("JSON parsing failed: ");  // 打印JSON解析失败信息
            Serial.println(error.c_str());  // 打印具体的错误信息
            http.end();                     // 结束HTTP会话
            return false;                   // 返回false表示更新失败
        }
        
        JsonObject results_0 = doc["results"][0];  // 获取结果数组的第一个元素
        JsonObject now = results_0["now"];         // 获取当前天气数据对象
        
        currentWeather.city = results_0["location"]["name"].as<String>();  // 提取并保存城市名称
        currentWeather.weather = now["text"].as<String>();                // 提取并保存天气情况
        currentWeather.temperature = now["temperature"].as<float>();       // 提取并保存温度
        currentWeather.humidity = now["humidity"].as<int>();              // 提取并保存湿度
        currentWeather.wind_speed = now["wind_speed"].as<float>();        // 提取并保存风速
        currentWeather.wind_direction = now["wind_direction"].as<String>();  // 提取并保存风向
        currentWeather.is_valid = true;             // 标记数据为有效
        currentWeather.last_update = millis();      // 记录更新时间
        
        http.end();                        // 结束HTTP会话
        Serial.println("Weather data updated successfully");  // 打印更新成功信息
        return true;                       // 返回true表示更新成功
    } else {                               // 如果HTTP请求失败
        Serial.print("HTTP request failed with error code: ");  // 打印HTTP请求失败信息
        Serial.println(httpCode);          // 打印HTTP错误码
        http.end();                        // 结束HTTP会话
        return false;                      // 返回false表示更新失败
    }
}

WeatherData getWeatherData() {            // 获取天气数据的函数
    return currentWeather;                // 返回当前天气数据
}

bool checkAndUpdateWeather() {            // 检查并更新天气数据的函数
    unsigned long currentTime = millis();  // 获取当前时间
    
    if (!currentWeather.is_valid ||       // 如果天气数据无效
        (currentTime - currentWeather.last_update > WEATHER_UPDATE_INTERVAL)) {  // 或者距离上次更新已经超过了更新间隔
        return updateWeatherData();        // 更新天气数据并返回更新结果
    }
    
    return false;                          // 如果不需要更新则返回false
}