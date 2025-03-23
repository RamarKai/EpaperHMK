/**
 * @file ws2812.h
 * @brief WS2812B LED控制功能声明
 * @details 包含WS2812B LED的初始化、控制和HomeSpan智能灯泡服务声明
 */

#ifndef WS2812_H    // 头文件保护宏定义开始
#define WS2812_H    // 定义头文件保护宏

#include "HomeSpan.h"      // 包含HomeSpan库，提供HomeKit功能支持
#include <FastLED.h>       // 包含FastLED库，用于控制WS2812B LED

// WS2812B LED配置
#define LED_PIN 2          // WS2812B LED数据输入引脚
#define NUM_LEDS 1         // WS2812B LED数量，当前只使用一个LED

// 外部LED对象声明
extern CRGB leds[NUM_LEDS];       // WS2812B LED对象数组

// 全局变量声明
extern bool ledState;             // LED开关状态，true表示开启，false表示关闭
extern CRGB currentColor;         // 当前LED颜色

/**
 * @brief 初始化WS2812B LED
 * @details 配置FastLED库并初始化WS2812B LED
 */
void initWS2812B();

/**
 * @brief 设置WS2812B LED颜色
 * @param color CRGB颜色值
 * @param update 是否立即更新显示，默认为true
 */
void setLEDColor(CRGB color, bool update = true);

/**
 * @brief 设置WS2812B LED状态
 * @param on 开关状态，true表示开启，false表示关闭
 * @param update 是否立即更新显示，默认为true
 */
void setLEDState(bool on, bool update = true);

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
    DEV_WS2812B();

    /**
     * @brief HomeSpan更新函数，当特征值变化时被调用
     * @return boolean 更新是否成功
     */
    boolean update();
};

#endif  // WS2812_H    // 头文件保护宏定义结束 