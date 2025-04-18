#ifndef EPAPER_H // 防止头文件被重复包含的宏定义开始
#define EPAPER_H // 定义宏以标记该头文件已被包含

#include <GxEPD2_BW.h>               // 包含GxEPD2黑白显示屏库
#include <GxEPD2_3C.h>               // 包含GxEPD2三色显示屏库
#include <U8g2_for_Adafruit_GFX.h>   // 包含U8g2字体支持
#include <Fonts/FreeMonoBold9pt7b.h> // 包含免费Mono字体
#include <Fonts/FreeSans9pt7b.h>     // 包含免费Sans字体
#include <Fonts/FreeSansBold9pt7b.h> // 包含免费SansBold字体
#include "weather.h"                 // 包含天气数据结构
#include "dht11.h"                   // 包含DHT11传感器数据
#include "time_manager.h"            // 包含时间管理模块
#include "lightsensor.h"             // 包含光线传感器模块
#include "mq2sensor.h"               // 包含气体传感器模块

// E029A01-FPC-A1 墨水屏参数（实际上是一个2.9寸黑白屏，基于SSD1680驱动IC）
#define EPAPER_WIDTH 128  // 墨水屏宽度为128像素
#define EPAPER_HEIGHT 296 // 墨水屏高度为296像素

// 墨水屏SPI引脚定义
#define EPAPER_BUSY 25 // 忙信号引脚为GPIO25
#define EPAPER_RST 26  // 复位引脚为GPIO26
#define EPAPER_DC 32   // 数据/命令选择引脚为GPIO32
#define EPAPER_CS 33   // 片选引脚为GPIO33
#define EPAPER_SCK 18  // 时钟引脚为GPIO18
#define EPAPER_MOSI 23 // 主机输出从机输入引脚为GPIO23
#define EPAPER_MISO -1 // 主机输入从机输出引脚不使用

#define EPAPER_UPDATE_INTERVAL 60000 // 墨水屏更新间隔，单位毫秒（1分钟）

// 定义常量
#define MAX_DISPLAY_BUFFER_SIZE 4096 // 最大显示缓冲区大小
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

// 界面元素常量
#define TOP_MARGIN 5    // 顶部边距
#define LEFT_MARGIN 5   // 左侧边距
#define LINE_HEIGHT 20  // 行高
#define VALUE_OFFSET 80 // 值的起始偏移量

// 枚举页面类型
enum EpaperPage
{
    MAIN_PAGE = 0,           // 主页面
    EPAPER_WEATHER_PAGE = 1, // 天气页面
    SENSOR_DETAILS_PAGE = 2  // 传感器详情页面
};

// 声明外部变量
extern GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display_epaper; // 墨水屏显示对象
extern U8G2_FOR_ADAFRUIT_GFX u8g2_for_epaper;                            // U8g2字体渲染对象
extern EpaperPage currentEpaperPage;                                     // 当前墨水屏页面
extern unsigned long lastEpaperUpdate;                                   // 上次墨水屏更新时间

// 时间和日期格式化函数
void getTimeString(char *buffer); // 获取时间字符串
void getDateString(char *buffer); // 获取日期字符串

// 函数声明
bool initEPaper();                                                                      // 初始化墨水屏
void updateEPaper();                                                                    // 更新墨水屏显示内容
void drawMainPage();                                                                    // 绘制主页面
void drawWeatherPage(const WeatherData &weather);                                       // 绘制天气页面
void drawSensorDetailsPage();                                                           // 绘制传感器详情页面
void switchEpaperPage();                                                                // 切换墨水屏页面
void displayBatteryStatus(int x, int y, int percentage);                                // 显示电池状态
void drawValueWithUnit(int x, int y, float value, const char *unit, const char *label); // 绘制带单位的值
void drawTitle(const char *title);                                                      // 绘制标题

#endif // 防止头文件被重复包含的宏定义结束
