#pragma once

#include "HomeSpan.h"
#include <Adafruit_NeoPixel.h>

// WS2812B LED设备类定义
struct DEV_WS2812B : Service::LightBulb {
    // 定义成员变量
    const int numPixels = 1;                    // LED数量
    Adafruit_NeoPixel *pixels;                 // NeoPixel对象指针
    SpanCharacteristic *power;                 // 开关特征
    SpanCharacteristic *H;                     // 色调特征
    SpanCharacteristic *S;                     // 饱和度特征
    SpanCharacteristic *V;                     // 亮度特征

    // 构造函数
    DEV_WS2812B(int pin) : Service::LightBulb() {
        // 初始化WS2812B
        pixels = new Adafruit_NeoPixel(numPixels, pin, NEO_GRB + NEO_KHZ800);
        pixels->begin();
        pixels->clear();
        pixels->show();

        // 初始化HomeKit特征
        power = new Characteristic::On();                    
        H = new Characteristic::Hue(0);                     // 色调初始值0度
        S = new Characteristic::Saturation(0);              // 饱和度初始值0%
        V = new Characteristic::Brightness(100);            // 亮度初始值100%
        V->setRange(5, 100, 1);                            // 设置亮度范围5-100%,步进1%

        // 输出初始化信息
        Serial.printf("配置WS2812B LED: Pin=%d\n", pin);
    }

    // 更新函数
    boolean update() {
        boolean p;
        float h, s, v, r, g, b;

        // 获取当前值
        h = H->getVal<float>();
        s = S->getVal<float>();
        v = V->getVal<float>();
        p = power->getVal();

        // 输出调试信息
        LOG1("更新WS2812B LED: ");

        // 检查各个特征的更新状态
        if(power->updated()) {
            p = power->getNewVal();
            LOG1("Power="); LOG1(p?"true":"false"); LOG1(" ");
        }
        
        if(H->updated()) {
            h = H->getNewVal<float>();
            LOG1("H="); LOG1(h); LOG1(" ");
        }
        
        if(S->updated()) {
            s = S->getNewVal<float>();
            LOG1("S="); LOG1(s); LOG1(" ");
        }
        
        if(V->updated()) {
            v = V->getNewVal<float>();
            LOG1("V="); LOG1(v); LOG1(" ");
        }

        // HSV转RGB
        LedPin::HSVtoRGB(h, s/100.0, v/100.0, &r, &g, &b);

        // 计算最终RGB值
        uint8_t R = p * r * 255;  // 转换为0-255范围
        uint8_t G = p * g * 255;
        uint8_t B = p * b * 255;

        LOG1("RGB=("); LOG1(R); LOG1(","); LOG1(G); LOG1(","); LOG1(B); LOG1(")\n");

        // 更新LED颜色
        pixels->setPixelColor(0, pixels->Color(R, G, B));
        pixels->show();

        return true;
    }
}; 