#ifndef OLED_H                    // 防止头文件被重复包含的宏定义开始
#define OLED_H                    // 定义宏以标记该头文件已被包含

#include <Adafruit_SSD1306.h>     // 包含SSD1306 OLED显示器库
#include <Wire.h>                 // 包含Wire库，用于I2C通信
#include <FastLED.h>              // 包含FastLED库，用于LED控制
#include "weather.h"              // 包含天气相关的头文件

#define SCREEN_WIDTH 128          // 定义OLED显示屏宽度为128像素
#define SCREEN_HEIGHT 64          // 定义OLED显示屏高度为64像素
#define OLED_RESET -1             // 定义复位引脚为-1，表示使用Arduino的复位引脚
#define SCREEN_ADDRESS 0x3C       // 定义OLED显示屏的I2C地址为0x3C

#define OLED_SDA 21               // 定义OLED的SDA引脚为GPIO21
#define OLED_SCL 22               // 定义OLED的SCL引脚为GPIO22

#define PAGE_SWITCH_INTERVAL 5000 // 定义页面自动切换的间隔时间为5000毫秒

enum DisplayPage {                // 定义显示页面的枚举类型
    SENSOR_PAGE = 0,              // 传感器数据页面，值为0
    WEATHER_PAGE = 1              // 天气数据页面，值为1
};

extern Adafruit_SSD1306 display;  // 声明外部显示器对象
extern TwoWire I2C_OLED;          // 声明外部I2C通信对象
extern DisplayPage currentPage;   // 声明外部当前显示页面变量

bool initOLED();                  // 声明初始化OLED显示屏的函数

void updateDisplay();             // 声明更新显示内容的函数

void updateSensorPage();          // 声明更新传感器页面的函数

void updateWeatherPage(const WeatherData &weather);  // 声明更新天气页面的函数，接收天气数据参数

bool switchDisplayPage();         // 声明切换显示页面的函数

void showStartupScreen();         // 声明显示启动画面的函数

void showError(const char* errorMsg);  // 声明显示错误信息的函数，接收错误消息字符串参数

#endif                            // 防止头文件被重复包含的宏定义结束