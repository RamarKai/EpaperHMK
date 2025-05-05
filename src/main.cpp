#include "HomeSpan.h"     // 包含HomeSpan库，用于HomeKit集成
#include "lightsensor.h"  // 包含光线传感器模块
#include "mq2sensor.h"    // 包含MQ2气体传感器模块
#include "oled.h"         // 包含OLED显示屏模块
#include "ws2812.h"       // 包含WS2812B LED控制模块
#include "weather.h"      // 包含天气服务模块
#include "command.h"      // 包含命令处理模块
#include "time_manager.h" // 包含时间管理模块
#include "dht11.h"        // 包含DHT11传感器模块
#include "epaper.h"       // 包含墨水屏显示模块

unsigned long lastDisplayUpdate = 0;             // 记录上次显示更新的时间
const unsigned long displayUpdateInterval = 100; // 设置显示更新的时间间隔为100毫秒

unsigned long lastWeatherUpdate = 0;                // 记录上次天气数据更新的时间
const unsigned long weatherUpdateInterval = 300000; // 设置天气数据更新的时间间隔为300000毫秒(5分钟)

unsigned long lastEPaperUpdate = 0;               // 记录上次墨水屏更新的时间
const unsigned long ePaperUpdateInterval = 40000; // 设置墨水屏完整更新的时间间隔为40000毫秒(40秒)

unsigned long lastEPaperSensorUpdate = 0;              // 记录上次墨水屏传感器数据更新的时间
const unsigned long ePaperSensorUpdateInterval = 3000; // 设置墨水屏传感器数据更新的时间间隔为3000毫秒(3秒)

bool weatherServiceInitialized = false; // 标记天气服务是否已初始化
bool weatherInitialUpdateDone = false;  // 标记是否已完成天气数据的首次更新
bool wifiPreviouslyConnected = false;   // 记录上一次WiFi的连接状态
bool timeManagerInitialized = false;    // 标记时间管理模块是否已初始化（改名以避免与time_manager.cpp中的变量冲突）
bool ePaperInitialized = false;         // 标记墨水屏是否已初始化
bool initializationComplete = false;    // 添加全局变量标记初始化完成状态

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

    // 初始化DHT11传感器并保存结果
    bool dht_init_success = initDHT11();
    if (!dht_init_success)
    {
        Serial.println("DHT11 initialization failed!"); // 如果初始化失败，打印错误信息
        showError("DHT11 init failed");                 // 在显示屏上显示错误信息
        delay(2000);                                    // 延迟2秒，让用户能看到错误信息
    }

    // 初始化墨水屏并保存结果
    ePaperInitialized = initEPaper();
    if (!ePaperInitialized)
    {
        Serial.println("E-Paper initialization failed!"); // 如果初始化失败，打印错误信息
        showError("E-Paper init failed");                 // 在OLED显示屏上显示错误信息
        delay(2000);                                      // 延迟2秒，让用户能看到错误信息
    }
    // 注意：此时已经在initEPaper函数中调用了showEPaperStartupScreen，显示了墨水屏启动界面

    // 初始化串口2用于命令通信
    initSerial2(); // 初始化串口2

    // 在OLED上更新状态
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("HomeSpan Starting...");
    display.setCursor(0, 16);
    display.println("Waiting for WiFi...");
    display.display();

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

    lastDisplayUpdate = millis(); // 记录当前时间为上次显示更新时间
    lastWeatherUpdate = millis(); // 记录当前时间为上次天气更新时间
    lastEPaperUpdate = millis();  // 记录当前时间为上次墨水屏更新时间

    // 不在setup中完成初始化标记，而是在loop中检测WiFi连接和初始化完成情况
}

void loop()
{                    // Arduino程序的主循环函数
    homeSpan.poll(); // 处理HomeSpan相关的事件和请求

    // 处理串口2接收到的命令
    handleSerial2Commands(); // 检查并处理串口2接收到的命令

    unsigned long currentMillis = millis(); // 获取当前时间

    // 更新DHT11传感器数据
    if (dht11_ok)
    {
        readTemperature(); // 读取温度数据
        readHumidity();    // 读取湿度数据
    }

    bool wifiConnected = (WiFi.status() == WL_CONNECTED); // 检查WiFi是否连接

    // 判断是否初始化完成
    if (!initializationComplete)
    {
        // 更新OLED显示状态
        if (currentMillis - lastDisplayUpdate >= displayUpdateInterval)
        {
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println("Initializing...");
            display.setCursor(0, 16);
            display.print("WiFi: ");
            display.println(wifiConnected ? "Connected" : "Waiting...");

            // 显示初始化进度
            display.setCursor(0, 32);
            display.print("Time: ");
            display.println(timeManagerInitialized ? "OK" : "Waiting...");

            display.setCursor(0, 44);
            display.print("Weather: ");
            display.println(weatherInitialUpdateDone ? "OK" : "Waiting...");

            display.display();
            lastDisplayUpdate = currentMillis;
        }

        // 检查所有初始化是否完成
        if (wifiConnected && timeManagerInitialized && weatherInitialUpdateDone)
        {
            initializationComplete = true;

            // 显示初始化完成消息
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println("Initialization");
            display.setCursor(0, 16);
            display.println("Complete!");
            display.setCursor(0, 32);
            display.println("Starting main");
            display.setCursor(0, 44);
            display.println("application...");
            display.display();

            // 刷新墨水屏，显示完整的主界面
            if (ePaperInitialized)
            {
                // 添加一个短暂的延迟让OLED显示完成
                delay(500);

                // 在OLED上显示更新墨水屏信息
                display.clearDisplay();
                display.setTextSize(1);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(0, 0);
                display.println("Updating E-Paper...");
                display.display();

                // 更新墨水屏显示主界面
                updateEPaper();

                // 更新完成后显示就绪信息
                display.clearDisplay();
                display.setCursor(0, 0);
                display.println("System Ready!");
                display.display();
            }

            delay(2000); // 显示完成消息2秒
        }
    }
    else
    {
        // 只有在初始化完成后才更新主界面显示
        if (currentMillis - lastDisplayUpdate >= displayUpdateInterval)
        {                                      // 检查是否到了更新显示的时间
            updateDisplay();                   // 更新显示内容
            lastDisplayUpdate = currentMillis; // 更新上次显示更新的时间
        }

        // 墨水屏传感器数据区域部分更新（每3秒更新一次传感器数据）
        if (ePaperInitialized && (currentMillis - lastEPaperSensorUpdate >= ePaperSensorUpdateInterval))
        {
            updateEPaperSensorData();               // 部分更新墨水屏上的传感器数据
            lastEPaperSensorUpdate = currentMillis; // 更新上次传感器数据更新的时间
        }

        // 墨水屏完整更新（每40秒刷新一次全屏）
        if (ePaperInitialized && (currentMillis - lastEPaperUpdate >= ePaperUpdateInterval))
        {
            updateEPaper();                         // 更新墨水屏全部内容
            lastEPaperUpdate = currentMillis;       // 更新上次墨水屏完整更新的时间
            lastEPaperSensorUpdate = currentMillis; // 同时也更新传感器数据时间戳
        }
    }

    if (wifiConnected && !wifiPreviouslyConnected)
    {                                      // 如果WiFi刚刚连接上
        Serial.println("WiFi connected!"); // 打印WiFi已连接的消息
        Serial.print("IP address: ");      // 打印IP地址信息前缀
        Serial.println(WiFi.localIP());    // 打印获取到的IP地址

        if (!initializationComplete)
        {
            // 在初始化界面上更新WiFi连接状态
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println("WiFi Connected!");
            display.setCursor(0, 16);
            display.print("IP: ");
            display.println(WiFi.localIP());
            display.setCursor(0, 32);
            display.println("Initializing...");
            display.display();
            delay(1000);
        }

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
            }
            else
            {                                                            // 如果初始化失败
                Serial.println("Weather service initialization failed"); // 打印初始化失败信息
                if (!initializationComplete)
                {
                    showError("Weather service init failed"); // 在显示屏上显示错误信息
                    delay(2000);                              // 延迟2秒，让用户能看到错误信息
                }
            }
        }
    }
    else if (!wifiConnected && wifiPreviouslyConnected)
    {                                         // 如果WiFi刚刚断开连接
        Serial.println("WiFi disconnected!"); // 打印WiFi已断开的消息
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
            }
            else
            {                                                 // 如果更新失败
                Serial.println("Weather data update failed"); // 打印更新失败信息
            }
            lastWeatherUpdate = currentMillis; // 无论成功与否，都更新上次天气更新的时间
        }
    }
}
