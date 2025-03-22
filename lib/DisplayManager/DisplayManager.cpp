#include "DisplayManager.h"

// 构造函数：初始化显示屏参数
DisplayManager::DisplayManager(uint8_t w, uint8_t h, int8_t reset_pin) 
    : width(w), height(h), reset_pin(reset_pin) {
    display = new Adafruit_SSD1306(width, height, &Wire, reset_pin);
}

// 初始化显示屏
bool DisplayManager::begin(uint8_t i2c_address) {
    if(!display->begin(SSD1306_SWITCHCAPVCC, i2c_address)) {
        return false;
    }
    display->clearDisplay();  // 清空显示
    display->display();       // 刷新显示
    return true;
}

// 清空显示缓冲区
void DisplayManager::clear() {
    display->clearDisplay();
}

// 刷新显示内容到屏幕
void DisplayManager::refresh() {
    display->display();
}

// 反转显示颜色
void DisplayManager::invertDisplay(bool invert) {
    display->invertDisplay(invert);
}

// 设置文本大小
void DisplayManager::setTextSize(uint8_t size) {
    display->setTextSize(size);
}

// 设置文本颜色
void DisplayManager::setTextColor(uint16_t color) {
    display->setTextColor(color);
}

// 设置光标位置
void DisplayManager::setCursor(int16_t x, int16_t y) {
    display->setCursor(x, y);
}

// 打印文本
void DisplayManager::print(const String &text) {
    display->print(text);
}

// 打印文本并换行
void DisplayManager::println(const String &text) {
    display->println(text);
}

// 绘制单个像素
void DisplayManager::drawPixel(int16_t x, int16_t y, uint16_t color) {
    display->drawPixel(x, y, color);
}

// 绘制直线
void DisplayManager::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    display->drawLine(x0, y0, x1, y1, color);
}

// 绘制矩形边框
void DisplayManager::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    display->drawRect(x, y, w, h, color);
}

// 绘制填充矩形
void DisplayManager::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    display->fillRect(x, y, w, h, color);
}

// 绘制圆形边框
void DisplayManager::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    display->drawCircle(x0, y0, r, color);
}

// 绘制填充圆形
void DisplayManager::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    display->fillCircle(x0, y0, r, color);
}

// 开始向右滚动
void DisplayManager::startScrollRight(uint8_t start, uint8_t stop) {
    display->startscrollright(start, stop);
}

// 开始向左滚动
void DisplayManager::startScrollLeft(uint8_t start, uint8_t stop) {
    display->startscrollleft(start, stop);
}

// 停止滚动
void DisplayManager::stopScroll() {
    display->stopscroll();
}

// 显示位图
void DisplayManager::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                              int16_t w, int16_t h, uint16_t color) {
    display->drawBitmap(x, y, bitmap, w, h, color);
} 