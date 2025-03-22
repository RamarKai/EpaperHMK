#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

// 包含必要的库文件
#include <Adafruit_GFX.h>    // Adafruit图形库
#include <Adafruit_SSD1306.h> // SSD1306显示屏驱动库
#include <Wire.h>            // I2C通信库

class DisplayManager {
public:
    // 构造函数：设置显示屏的基本参数
    // w: 屏幕宽度(默认128像素)
    // h: 屏幕高度(默认64像素)
    // reset_pin: 复位引脚(默认-1，使用软件复位)
    DisplayManager(uint8_t w = 128, uint8_t h = 64, int8_t reset_pin = -1);
    
    // 初始化显示屏
    // i2c_address: I2C设备地址(默认0x3C)
    // 返回值：初始化成功返回true，失败返回false
    bool begin(uint8_t i2c_address = 0x3C);
    
    // 基本显示控制函数
    void clear();          // 清空显示缓冲区
    void refresh();        // 将缓冲区内容刷新到屏幕
    void invertDisplay(bool invert); // 反转显示颜色
    
    // 文本显示相关函数
    void setTextSize(uint8_t size);     // 设置文本大小
    void setTextColor(uint16_t color);  // 设置文本颜色
    void setCursor(int16_t x, int16_t y); // 设置光标位置
    void print(const String &text);     // 打印文本
    void println(const String &text);   // 打印文本并换行
    
    // 图形绘制函数
    void drawPixel(int16_t x, int16_t y, uint16_t color);  // 绘制单个像素
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);  // 绘制直线
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);      // 绘制矩形
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);      // 绘制填充矩形
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);             // 绘制圆形
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);             // 绘制填充圆形
    
    // 滚动效果函数
    void startScrollRight(uint8_t start, uint8_t stop);  // 向右滚动
    void startScrollLeft(uint8_t start, uint8_t stop);   // 向左滚动
    void stopScroll();                                   // 停止滚动
    
    // 位图显示函数
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                   int16_t w, int16_t h, uint16_t color); // 显示位图

private:
    Adafruit_SSD1306* display;  // SSD1306显示屏对象指针
    uint8_t width;              // 显示屏宽度
    uint8_t height;             // 显示屏高度
    int8_t reset_pin;           // 复位引脚编号
};

#endif 