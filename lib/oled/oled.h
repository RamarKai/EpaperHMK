/**
 * @file oled.h
 * @brief OLED显示屏相关功能声明
 * @details 包含OLED显示屏初始化、更新和显示功能的声明
 */

#ifndef OLED_H    // 头文件保护宏定义开始
#define OLED_H    // 定义头文件保护宏

#include <Adafruit_SSD1306.h>  // 包含OLED显示屏驱动库
#include <Wire.h>              // 包含I2C通信库
#include <FastLED.h>           // 包含FastLED库，用于处理RGB颜色类型
#include "weather.h"           // 包含天气库，用于显示天气数据

// OLED显示屏配置参数
#define SCREEN_WIDTH 128     // OLED显示屏宽度，单位像素
#define SCREEN_HEIGHT 64     // OLED显示屏高度，单位像素
#define OLED_RESET -1        // OLED重置引脚，-1表示使用Arduino的重置引脚
#define SCREEN_ADDRESS 0x3C  // OLED显示屏I2C地址，0x3C为常见地址

// OLED接口引脚定义
#define OLED_SDA 21     // OLED的SDA引脚，用于I2C数据传输
#define OLED_SCL 22     // OLED的SCL引脚，用于I2C时钟信号

// 页面切换间隔时间(毫秒)
#define PAGE_SWITCH_INTERVAL 5000  // 页面切换间隔，5秒

// 页面枚举
enum DisplayPage {
    SENSOR_PAGE = 0,    // 传感器数据页面
    WEATHER_PAGE = 1    // 天气数据页面
};

// 外部OLED对象声明
extern Adafruit_SSD1306 display;  // OLED显示屏对象
extern TwoWire I2C_OLED;         // OLED使用的I2C，使用第一个I2C控制器
extern DisplayPage currentPage;   // 当前显示页面

/**
 * @brief 初始化OLED显示屏
 * @details 配置I2C接口并初始化OLED显示屏
 * @return bool 初始化成功返回true，否则返回false
 */
bool initOLED();

/**
 * @brief 更新OLED显示内容
 * @details 根据当前页面更新OLED显示
 */
void updateDisplay();

/**
 * @brief 更新传感器页面
 * @details 在OLED上显示传感器数据和LED状态
 */
void updateSensorPage();

/**
 * @brief 更新天气页面
 * @details 在OLED上显示天气数据
 * @param weather 天气数据结构体
 */
void updateWeatherPage(const WeatherData &weather);

/**
 * @brief 切换显示页面
 * @details 根据当前页面和时间间隔切换显示页面
 * @return 是否进行了页面切换
 */
bool switchDisplayPage();

/**
 * @brief 在OLED上显示启动画面
 * @details 在系统启动时显示的欢迎信息
 */
void showStartupScreen();

/**
 * @brief 在OLED上显示错误信息
 * @param errorMsg 要显示的错误信息
 */
void showError(const char* errorMsg);

#endif  // OLED_H    // 头文件保护宏定义结束 