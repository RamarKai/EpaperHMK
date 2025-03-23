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

// 天气服务状态变量
bool weatherServiceInitialized = false;  // 天气服务是否初始化
bool weatherInitialUpdateDone = false;   // 是否已经进行了首次天气数据更新
bool wifiPreviouslyConnected = false;    // 上一次WiFi是否已连接

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
    
    // 初始化BH1750光照传感器
    initBH1750();
    
    // 初始化MQ2气体传感器
    initMQ2Sensor();
    
    // 初始化WS2812B LED
    initWS2812B();
    
    // 初始化HomeSpan - WiFi连接将在homeSpan.poll()时进行
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
    
    // 显示等待WiFi连接的提示
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("HomeSpan Starting...");
    display.setCursor(0, 16);
    display.println("Waiting for WiFi...");
    display.display();
    
    // 初始化更新时间
    lastDisplayUpdate = millis();
    lastWeatherUpdate = millis();
}

void loop() {
    // HomeSpan主循环，WiFi连接和HomeKit通信
    homeSpan.poll();
    
    unsigned long currentMillis = millis();
    
    // 非阻塞方式更新显示 - 每100ms更新一次
    if(currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        updateDisplay();                    // 更新OLED显示
        lastDisplayUpdate = currentMillis;  // 更新上次显示时间
    }
    
    // 检查WiFi连接状态
    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    
    // WiFi连接状态变化检测 - 从断开到连接
    if (wifiConnected && !wifiPreviouslyConnected) {
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        // 显示WiFi连接成功信息
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("WiFi Connected!");
        display.setCursor(0, 16);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.display();
        delay(2000); // 显示2秒
        
        // 初始化天气服务 - 只在首次连接WiFi时进行
        if (!weatherServiceInitialized) {
            if (initWeatherService()) {
                Serial.println("Weather service initialized");
                weatherServiceInitialized = true;
                
                // 首次更新天气数据在下一个if块中处理
            } else {
                Serial.println("Weather service initialization failed");
                showError("Weather service init failed");
                delay(2000);
            }
        }
    }
    // WiFi连接状态变化检测 - 从连接到断开
    else if (!wifiConnected && wifiPreviouslyConnected) {
        Serial.println("WiFi disconnected!");
    }
    
    // 更新上一次WiFi状态
    wifiPreviouslyConnected = wifiConnected;
    
    // 如果WiFi已连接且天气服务已初始化
    if (wifiConnected && weatherServiceInitialized) {
        // 首次天气数据更新 - 只在天气服务初始化后执行一次
        if (!weatherInitialUpdateDone) {
            Serial.println("Performing initial weather data update...");
            if (updateWeatherData()) {
                Serial.println("Initial weather data updated successfully");
                weatherInitialUpdateDone = true;
            } else {
                Serial.println("Failed to get initial weather data");
                // 稍后将再次尝试更新
            }
            lastWeatherUpdate = currentMillis;
        }
        // 定期天气数据更新 - 每5分钟更新一次
        else if (currentMillis - lastWeatherUpdate >= weatherUpdateInterval) {
            Serial.println("Updating weather data...");
            if (checkAndUpdateWeather()) {
                Serial.println("Weather data updated successfully");
            } else {
                Serial.println("Weather data update failed");
            }
            lastWeatherUpdate = currentMillis;
        }
    }
}
