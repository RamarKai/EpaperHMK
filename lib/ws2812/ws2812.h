#ifndef WS2812_H                // 防止头文件被重复包含的宏定义开始
#define WS2812_H                // 定义宏以标记该头文件已被包含

#include "HomeSpan.h"           // 包含HomeSpan库，用于HomeKit集成
#include <FastLED.h>            // 包含FastLED库，用于控制WS2812B LED

#define LED_PIN 2               // 定义WS2812B LED的数据引脚为GPIO2
#define NUM_LEDS 1              // 定义LED数量为1个

extern CRGB leds[NUM_LEDS];     // 声明外部LED数组，用于存储每个LED的颜色

extern bool ledState;           // 声明外部变量ledState，表示LED开关状态
extern CRGB currentColor;       // 声明外部变量currentColor，存储当前LED的颜色

// 前向声明
struct DEV_WS2812B;

// LED设备实例，用于在其他模块中访问和更新HomeKit状态
extern DEV_WS2812B* ws2812Device;

void initWS2812B();             // 声明初始化WS2812B LED的函数

void setLEDColor(CRGB color, bool update = true);  // 声明设置LED颜色的函数，带有可选的立即更新参数

void setLEDState(bool on, bool update = true);     // 声明设置LED开关状态的函数，带有可选的立即更新参数

// 声明与HomeKit同步LED状态的函数
void syncLEDWithHomeKit(bool on);

// 声明增加LED亮度的函数
void increaseLEDBrightness(float percentage = 20.0);

// 声明减少LED亮度的函数
void decreaseLEDBrightness(float percentage = 20.0);

// 声明与HomeKit同步LED亮度的函数
void syncLEDBrightnessWithHomeKit(float brightness);

struct DEV_WS2812B : Service::LightBulb {  // 定义WS2812B服务结构体，继承自HomeSpan的LightBulb服务
    SpanCharacteristic *power;             // 指向灯泡电源特性的指针
    SpanCharacteristic *brightness;        // 指向亮度特性的指针
    SpanCharacteristic *hue;               // 指向色调特性的指针
    SpanCharacteristic *saturation;        // 指向饱和度特性的指针
    
    DEV_WS2812B();                         // 构造函数声明

    boolean update();                      // 更新函数声明，当HomeKit发送控制命令时被调用
};

#endif                          // 防止头文件被重复包含的宏定义结束