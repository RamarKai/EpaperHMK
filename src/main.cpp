#include "HomeSpan.h"
//#include <Wire.h>
//#include <Adafruit_SSD1306.h>
//#include <FastLED.h>

// 包含光照传感器头文件
#include "lightsensor.h"
// 包含气体传感器头文件
#include "mq2sensor.h"
// 包含OLED显示屏头文件
#include "oled.h"
// 包含WS2812B LED头文件
#include "ws2812.h"
// 包含天气服务头文件
#include "weather.h"

// WS2812B LED相关代码已移至ws2812.cpp
// OLED相关定义已移至oled.cpp
// MQ2_PIN已移至mq2sensor.cpp
// BH1750光传感器引脚已移至lightsensor.cpp

// 用于非阻塞显示更新的变量
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 100; // 显示更新间隔为100毫秒

// 用于非阻塞天气更新的变量
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 300000; // 天气更新间隔为5分钟(300000ms)

// WiFi凭据
const char* ssid = "YOUR_WIFI_SSID";       // 请替换为您的WiFi名称
const char* password = "YOUR_WIFI_PASSWORD"; // 请替换为您的WiFi密码

void setup() {
    // 初始化串口通信
    Serial.begin(115200);
    Serial.println("Starting setup...");
    
    // 初始化OLED显示屏
    if(!initOLED()) {
        Serial.println("OLED initialization failed!");
        for(;;); // 如果OLED初始化失败，进入死循环
    }
    
    // 显示启动画面
    showStartupScreen();
    
    // 连接WiFi
    connectToWiFi();
    
    // 初始化天气服务
    if (initWeatherService()) {
        Serial.println("Weather service initialized");
        // 首次获取天气数据
        if (updateWeatherData()) {
            Serial.println("Initial weather data updated successfully");
        } else {
            Serial.println("Failed to get initial weather data");
        }
    } else {
        Serial.println("Weather service initialization failed");
        showError("Weather service init failed");
        delay(2000);
    }
    
    // 初始化BH1750光照传感器
    initBH1750();
    
    // 初始化MQ2气体传感器
    initMQ2Sensor();
    
    // 初始化WS2812B LED
    initWS2812B();
    
    // 初始化HomeSpan
    homeSpan.begin(Category::Lighting, "ESP32 Sensors");

    // 创建主配件
    new SpanAccessory();                                                          
        new Service::AccessoryInformation();
            new Characteristic::Identify();                
            new Characteristic::Name("ESP32 Sensors");    
            new Characteristic::Manufacturer("DIY");      
            new Characteristic::SerialNumber("123-ABC");  
            new Characteristic::Model("ESP32");          
            new Characteristic::FirmwareRevision("1.0"); 
            
        new DEV_WS2812B();                  // 添加WS2812B LED服务
        new DEV_LightSensor();              // 添加光照传感器服务
        new DEV_AirQualitySensor();         // 添加空气质量传感器服务
}

void loop() {
    homeSpan.poll();                        // HomeSpan主循环，每次循环都执行
    
    unsigned long currentMillis = millis();
    
    // 非阻塞方式更新显示 - 每100ms更新一次
    if(currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        updateDisplay();                    // 更新OLED显示
        lastDisplayUpdate = currentMillis;  // 更新上次显示时间
    }
    
    // 非阻塞方式更新天气数据 - 每5分钟更新一次
    if(currentMillis - lastWeatherUpdate >= weatherUpdateInterval) {
        Serial.println("Updating weather data...");
        if(checkAndUpdateWeather()) {
            Serial.println("Weather data updated successfully");
        }
        lastWeatherUpdate = currentMillis;
    }
}

/**
 * 连接到WiFi网络
 */
void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to WiFi:");
    display.setCursor(0, 16);
    display.println(ssid);
    display.display();
    
    WiFi.begin(ssid, password);
    
    // 等待连接
    int attemptCount = 0;
    while (WiFi.status() != WL_CONNECTED && attemptCount < 20) {
        delay(500);
        Serial.print(".");
        display.print(".");
        display.display();
        attemptCount++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("WiFi Connected!");
        display.setCursor(0, 16);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.display();
        delay(2000);
    } else {
        Serial.println("\nWiFi connection failed");
        showError("WiFi connection failed");
        delay(2000);
    }
}
