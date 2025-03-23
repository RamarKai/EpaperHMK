/**
 * @file oled.cpp
 * @brief OLED显示屏相关功能实现
 * @details 包含OLED显示屏初始化、更新和显示功能
 */

#include "oled.h"    // 包含对应的头文件

// 创建OLED专用I2C实例
TwoWire I2C_OLED = TwoWire(0);  // OLED使用的I2C，使用第一个I2C控制器

// 创建OLED显示对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET);  // OLED显示屏对象

// 外部引用的变量声明，这些变量定义在其他模块中
extern float lightLevel;  // 光照强度，定义在lightsensor.cpp
extern bool bh1750_ok;    // BH1750传感器状态，定义在lightsensor.cpp
extern int gasLevel;      // 气体浓度，定义在mq2sensor.cpp
extern bool ledState;     // LED状态，定义在ws2812.cpp或main.cpp
extern CRGB currentColor; // 当前LED颜色，定义在ws2812.cpp或main.cpp

/**
 * @brief 初始化OLED显示屏
 * @details 配置I2C接口并初始化OLED显示屏
 * @return bool 初始化成功返回true，否则返回false
 */
bool initOLED() {
    // 初始化OLED使用的I2C总线
    I2C_OLED.begin(OLED_SDA, OLED_SCL);
    
    // 初始化OLED显示屏
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;  // 初始化失败，返回false
    }
    
    // 清空显示缓冲区并更新显示
    display.clearDisplay();
    display.display();
    Serial.println("OLED initialized");
    return true;  // 初始化成功，返回true
}

/**
 * @brief 更新OLED显示内容
 * @details 在OLED上显示传感器数据和LED状态
 */
void updateDisplay() {
    // 清空显示缓冲区
    display.clearDisplay();
    // 设置文本大小为1
    display.setTextSize(1);
    // 设置文本颜色为白色
    display.setTextColor(SSD1306_WHITE);
    
    // 显示光照强度
    display.setCursor(0, 0);  // 设置光照数据显示位置
    display.print("Light: ");
    if(bh1750_ok) {
        // 如果BH1750传感器正常工作，显示光照强度值
        display.print(lightLevel);
        display.println(" lx");  // 显示单位
    } else {
        // 传感器异常，显示错误信息
        display.println("Error");
    }
    
    // 显示气体浓度
    display.setCursor(0, 16);  // 设置气体数据显示位置
    display.print("Gas: ");
    display.println(gasLevel);  // 显示气体传感器读数
    
    // 显示LED状态
    display.setCursor(0, 32);  // 设置LED状态显示位置
    display.print("LED: ");
    if(ledState) {
        // 如果LED开启，显示其RGB颜色值
        display.print("ON (RGB:");
        display.print(currentColor.r);  // 显示红色分量
        display.print(",");
        display.print(currentColor.g);  // 显示绿色分量
        display.print(",");
        display.print(currentColor.b);  // 显示蓝色分量
        display.println(")");
    } else {
        // LED关闭，显示关闭状态
        display.println("OFF");
    }
    
    // 将缓冲区内容发送到OLED显示
    display.display();
}

/**
 * @brief 在OLED上显示启动画面
 * @details 在系统启动时显示的欢迎信息
 */
void showStartupScreen() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.println("ESP32");
    display.setCursor(10, 25);
    display.println("HomeKit");
    display.setTextSize(1);
    display.setCursor(10, 45);
    display.println("Initializing...");
    display.display();
    delay(2000);  // 显示启动画面2秒
}

/**
 * @brief 在OLED上显示错误信息
 * @param errorMsg 要显示的错误信息
 */
void showError(const char* errorMsg) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("ERROR:");
    display.setCursor(0, 16);
    display.println(errorMsg);
    display.display();
} 