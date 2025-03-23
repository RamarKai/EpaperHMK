#include "ws2812.h"                      // 包含WS2812B LED控制的头文件

CRGB leds[NUM_LEDS];                     // 定义LED数组，用于存储LED的颜色数据

bool ledState = false;                   // 定义LED状态变量，初始为关闭
CRGB currentColor = CRGB::Black;         // 定义当前颜色变量，初始为黑色(关闭)

void initWS2812B() {                     // 初始化WS2812B LED的函数
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);  // 配置FastLED，指定LED类型、引脚和颜色顺序
    FastLED.setBrightness(255);          // 设置LED亮度为最大值255
    FastLED.show();                      // 显示初始状态（全黑）
    Serial.println("WS2812B initialized");  // 打印初始化成功信息
}

void setLEDColor(CRGB color, bool update) {  // 设置LED颜色的函数
    currentColor = color;                    // 更新当前颜色变量
    leds[0] = color;                         // 设置第一个LED的颜色
    if (update) {                            // 如果需要立即更新
        FastLED.show();                      // 显示更新后的颜色
    }
    ledState = (color != CRGB::Black);       // 如果颜色不是黑色，则LED状态为开启
}

void setLEDState(bool on, bool update) {     // 设置LED开关状态的函数
    if (on) {                                // 如果要开启LED
        if (currentColor == CRGB::Black) {   // 如果当前颜色是黑色(关闭状态)
            currentColor = CRGB::White;      // 默认使用白色
        }
        leds[0] = currentColor;              // 设置LED颜色为当前颜色
    } else {                                 // 如果要关闭LED
        leds[0] = CRGB::Black;               // 设置LED颜色为黑色(关闭)
    }
    
    if (update) {                            // 如果需要立即更新
        FastLED.show();                      // 显示更新后的状态
    }
    
    ledState = on;                           // 更新LED状态变量
}

DEV_WS2812B::DEV_WS2812B() : Service::LightBulb() {  // DEV_WS2812B类的构造函数
    power = new Characteristic::On();                // 创建开关特性
    
    brightness = new Characteristic::Brightness(100);  // 创建亮度特性，初始值为100%
    brightness->setRange(0, 100);                     // 设置亮度范围为0-100%
    
    hue = new Characteristic::Hue(0);                 // 创建色调特性，初始值为0度
    
    saturation = new Characteristic::Saturation(0);   // 创建饱和度特性，初始值为0%
}

boolean DEV_WS2812B::update() {                       // 当HomeKit发送控制命令时的更新函数
    boolean isOn = power->getNewVal();                // 获取新的开关状态
    
    if(isOn) {                                        // 如果要开启LED
        float h = hue->getNewVal();                   // 获取新的色调值(0-360度)
        float s = saturation->getNewVal();            // 获取新的饱和度值(0-100%)
        float v = brightness->getNewVal();            // 获取新的亮度值(0-100%)
        
        CHSV hsv(h * 255/360, s * 255/100, v * 255/100);  // 将HSV值转换为FastLED使用的范围(0-255)
        hsv2rgb_rainbow(hsv, currentColor);           // 将HSV颜色转换为RGB颜色并存储
    } else {                                          // 如果要关闭LED
        currentColor = CRGB::Black;                   // 设置当前颜色为黑色
    }
    
    leds[0] = currentColor;                           // 更新LED颜色
    FastLED.show();                                   // 显示更新后的颜色
    
    ledState = isOn;                                  // 更新LED状态变量
    
    return(true);                                     // 返回true表示更新成功
}