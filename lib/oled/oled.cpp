#include "oled.h"  // 包含OLED显示器的头文件
#include "dht11.h" // 包含DHT11传感器的头文件

TwoWire I2C_OLED = TwoWire(0); // 创建TwoWire对象，指定使用ESP32的第一个I2C控制器

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET); // 创建SSD1306显示器对象

DisplayPage currentPage = SENSOR_PAGE; // 初始化当前显示页面为传感器页面
unsigned long lastPageSwitchTime = 0;  // 初始化上次页面切换时间为0

extern float lightLevel;           // 声明外部光照强度变量
extern bool bh1750_ok;             // 声明外部光线传感器状态变量
extern int gasLevel;               // 声明外部气体浓度变量
extern bool ledState;              // 声明外部LED状态变量
extern CRGB currentColor;          // 声明外部当前LED颜色变量
extern WeatherData currentWeather; // 声明外部当前天气数据变量

bool initOLED()
{                                       // 初始化OLED显示屏的函数
    I2C_OLED.begin(OLED_SDA, OLED_SCL); // 初始化I2C总线，设置SDA和SCL引脚

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {                                                   // 尝试初始化显示器
        Serial.println(F("SSD1306 allocation failed")); // 如果初始化失败则打印错误信息
        return false;                                   // 返回false表示初始化失败
    }

    display.clearDisplay();             // 清除显示内容
    display.display();                  // 更新显示，使清除生效
    Serial.println("OLED initialized"); // 打印初始化成功信息

    lastPageSwitchTime = millis(); // 记录初始化时间为当前时间

    return true; // 返回true表示初始化成功
}

void updateDisplay()
{                        // 更新显示内容的函数
    switchDisplayPage(); // 尝试切换显示页面

    switch (currentPage)
    {                       // 根据当前页面类型来更新显示内容
    case SENSOR_PAGE:       // 如果当前是传感器页面
        updateSensorPage(); // 更新传感器数据页面
        break;
    case WEATHER_PAGE:                     // 如果当前是天气页面
        updateWeatherPage(currentWeather); // 更新天气数据页面
        break;
    }
}

void updateSensorPage()
{                                        // 更新传感器页面的函数
    display.clearDisplay();              // 清除显示内容
    display.setTextSize(1);              // 设置文本大小为1
    display.setTextColor(SSD1306_WHITE); // 设置文本颜色为白色

    display.setCursor(0, 0);                       // 设置光标位置到左上角
    display.println("-- SENSORS DATA --");         // 显示传感器数据标题
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE); // 绘制横线分隔标题区域

    // 显示温度和湿度数据
    display.setCursor(0, 12); // 设置光标位置到第二行
    display.print("Temp: ");  // 显示"Temp:"文本
    if (dht11_ok)
    {                               // 如果DHT11传感器正常
        display.print(temperature); // 显示温度值
        display.println(" C");      // 显示单位"C"
    }
    else
    {                             // 如果DHT11传感器异常
        display.println("Error"); // 显示"Error"
    }

    display.setCursor(0, 22);    // 设置光标位置到第三行
    display.print("Humidity: "); // 显示"Humidity:"文本
    if (dht11_ok)
    {                            // 如果DHT11传感器正常
        display.print(humidity); // 显示湿度值
        display.println("%");    // 显示单位"%"
    }
    else
    {                             // 如果DHT11传感器异常
        display.println("Error"); // 显示"Error"
    }

    display.setCursor(0, 32); // 设置光标位置到第四行
    display.print("Light: "); // 显示"Light:"文本
    if (bh1750_ok)
    {                              // 如果光线传感器正常
        display.print(lightLevel); // 显示光照强度值
        display.println(" lx");    // 显示单位"lx"
    }
    else
    {                             // 如果光线传感器异常
        display.println("Error"); // 显示"Error"
    }

    display.setCursor(0, 42);  // 设置光标位置到第五行
    display.print("Gas: ");    // 显示"Gas:"文本
    display.println(gasLevel); // 显示气体浓度值

    display.setCursor(0, 52);   // 设置光标位置到显示屏底部
    display.print("Page: 1/2"); // 显示页面指示器"Page: 1/2"

    display.display(); // 更新显示内容
}

void updateWeatherPage(const WeatherData &weather)
{                                        // 更新天气页面的函数，接收天气数据参数
    display.clearDisplay();              // 清除显示内容
    display.setTextColor(SSD1306_WHITE); // 设置文本颜色为白色

    display.setTextSize(1);                        // 设置文本大小为1
    display.setCursor(0, 0);                       // 设置光标位置到左上角
    display.println("-- WEATHER DATA --");         // 显示天气数据标题
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE); // 绘制横线分隔标题区域

    if (weather.is_valid)
    {                                  // 如果天气数据有效
        display.setCursor(0, 12);      // 设置光标位置到第二行
        display.print("City: ");       // 显示"City:"文本
        display.println(weather.city); // 显示城市名称

        display.setCursor(0, 22);         // 设置光标位置到第三行
        display.print("Weather: ");       // 显示"Weather:"文本
        display.println(weather.weather); // 显示天气情况

        display.setCursor(0, 32);           // 设置光标位置到第四行
        display.print("Temp: ");            // 显示"Temp:"文本
        display.print(weather.temperature); // 显示温度值
        display.println(" C");              // 显示单位"C"

        display.setCursor(0, 42);        // 设置光标位置到第五行
        display.print("Humidity: ");     // 显示"Humidity:"文本
        display.print(weather.humidity); // 显示湿度值
        display.println("%");            // 显示单位"%"
    }
    else
    {                                             // 如果天气数据无效
        display.setTextSize(1);                   // 设置文本大小为1
        display.setCursor(0, 24);                 // 设置光标位置到第二行
        display.println("No valid weather data"); // 显示无有效天气数据提示
        display.setCursor(0, 36);                 // 设置光标位置到第三行
        display.println("Please check WiFi");     // 显示请检查WiFi连接提示
        display.setCursor(0, 46);                 // 设置光标位置到第四行
        display.println("and API key settings");  // 显示请检查API密钥设置提示
    }

    display.setCursor(0, 56);   // 设置光标位置到显示屏底部
    display.print("Page: 2/2"); // 显示页面指示器"Page: 2/2"

    display.display(); // 更新显示内容
}

bool switchDisplayPage()
{                                         // 切换显示页面的函数
    unsigned long currentTime = millis(); // 获取当前时间

    if (currentTime - lastPageSwitchTime >= PAGE_SWITCH_INTERVAL)
    { // 如果距离上次切换已过指定时间间隔
        switch (currentPage)
        {                               // 根据当前页面切换到下一个页面
        case SENSOR_PAGE:               // 如果当前是传感器页面
            currentPage = WEATHER_PAGE; // 切换到天气页面
            break;
        case WEATHER_PAGE:             // 如果当前是天气页面
            currentPage = SENSOR_PAGE; // 切换到传感器页面
            break;
        }

        lastPageSwitchTime = currentTime; // 更新上次页面切换时间
        return true;                      // 返回true表示已切换页面
    }

    return false; // 返回false表示未切换页面
}

void showStartupScreen()
{                                        // 显示启动画面的函数
    display.clearDisplay();              // 清除显示内容
    display.setTextSize(2);              // 设置文本大小为2
    display.setTextColor(SSD1306_WHITE); // 设置文本颜色为白色
    display.setCursor(10, 5);            // 设置光标位置到上部区域
    display.println("ESP32");            // 显示"ESP32"文本
    display.setCursor(10, 25);           // 设置光标位置到中部区域
    display.println("HomeKit");          // 显示"HomeKit"文本
    display.setTextSize(1);              // 设置文本大小为1
    display.setCursor(10, 45);           // 设置光标位置到下部区域
    display.println("Initializing...");  // 显示"Initializing..."文本
    display.display();                   // 更新显示内容
    delay(2000);                         // 延迟2000毫秒(2秒)显示启动画面
}

void showError(const char *errorMsg)
{                                        // 显示错误信息的函数
    display.clearDisplay();              // 清除显示内容
    display.setTextSize(1);              // 设置文本大小为1
    display.setTextColor(SSD1306_WHITE); // 设置文本颜色为白色
    display.setCursor(0, 0);             // 设置光标位置到左上角
    display.println("ERROR:");           // 显示"ERROR:"文本
    display.setCursor(0, 16);            // 设置光标位置到第二行
    display.println(errorMsg);           // 显示传入的错误信息
    display.display();                   // 更新显示内容
}