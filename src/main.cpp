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

// WS2812B LED相关代码已移至ws2812.cpp
// OLED相关定义已移至oled.cpp
// MQ2_PIN已移至mq2sensor.cpp
// BH1750光传感器引脚已移至lightsensor.cpp

// 用于非阻塞显示更新的变量
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 100; // 显示更新间隔为100毫秒

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
    
    // 非阻塞方式更新显示 - 每100ms更新一次
    unsigned long currentMillis = millis();
    if(currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        updateDisplay();                    // 更新OLED显示
        lastDisplayUpdate = currentMillis;  // 更新上次显示时间
    }
}
