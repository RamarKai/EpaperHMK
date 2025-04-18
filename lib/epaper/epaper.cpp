#include "epaper.h" // 包含墨水屏的头文件

// 创建GxEPD2显示器对象，使用GxEPD2_290显示器控制器，分辨率为128x296像素
GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display_epaper(GxEPD2_290(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // 创建U8g2字体对象，用于显示中文等复杂字符

EPaperPage currentEPaperPage = TIME_WEATHER_PAGE;        // 初始化当前显示页面为时间和天气页面
unsigned long lastEPaperPageSwitchTime = 0;              // 记录上次页面切换时间
const unsigned long EPAPER_PAGE_SWITCH_INTERVAL = 10000; // 设置页面自动切换的间隔为30秒
bool epaper_initialized = false;                         // 墨水屏初始化状态标志

// 初始化墨水屏
bool initEPaper()
{
    // 初始化SPI连接
    SPI.begin(EPAPER_SCK, -1, EPAPER_MOSI, EPAPER_CS);

    // 初始化墨水屏
    display_epaper.init();

    // 设置墨水屏为横向显示（旋转90度）
    display_epaper.setRotation(1);

    // 初始化U8g2字体
    u8g2Fonts.begin(display_epaper);

    // 设置文本颜色为黑色（墨水屏上为黑色），背景为白色
    display_epaper.setTextColor(GxEPD_BLACK);

    // 清除屏幕内容为白色
    display_epaper.clearScreen();

    // 记录初始化时间为当前时间
    lastEPaperPageSwitchTime = millis();

    // 显示启动画面
    showEPaperStartupScreen();

    // 设置初始化成功标志
    epaper_initialized = true;

    Serial.println("E-Paper initialized (landscape mode)");

    return true;
}

// 更新墨水屏显示
void updateEPaper()
{
    // 检查墨水屏是否已初始化
    if (!epaper_initialized)
    {
        Serial.println("E-Paper not initialized, cannot update display");
        return;
    }

    // 尝试切换页面
    switchEPaperPage();

    // 根据当前页面类型更新显示内容
    switch (currentEPaperPage)
    {
    case TIME_WEATHER_PAGE:
        displayTimeWeatherPage();
        break;
    case EPAPER_SENSOR_PAGE:
        displaySensorPage();
        break;
    default:
        // 默认显示传感器页面
        displaySensorPage();
        break;
    }
}

// 显示时间和天气页面
void displayTimeWeatherPage()
{
    // 获取当前天气数据
    WeatherData weather = getWeatherData();

    // 开始绘制到缓冲区
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 设置背景为白色

        // 绘制页面标题
        display_epaper.setFont(&FreeSansBold12pt7b);
        display_epaper.setCursor(10, 20);
        display_epaper.println("时间和天气");

        // 绘制横线分隔标题区域
        display_epaper.drawLine(0, 25, display_epaper.width(), 25, GxEPD_BLACK);

        // 显示当前时间和日期（横向布局）
        display_epaper.setFont(&FreeSans9pt7b);

        // 左侧显示时间信息
        if (isTimeSet())
        {
            // 时间显示
            display_epaper.setCursor(10, 50);
            display_epaper.print("时间: ");
            display_epaper.print(getCurrentTimeStr());

            // 日期显示
            display_epaper.setCursor(10, 75);
            display_epaper.print("日期: ");
            display_epaper.print(getCurrentDateStr());
        }
        else
        {
            display_epaper.setCursor(10, 50);
            display_epaper.print("时间未设置");
        }

        // 右侧显示天气信息，利用横屏更宽的空间
        int rightColumnX = 160; // 右侧列的X坐标

        // 显示天气信息（如果有效）
        if (weather.is_valid)
        {
            // 左侧天气信息
            display_epaper.setCursor(10, 100);
            display_epaper.print("城市: ");
            display_epaper.print(weather.city);

            display_epaper.setCursor(10, 120);
            display_epaper.print("天气: ");
            display_epaper.print(weather.weather);

            // 右侧天气信息
            display_epaper.setCursor(rightColumnX, 50);
            display_epaper.print("温度: ");
            display_epaper.print(weather.temperature);
            display_epaper.print(" C");

            display_epaper.setCursor(rightColumnX, 75);
            display_epaper.print("湿度: ");
            display_epaper.print(weather.humidity);
            display_epaper.print("%");

            display_epaper.setCursor(rightColumnX, 100);
            display_epaper.print("风速: ");
            display_epaper.print(weather.wind_speed);
            display_epaper.print(" 级");

            display_epaper.setCursor(rightColumnX, 120);
            display_epaper.print("风向: ");
            display_epaper.print(weather.wind_direction);
        }
        else
        {
            // 如果没有有效的天气数据，显示提示信息
            display_epaper.setCursor(10, 100);
            display_epaper.print("无有效天气数据");
            display_epaper.setCursor(10, 120);
            display_epaper.print("请检查网络连接");
        }

        // 显示页面指示器
        display_epaper.setCursor(250, 120);
        display_epaper.print("1/");
        display_epaper.print(PAGE_COUNT);

    } while (display_epaper.nextPage());
}

// 显示传感器数据页面
void displaySensorPage()
{
    // 开始绘制到缓冲区
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 设置背景为白色

        // 绘制页面标题
        display_epaper.setFont(&FreeSansBold12pt7b);
        display_epaper.setCursor(10, 20);
        display_epaper.println("传感器数据");

        // 绘制横线分隔标题区域
        display_epaper.drawLine(0, 25, display_epaper.width(), 25, GxEPD_BLACK);

        // 横屏布局，左右两列显示数据
        display_epaper.setFont(&FreeSans9pt7b);
        int rightColumnX = 160; // 右侧列的X坐标

        // 左侧显示温湿度和光照
        // 显示温度数据
        display_epaper.setCursor(10, 50);
        display_epaper.print("温度: ");
        if (dht11_ok)
        {
            display_epaper.print(temperature);
            display_epaper.print(" C");
        }
        else
        {
            display_epaper.print("错误");
        }

        // 显示湿度数据
        display_epaper.setCursor(10, 75);
        display_epaper.print("湿度: ");
        if (dht11_ok)
        {
            display_epaper.print(humidity);
            display_epaper.print("%");
        }
        else
        {
            display_epaper.print("错误");
        }

        // 显示光照强度
        display_epaper.setCursor(10, 100);
        display_epaper.print("光照: ");
        if (bh1750_ok)
        {
            display_epaper.print(lightLevel);
            display_epaper.print(" lx");
        }
        else
        {
            display_epaper.print("错误");
        }

        // 右侧显示气体和WiFi信息
        // 显示气体浓度
        display_epaper.setCursor(rightColumnX, 50);
        display_epaper.print("气体: ");
        display_epaper.print(gasLevel);

        // 显示空气质量等级
        int airQuality = convertToAirQuality(gasLevel);
        display_epaper.setCursor(rightColumnX, 75);
        display_epaper.print("空气质量: ");

        // 根据不同的空气质量等级显示不同的文本
        switch (airQuality)
        {
        case 1:
            display_epaper.print("优");
            break;
        case 2:
            display_epaper.print("良");
            break;
        case 3:
            display_epaper.print("一般");
            break;
        case 4:
            display_epaper.print("较差");
            break;
        case 5:
            display_epaper.print("差");
            break;
        default:
            display_epaper.print("未知");
            break;
        }

        // 显示WiFi连接状态
        display_epaper.setCursor(rightColumnX, 100);
        display_epaper.print("WiFi: ");
        if (WiFi.status() == WL_CONNECTED)
        {
            display_epaper.print("已连接");
        }
        else
        {
            display_epaper.print("未连接");
        }

        // 显示IP地址
        if (WiFi.status() == WL_CONNECTED)
        {
            display_epaper.setCursor(10, 120);
            display_epaper.print("IP: ");
            display_epaper.print(WiFi.localIP().toString());
        }

        // 显示页面指示器
        display_epaper.setCursor(250, 120);
        display_epaper.print("2/");
        display_epaper.print(PAGE_COUNT);

    } while (display_epaper.nextPage());
}

// 切换墨水屏显示页面
void switchEPaperPage()
{
    unsigned long currentTime = millis();

    // 如果已经过了指定的页面切换时间间隔
    if (currentTime - lastEPaperPageSwitchTime >= EPAPER_PAGE_SWITCH_INTERVAL)
    {
        // 切换到下一个页面
        currentEPaperPage = static_cast<EPaperPage>((currentEPaperPage + 1) % PAGE_COUNT);

        // 更新上次页面切换时间
        lastEPaperPageSwitchTime = currentTime;

        // 打印页面切换信息
        Serial.print("E-Paper page switched to: ");
        Serial.println(currentEPaperPage);
    }
}

// 使墨水屏进入深度睡眠模式以节省电量
void deepSleepEPaper()
{
    if (epaper_initialized)
    {
        display_epaper.hibernate();
        Serial.println("E-Paper entered deep sleep mode");
    }
}

// 唤醒墨水屏
void wakeupEPaper()
{
    if (epaper_initialized)
    {
        display_epaper.init();
        display_epaper.setRotation(1); // 确保唤醒后仍然保持横屏显示
        Serial.println("E-Paper woke up from sleep mode");
    }
}

// 显示墨水屏启动画面
void showEPaperStartupScreen()
{
    // 开始绘制到缓冲区
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 设置背景为白色

        // 横屏布局启动画面，居中显示
        // 绘制标题
        display_epaper.setFont(&FreeSansBold12pt7b);
        display_epaper.setCursor(50, 45);
        display_epaper.println("ESP32 HomeKit");

        // 绘制副标题
        display_epaper.setFont(&FreeSans9pt7b);
        display_epaper.setCursor(70, 75);
        display_epaper.println("传感器监控系统");

        // 绘制初始化信息
        display_epaper.setCursor(90, 100);
        display_epaper.println("正在初始化...");

        // 版本号
        display_epaper.setCursor(90, 120);
        display_epaper.print("固件版本: v1.0");

    } while (display_epaper.nextPage());

    // 等待2秒钟以显示启动画面
    delay(2000);
}

// 在墨水屏上显示错误信息
void showEPaperError(const char *errorMsg)
{
    // 如果墨水屏未初始化，尝试初始化
    if (!epaper_initialized)
    {
        if (!initEPaper())
        {
            Serial.println("Failed to initialize E-Paper for error display");
            return;
        }
    }

    // 开始绘制到缓冲区
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 设置背景为白色

        // 绘制错误标题
        display_epaper.setFont(&FreeSansBold12pt7b);
        display_epaper.setCursor(10, 30);
        display_epaper.println("错误");

        // 绘制横线分隔标题区域
        display_epaper.drawLine(0, 35, display_epaper.width(), 35, GxEPD_BLACK);

        // 显示错误信息
        display_epaper.setFont(&FreeSans9pt7b);
        display_epaper.setCursor(10, 70);
        display_epaper.println(errorMsg);

    } while (display_epaper.nextPage());
}

// 局部更新墨水屏，只更新指定区域
void partialUpdateEPaper()
{
    if (!epaper_initialized)
    {
        Serial.println("E-Paper not initialized, cannot perform partial update");
        return;
    }

    // 创建一个局部更新区域，例如只更新时间区域
    if (currentEPaperPage == TIME_WEATHER_PAGE)
    {
        // 只更新时间区域
        if (isTimeSet())
        {
            display_epaper.setPartialWindow(10, 30, 150, 30);
            display_epaper.firstPage();
            do
            {
                display_epaper.fillScreen(GxEPD_WHITE);
                display_epaper.setFont(&FreeSans9pt7b);
                display_epaper.setCursor(10, 50);
                display_epaper.print("时间: ");
                display_epaper.print(getCurrentTimeStr());
            } while (display_epaper.nextPage());
        }
    }
}
