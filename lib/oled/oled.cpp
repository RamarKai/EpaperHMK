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

// 页面相关变量
DisplayPage currentPage = SENSOR_PAGE;  // 当前显示页面，默认为传感器页面
unsigned long lastPageSwitchTime = 0;   // 上次页面切换时间

// 外部引用的变量声明，这些变量定义在其他模块中
extern float lightLevel;  // 光照强度，定义在lightsensor.cpp
extern bool bh1750_ok;    // BH1750传感器状态，定义在lightsensor.cpp
extern int gasLevel;      // 气体浓度，定义在mq2sensor.cpp
extern bool ledState;     // LED状态，定义在ws2812.cpp或main.cpp
extern CRGB currentColor; // 当前LED颜色，定义在ws2812.cpp或main.cpp
extern WeatherData currentWeather; // 当前天气数据，定义在weather.cpp

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
    
    // 初始化页面切换时间
    lastPageSwitchTime = millis();
    
    return true;  // 初始化成功，返回true
}

/**
 * @brief 更新OLED显示内容
 * @details 根据当前页面更新OLED显示
 */
void updateDisplay() {
    // 切换页面
    switchDisplayPage();
    
    // 根据当前页面选择显示内容
    switch(currentPage) {
        case SENSOR_PAGE:
            updateSensorPage();
            break;
        case WEATHER_PAGE:
            updateWeatherPage(currentWeather);
            break;
    }
}

/**
 * @brief 更新传感器页面
 * @details 在OLED上显示传感器数据和LED状态
 */
void updateSensorPage() {
    // 清空显示缓冲区
    display.clearDisplay();
    // 设置文本大小为1
    display.setTextSize(1);
    // 设置文本颜色为白色
    display.setTextColor(SSD1306_WHITE);
    
    // 显示页面标题
    display.setCursor(0, 0);
    display.println("-- SENSORS DATA --");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);
    
    // 显示光照强度
    display.setCursor(0, 12);  // 设置光照数据显示位置
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
    display.setCursor(0, 24);  // 设置气体数据显示位置
    display.print("Gas: ");
    display.println(gasLevel);  // 显示气体传感器读数
    
    // 显示LED状态
    display.setCursor(0, 36);  // 设置LED状态显示位置
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
    
    // 显示页面指示器
    display.setCursor(0, 56);
    display.print("Page: 1/2");
    
    // 将缓冲区内容发送到OLED显示
    display.display();
}

/**
 * @brief 更新天气页面
 * @details 在OLED上显示天气数据
 * @param weather 天气数据结构体
 */
void updateWeatherPage(const WeatherData &weather) {
    // 清空显示缓冲区
    display.clearDisplay();
    // 设置文本颜色为白色
    display.setTextColor(SSD1306_WHITE);
    
    // 显示页面标题
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("-- WEATHER DATA --");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);
    
    if(weather.is_valid) {
        // 显示城市
        display.setCursor(0, 12);
        display.print("City: ");
        display.println(weather.city);
        
        // 显示天气状况
        display.setCursor(0, 22);
        display.print("Weather: ");
        display.println(weather.weather);
        
        // 显示温度
        display.setCursor(0, 32);
        display.print("Temp: ");
        display.print(weather.temperature);
        display.println(" C");
        
        // 显示湿度
        display.setCursor(0, 42);
        display.print("Humidity: ");
        display.print(weather.humidity);
        display.println("%");
    } else {
        // 如果没有有效的天气数据
        display.setTextSize(1);
        display.setCursor(0, 24);
        display.println("No valid weather data");
        display.setCursor(0, 36);
        display.println("Please check WiFi");
        display.setCursor(0, 46);
        display.println("and API key settings");
    }
    
    // 显示页面指示器
    display.setCursor(0, 56);
    display.print("Page: 2/2");
    
    // 将缓冲区内容发送到OLED显示
    display.display();
}

/**
 * @brief 切换显示页面
 * @details 根据当前页面和时间间隔切换显示页面
 * @return 是否进行了页面切换
 */
bool switchDisplayPage() {
    unsigned long currentTime = millis();
    
    // 检查是否需要切换页面
    if(currentTime - lastPageSwitchTime >= PAGE_SWITCH_INTERVAL) {
        // 切换到下一个页面
        switch(currentPage) {
            case SENSOR_PAGE:
                currentPage = WEATHER_PAGE;
                break;
            case WEATHER_PAGE:
                currentPage = SENSOR_PAGE;
                break;
        }
        
        // 更新页面切换时间
        lastPageSwitchTime = currentTime;
        return true;
    }
    
    return false;
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