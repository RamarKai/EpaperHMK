#include "epaper.h" // 包含墨水屏的头文件

// 创建GxEPD2显示器对象，使用GxEPD2_290显示器控制器，分辨率为128x296像素
GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display_epaper(GxEPD2_290(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // 创建U8g2字体对象，用于显示中文等复杂字符

bool epaper_initialized = false; // 墨水屏初始化状态标志

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

    // 更新显示内容
    refreshDisplay();
}

// 显示整个屏幕内容（第一次全刷）
void refreshDisplay()
{
    // 获取当前时间和日期
    char timeStr[6] = "--:--";       // HH:MM
    char dateStr[12] = "----/--/--"; // YYYY-MM-DD

    // 获取星期几
    const char *weekDay = "未知";

    struct tm timeinfo;
    if (isTimeSet() && getLocalTime(&timeinfo))
    {
        sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        sprintf(dateStr, "%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

        const char *weekDays[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
        weekDay = weekDays[timeinfo.tm_wday];
    }

    // 获取天气数据
    WeatherData weather = getWeatherData();
    String weatherText = weather.is_valid ? weather.weather : "无天气数据";
    String temperatureText = weather.is_valid ? (String(weather.temperature) + "°C") : "--°C";

    // 获取传感器数据
    String humidityText = dht11_ok ? (String(humidity) + "%") : "--%";
    String lightText = bh1750_ok ? (String(lightLevel) + " lx") : "-- lx";
    String airQualityText = "空气:" + String(gasLevel);

    // 分区域更新显示

    // 1. 更新时间区域 (大字体)
    u8g2Fonts.setFontMode(1);
    u8g2Fonts.setFontDirection(0);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

    u8g2Fonts.setFont(u8g2_font_logisoso46_tf);       // 使用大字体显示时间
    display_epaper.setPartialWindow(24, 96, 180, 60); // 时间显示区域
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE);
        u8g2Fonts.setCursor(24, 144);
        u8g2Fonts.print(timeStr);
    } while (display_epaper.nextPage());

    // 2. 更新日期和星期区域
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
    display_epaper.setPartialWindow(200, 96, 96, 60); // 日期显示区域
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE);
        u8g2Fonts.setCursor(200, 120);
        u8g2Fonts.print(dateStr);
        u8g2Fonts.setCursor(200, 140);
        u8g2Fonts.print(weekDay);
    } while (display_epaper.nextPage());

    // 3. 更新顶部黑底白字信息区域
    u8g2Fonts.setForegroundColor(GxEPD_WHITE);
    u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);

    display_epaper.setPartialWindow(0, 0, display_epaper.width(), 80); // 顶部信息区域
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_BLACK);

        // 天气信息
        u8g2Fonts.setCursor(10, 20);
        u8g2Fonts.print("天气: ");
        u8g2Fonts.print(weatherText.c_str());

        // 温度信息
        u8g2Fonts.setCursor(160, 20);
        u8g2Fonts.print("温度: ");
        u8g2Fonts.print(temperatureText.c_str());

        // 湿度信息
        u8g2Fonts.setCursor(10, 50);
        u8g2Fonts.print("湿度: ");
        u8g2Fonts.print(humidityText.c_str());

        // 光照强度
        u8g2Fonts.setCursor(160, 50);
        u8g2Fonts.print("光照: ");
        u8g2Fonts.print(lightText.c_str());

        // 空气质量
        u8g2Fonts.setCursor(10, 75);
        u8g2Fonts.print(airQualityText.c_str());

        // WiFi状态
        u8g2Fonts.setCursor(160, 75);
        u8g2Fonts.print("WiFi: ");
        u8g2Fonts.print(WiFi.status() == WL_CONNECTED ? "已连接" : "未连接");

    } while (display_epaper.nextPage());
}

// 更新局部时间显示
void updateTimeDisplay()
{
    if (!epaper_initialized || !isTimeSet())
    {
        return;
    }

    // 获取当前时间
    char timeStr[6]; // HH:MM
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

        // 使用局部更新仅更新时间
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        u8g2Fonts.setFont(u8g2_font_logisoso46_tf);
        display_epaper.setPartialWindow(24, 96, 180, 60);
        display_epaper.firstPage();
        do
        {
            display_epaper.fillScreen(GxEPD_WHITE);
            u8g2Fonts.setCursor(24, 144);
            u8g2Fonts.print(timeStr);
        } while (display_epaper.nextPage());
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

        // 顶部黑色标题栏
        display_epaper.fillRect(0, 0, display_epaper.width(), 40, GxEPD_BLACK);
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        int16_t title_w = u8g2Fonts.getUTF8Width("ESP32 HomeKit");
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setCursor((display_epaper.width() - title_w) / 2, 25);
        u8g2Fonts.print("ESP32 HomeKit");
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);

        // 显示中间的标题和信息
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        u8g2Fonts.setCursor((display_epaper.width() - u8g2Fonts.getUTF8Width("传感器监控系统")) / 2, 80);
        u8g2Fonts.print("传感器监控系统");

        u8g2Fonts.setCursor((display_epaper.width() - u8g2Fonts.getUTF8Width("正在初始化...")) / 2, 110);
        u8g2Fonts.print("正在初始化...");

        // 底部版本号
        display_epaper.drawLine(0, 130, display_epaper.width(), 130, GxEPD_BLACK);
        u8g2Fonts.setCursor(10, 145);
        u8g2Fonts.print("固件版本: v2.1");

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

        // 使用黑底白字显示错误标题
        display_epaper.fillRect(0, 0, display_epaper.width(), 40, GxEPD_BLACK);
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        int16_t title_w = u8g2Fonts.getUTF8Width("错误");
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setCursor((display_epaper.width() - title_w) / 2, 25);
        u8g2Fonts.print("错误");
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);

        // 显示错误信息
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);

        // 计算文本行数并适当显示
        const char *ptr = errorMsg;
        int yPos = 70;

        while (*ptr)
        {
            char buffer[50];
            int i = 0;

            // 提取一行文本（最多40个字符）
            while (*ptr && i < 40 && *ptr != '\n')
            {
                buffer[i++] = *ptr++;
            }

            // 如果遇到换行符，跳过
            if (*ptr == '\n')
            {
                ptr++;
            }

            buffer[i] = '\0';

            // 显示这一行文本
            u8g2Fonts.setCursor(10, yPos);
            u8g2Fonts.print(buffer);

            // 更新Y位置
            yPos += 25;

            // 防止超出显示区域
            if (yPos > 140)
            {
                break;
            }
        }

    } while (display_epaper.nextPage());
}
