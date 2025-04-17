#include "HomeSpan.h"     // 包含HomeSpan库，用于HomeKit集成
#include "lightsensor.h"  // 包含光线传感器模块
#include "mq2sensor.h"    // 包含MQ2气体传感器模块
#include "oled.h"         // 包含OLED显示屏模块
#include "ws2812.h"       // 包含WS2812B LED控制模块
#include "weather.h"      // 包含天气服务模块
#include "command.h"      // 包含命令处理模块
#include "time_manager.h" // 包含时间管理模块
#include "dht11.h"        // 包含DHT11传感器模块
#include "epaper.h"       // 包含墨水屏模块
#include "key.h"          // 包含按键模块

unsigned long lastDisplayUpdate = 0;             // 记录上次显示更新的时间
const unsigned long displayUpdateInterval = 100; // 设置显示更新的时间间隔为100毫秒

unsigned long lastWeatherUpdate = 0;                // 记录上次天气数据更新的时间
const unsigned long weatherUpdateInterval = 300000; // 设置天气数据更新的时间间隔为300000毫秒(5分钟)

bool weatherServiceInitialized = false; // 标记天气服务是否已初始化
bool weatherInitialUpdateDone = false;  // 标记是否已完成天气数据的首次更新
bool wifiPreviouslyConnected = false;   // 记录上一次WiFi的连接状态
bool timeManagerInitialized = false;    // 标记时间管理模块是否已初始化（改名以避免与time_manager.cpp中的变量冲突）
bool epaperInitialized = false;         // 标记墨水屏是否初始化成功

void setup()
{                                        // Arduino程序的初始化函数
    Serial.begin(115200);                // 初始化串口通信，波特率设为115200
    Serial.println("Starting setup..."); // 通过串口打印设置开始的消息

    if (!initOLED())
    {                                                  // 初始化OLED显示屏，并检查是否成功
        Serial.println("OLED initialization failed!"); // 如果初始化失败，打印错误信息
        for (;;)
            ; // 无限循环，程序停止执行
    }

    showStartupScreen(); // 显示启动画面

    initBH1750(); // 初始化BH1750光线传感器

    initMQ2Sensor(); // 初始化MQ2气体传感器

    initWS2812B(); // 初始化WS2812B LED

    // 初始化按钮
    initButtons(); // 初始化按钮模块

    // 初始化DHT11传感器并保存结果
    bool dht_init_success = initDHT11();
    if (!dht_init_success)
    {
        Serial.println("DHT11 initialization failed!"); // 如果初始化失败，打印错误信息
        showError("DHT11 init failed");                 // 在显示屏上显示错误信息
        delay(2000);                                    // 延迟2秒，让用户能看到错误信息
    }

    // 初始化墨水屏
    epaperInitialized = initEPaper();
    if (!epaperInitialized)
    {
        Serial.println("E-Paper initialization failed!"); // 如果初始化失败，打印错误信息
        showError("E-Paper init failed");                 // 在OLED显示屏上显示错误信息
        delay(2000);                                      // 延迟2秒，让用户能看到错误信息
    }
    else
    {
        Serial.println("E-Paper initialized successfully!"); // 打印初始化成功信息
    }

    // 初始化串口2用于命令通信
    initSerial2(); // 初始化串口2

    homeSpan.begin(Category::Lighting, "ESP32 Sensors"); // 初始化HomeSpan，设置设备类别和名称

    new SpanAccessory();                         // 创建HomeKit配件
    new Service::AccessoryInformation();         // 创建配件信息服务
    new Characteristic::Identify();              // 添加识别特性
    new Characteristic::Name("ESP32 Sensors");   // 设置设备名称
    new Characteristic::Manufacturer("DIY");     // 设置制造商名称
    new Characteristic::SerialNumber("123-ABC"); // 设置序列号
    new Characteristic::Model("ESP32");          // 设置型号
    new Characteristic::FirmwareRevision("1.0"); // 设置固件版本

    new DEV_WS2812B();          // 添加WS2812B LED服务
    new DEV_LightSensor();      // 添加光线传感器服务
    new DEV_AirQualitySensor(); // 添加空气质量传感器服务

    // 只有在DHT11传感器初始化成功时才添加温度和湿度传感器服务
    if (dht_init_success)
    {
        new DEV_TemperatureSensor(); // 添加温度传感器服务
        new DEV_HumiditySensor();    // 添加湿度传感器服务
    }

    display.clearDisplay();                  // 清除显示屏内容
    display.setTextSize(1);                  // 设置文本大小为1
    display.setTextColor(SSD1306_WHITE);     // 设置文本颜色为白色
    display.setCursor(0, 0);                 // 设置光标位置到左上角
    display.println("HomeSpan Starting..."); // 显示HomeSpan启动中的信息
    display.setCursor(0, 16);                // 设置光标位置到第二行
    display.println("Waiting for WiFi...");  // 显示等待WiFi连接的信息
    display.display();                       // 更新显示内容

    lastDisplayUpdate = millis(); // 记录当前时间为上次显示更新时间
    lastWeatherUpdate = millis(); // 记录当前时间为上次天气更新时间
}

void loop()
{                    // Arduino程序的主循环函数
    homeSpan.poll(); // 处理HomeSpan相关的事件和请求

    // 处理串口2接收到的命令
    handleSerial2Commands(); // 检查并处理串口2接收到的命令

    // 更新按钮状态
    updateButtonStates(); // 更新并检测按钮状态

    // 检测按钮状态（不做任何处理，仅检测）
    if (isButton1Pressed())
    {
        Serial.println("Button 1 is pressed"); // 打印按钮1被按下的信息
    }

    if (isButton2Pressed())
    {
        Serial.println("Button 2 is pressed"); // 打印按钮2被按下的信息
    }

    unsigned long currentMillis = millis(); // 获取当前时间

    // 更新DHT11传感器数据
    if (dht11_ok)
    {
        readTemperature(); // 读取温度数据
        readHumidity();    // 读取湿度数据
    }

    if (currentMillis - lastDisplayUpdate >= displayUpdateInterval)
    {                                      // 检查是否到了更新显示的时间
        updateDisplay();                   // 更新OLED显示内容
        lastDisplayUpdate = currentMillis; // 更新上次显示更新的时间
    }

    // 如果墨水屏初始化成功，则更新墨水屏
    if (epaperInitialized)
    {
        updateEPaper(); // 更新墨水屏显示内容
    }

    bool wifiConnected = (WiFi.status() == WL_CONNECTED); // 检查WiFi是否连接

    if (wifiConnected && !wifiPreviouslyConnected)
    {                                      // 如果WiFi刚刚连接上
        Serial.println("WiFi connected!"); // 打印WiFi已连接的消息
        Serial.print("IP address: ");      // 打印IP地址信息前缀
        Serial.println(WiFi.localIP());    // 打印获取到的IP地址

        display.clearDisplay();              // 清除OLED显示屏内容
        display.setTextSize(1);              // 设置文本大小为1
        display.setTextColor(SSD1306_WHITE); // 设置文本颜色为白色
        display.setCursor(0, 0);             // 设置光标位置到左上角
        display.println("WiFi Connected!");  // 显示WiFi已连接的信息
        display.setCursor(0, 16);            // 设置光标位置到第二行
        display.print("IP: ");               // 显示IP地址前缀
        display.println(WiFi.localIP());     // 显示获取到的IP地址
        display.display();                   // 更新显示内容
        delay(2000);                         // 延迟2秒，让用户能看到WiFi连接信息

        // 初始化时间管理模块
        if (!timeManagerInitialized)
        { // 如果时间模块尚未初始化
            Serial.println("Initializing time manager...");
            if (initTimeManager())
            { // 尝试初始化时间管理模块
                Serial.println("Time manager initialized successfully");
                timeManagerInitialized = true; // 标记时间模块已初始化
            }
            else
            {
                Serial.println("Time manager initialization failed");
            }
        }

        if (!weatherServiceInitialized)
        { // 如果天气服务尚未初始化
            if (initWeatherService())
            {                                                  // 尝试初始化天气服务
                Serial.println("Weather service initialized"); // 如果成功，打印初始化成功信息
                weatherServiceInitialized = true;              // 标记天气服务已初始化

                // 如果墨水屏已初始化，连接WiFi成功后立即更新墨水屏一次，显示主页面
                if (epaperInitialized)
                {
                    currentEpaperPage = MAIN_PAGE; // 设置为主页面
                    drawMainPage();                // 绘制主页面
                }
            }
            else
            {                                                            // 如果初始化失败
                Serial.println("Weather service initialization failed"); // 打印初始化失败信息
                showError("Weather service init failed");                // 在显示屏上显示错误信息
                delay(2000);                                             // 延迟2秒，让用户能看到错误信息
            }
        }
    }
    else if (!wifiConnected && wifiPreviouslyConnected)
    {                                         // 如果WiFi刚刚断开连接
        Serial.println("WiFi disconnected!"); // 打印WiFi已断开的消息

        // 在WiFi断开连接时，如果墨水屏已初始化，显示断开连接信息
        if (epaperInitialized)
        {
            display_epaper.setFullWindow();
            display_epaper.firstPage();
            do
            {
                display_epaper.fillScreen(GxEPD_WHITE);
                u8g2_for_epaper.setFont(u8g2_font_inb16_mr);
                u8g2_for_epaper.setCursor(50, 50);
                u8g2_for_epaper.print("WiFi断开");
                u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
                u8g2_for_epaper.setCursor(30, 80);
                u8g2_for_epaper.print("正在等待重新连接...");
            } while (display_epaper.nextPage());
        }
    }

    wifiPreviouslyConnected = wifiConnected; // 更新上一次WiFi连接状态

    // 更新时间管理模块
    if (timeManagerInitialized)
    {
        updateTimeManager(); // 定期更新和保存时间
    }

    if (wifiConnected && weatherServiceInitialized)
    { // 如果WiFi已连接且天气服务已初始化
        if (!weatherInitialUpdateDone)
        {                                                                // 如果尚未完成首次天气数据更新
            Serial.println("Performing initial weather data update..."); // 打印正在进行首次天气数据更新的消息
            if (updateWeatherData())
            {                                                                // 尝试更新天气数据
                Serial.println("Initial weather data updated successfully"); // 如果成功，打印更新成功信息
                weatherInitialUpdateDone = true;                             // 标记首次天气数据更新已完成

                // 首次获取天气数据成功后，如果墨水屏已初始化，更新天气页面
                if (epaperInitialized)
                {
                    currentEpaperPage = WEATHER_PAGE; // 设置为天气页面
                    drawWeatherPage(currentWeather);  // 绘制天气页面
                }
            }
            else
            {                                                         // 如果更新失败
                Serial.println("Failed to get initial weather data"); // 打印获取天气数据失败的消息
            }
            lastWeatherUpdate = currentMillis; // 无论成功与否，都更新上次天气更新的时间
        }
        else if (currentMillis - lastWeatherUpdate >= weatherUpdateInterval)
        {                                               // 如果距离上次天气更新已过指定时间间隔
            Serial.println("Updating weather data..."); // 打印正在更新天气数据的消息
            if (checkAndUpdateWeather())
            {                                                        // 检查并尝试更新天气数据
                Serial.println("Weather data updated successfully"); // 如果成功，打印更新成功信息

                // 如果墨水屏已初始化，且当前页面是天气页面，立即更新天气信息
                if (epaperInitialized && currentEpaperPage == WEATHER_PAGE)
                {
                    drawWeatherPage(currentWeather); // 绘制天气页面
                }
            }
            else
            {                                                 // 如果更新失败
                Serial.println("Weather data update failed"); // 打印更新失败信息
            }
            lastWeatherUpdate = currentMillis; // 无论成功与否，都更新上次天气更新的时间
        }
    }
}
