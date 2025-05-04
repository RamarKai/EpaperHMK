#ifndef EPAPER_H // 防止头文件被重复包含的宏定义开始
#define EPAPER_H // 定义宏以标记该头文件已被包含

#include <GxEPD2_BW.h>                // 包含GxEPD2黑白墨水屏库
#include <Fonts/FreeMonoBold9pt7b.h>  // 包含字体库
#include <Fonts/FreeSans9pt7b.h>      // 包含字体库
#include <Fonts/FreeSansBold12pt7b.h> // 包含字体库
#include <Fonts/FreeSansBold24pt7b.h> // 包含大字体库
#include <Fonts/FreeSerif9pt7b.h>     // 包含衬线字体库
#include <Fonts/FreeMonoBold12pt7b.h> // 包含等宽粗体字体库
#include <U8g2_for_Adafruit_GFX.h>    // 包含U8g2字体支持库
#include "weather.h"                  // 包含天气相关的头文件
#include "dht11.h"                    // 包含DHT11传感器相关头文件
#include "mq2sensor.h"                // 包含MQ2传感器相关头文件
#include "lightsensor.h"              // 包含光线传感器相关头文件
#include "time_manager.h"             // 包含时间管理相关头文件
#include "ws2812.h"                   // 包含LED灯相关头文件

// 墨水屏引脚定义
#define EPAPER_BUSY 25 // 墨水屏忙状态引脚
#define EPAPER_RST 26  // 墨水屏复位引脚
#define EPAPER_DC 32   // 墨水屏数据/命令控制引脚
#define EPAPER_CS 33   // 墨水屏片选引脚
#define EPAPER_SCK 18  // 墨水屏SPI时钟引脚
#define EPAPER_MOSI 23 // 墨水屏SPI数据输出引脚

// 自定义显示短语
extern String customPhrase; // 墨水屏底部显示的自定义短语

// 墨水屏相关函数声明
bool initEPaper();                          // 初始化墨水屏
void updateEPaper();                        // 更新墨水屏显示内容
void refreshDisplay();                      // 刷新显示内容
void updateTimeDisplay();                   // 更新时间显示
void deepSleepEPaper();                     // 使墨水屏进入深度睡眠模式
void wakeupEPaper();                        // 唤醒墨水屏
void showEPaperStartupScreen();             // 显示墨水屏启动画面
void showEPaperError(const char *errorMsg); // 在墨水屏上显示错误信息
void setCustomPhrase(const char *phrase);   // 设置自定义短语

// 外部变量声明
extern GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display_epaper; // 声明外部墨水屏显示对象
extern U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;                          // 声明外部U8g2字体对象
extern bool epaper_initialized;                                  // 声明外部墨水屏初始化状态

#endif // 防止头文件被重复包含的宏定义结束
