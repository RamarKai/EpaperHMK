/**
 * @file ws2812.cpp
 * @brief WS2812B LED控制功能实现
 * @details 包含WS2812B LED的初始化、控制和HomeSpan智能灯泡服务实现
 */

#include "HomeSpan.h"      // 包含HomeSpan库，提供HomeKit功能支持
#include <FastLED.h>       // 包含FastLED库，用于控制WS2812B LED

// WS2812B LED配置
#define LED_PIN 2          // WS2812B LED数据输入引脚
#define NUM_LEDS 1         // WS2812B LED数量，当前只使用一个LED

// 创建LED对象
CRGB leds[NUM_LEDS];       // WS2812B LED对象数组

// 全局变量
bool ledState = false;                // LED开关状态，true表示开启，false表示关闭
CRGB currentColor = CRGB::Black;      // 当前LED颜色，初始化为黑色（关闭状态）

/**
 * @brief 初始化WS2812B LED
 * @details 配置FastLED库并初始化WS2812B LED
 */
void initWS2812B() {
    // 初始化WS2812B LED，使用GRB颜色顺序（WS2812B的标准顺序）
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    // 设置最大亮度为255（满亮度）
    FastLED.setBrightness(255);
    // 初始显示（所有LED设为黑色/关闭状态）
    FastLED.show();
    Serial.println("WS2812B initialized");
}

/**
 * @brief 设置WS2812B LED颜色
 * @param color CRGB颜色值
 * @param update 是否立即更新显示，默认为true
 */
void setLEDColor(CRGB color, bool update = true) {
    // 存储当前颜色
    currentColor = color;
    // 应用颜色到LED
    leds[0] = color;
    // 如果update为true，立即更新LED显示
    if (update) {
        FastLED.show();
    }
    // 更新LED状态（如果颜色不是黑色，则LED被视为开启状态）
    ledState = (color != CRGB::Black);
}

/**
 * @brief 设置WS2812B LED状态
 * @param on 开关状态，true表示开启，false表示关闭
 * @param update 是否立即更新显示，默认为true
 */
void setLEDState(bool on, bool update = true) {
    // 根据开关状态设置LED颜色
    if (on) {
        // 如果要开启LED且当前颜色是黑色，设置为白色作为默认颜色
        if (currentColor == CRGB::Black) {
            currentColor = CRGB::White;
        }
        // 应用当前颜色
        leds[0] = currentColor;
    } else {
        // 关闭状态，设置为黑色
        leds[0] = CRGB::Black;
    }
    
    // 如果update为true，立即更新LED显示
    if (update) {
        FastLED.show();
    }
    
    // 更新LED状态
    ledState = on;
}

/**
 * @brief HomeSpan智能灯泡服务类
 * @details 实现了HomeKit智能灯泡服务，支持开关、亮度、色调和饱和度控制
 */
struct DEV_WS2812B : Service::LightBulb {
    // HomeKit特征值
    SpanCharacteristic *power;              // 开关特征
    SpanCharacteristic *brightness;         // 亮度特征
    SpanCharacteristic *hue;                // 色调特征
    SpanCharacteristic *saturation;         // 饱和度特征
    
    /**
     * @brief 构造函数，初始化智能灯泡服务
     */
    DEV_WS2812B() : Service::LightBulb() {
        // 创建开关特征，默认为关闭状态
        power = new Characteristic::On();
        
        // 创建亮度特征，默认100%亮度
        brightness = new Characteristic::Brightness(100);
        // 设置亮度范围为0-100%
        brightness->setRange(0, 100);
        
        // 创建色调特征，默认0度（红色）
        hue = new Characteristic::Hue(0);
        
        // 创建饱和度特征，默认0%饱和度（白色）
        saturation = new Characteristic::Saturation(0);
    }

    /**
     * @brief HomeSpan更新函数，当特征值变化时被调用
     * @return boolean 更新是否成功
     */
    boolean update() {
        // 获取新的开关状态
        boolean isOn = power->getNewVal();
        
        if(isOn) {
            // LED开启状态，获取HSV颜色值
            float h = hue->getNewVal();          // 获取色调值（0-360度）
            float s = saturation->getNewVal();   // 获取饱和度值（0-100%）
            float v = brightness->getNewVal();   // 获取亮度值（0-100%）
            
            // 将HSV转换为RGB
            // 将HomeKit值域转换为FastLED值域：
            // - 色调: 0-360度 -> 0-255
            // - 饱和度: 0-100% -> 0-255
            // - 亮度: 0-100% -> 0-255
            CHSV hsv(h * 255/360, s * 255/100, v * 255/100);
            // 使用rainbow色彩转换算法转换为RGB
            hsv2rgb_rainbow(hsv, currentColor);
        } else {
            // LED关闭状态，设置为黑色
            currentColor = CRGB::Black;
        }
        
        // 更新LED显示
        leds[0] = currentColor;
        FastLED.show();
        
        // 更新全局LED状态
        ledState = isOn;
        
        // 返回更新成功
        return(true);
    }
};
