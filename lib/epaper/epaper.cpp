#include "epaper.h" // 包含墨水屏头文件

// 定义墨水屏显示对象，使用SPI接口
// E029A01-FPC-A1实际上与Waveshare 2.9英寸墨水屏(128x296)兼容，使用SSD1680控制器
GxEPD2_BW<GxEPD2_290_T94, GxEPD2_290_T94::HEIGHT> display_epaper(GxEPD2_290_T94(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));
U8G2_FOR_ADAFRUIT_GFX u8g2_for_epaper; // U8g2字体渲染对象，用于更好的文本显示

EpaperPage currentEpaperPage = MAIN_PAGE; // 当前墨水屏页面，初始为主页面
unsigned long lastEpaperUpdate = 0;       // 上次墨水屏更新时间

// 时间格式化函数，将当前时间格式化为字符串
void getTimeString(char *buffer)
{
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        // 格式化为 HH:MM:SS
        sprintf(buffer, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }
    else
    {
        strcpy(buffer, "--:--:--");
    }
}

// 日期格式化函数，将当前日期格式化为字符串
void getDateString(char *buffer)
{
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        // 格式化为 YYYY-MM-DD
        sprintf(buffer, "%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    }
    else
    {
        strcpy(buffer, "----/--/--");
    }
}

// 初始化墨水屏
bool initEPaper()
{
    // 初始化SPI接口
    SPI.begin(EPAPER_SCK, EPAPER_MISO, EPAPER_MOSI, EPAPER_CS);

    // 初始化墨水屏
    display_epaper.init(0, false, 2, false);  // 使用默认参数初始化，发送2次复位脉冲
    display_epaper.setRotation(1);            // 设置屏幕旋转90度，使宽度为296，高度为128
    display_epaper.setTextColor(GxEPD_BLACK); // 设置文本颜色为黑色

    // 初始化U8g2字体渲染对象
    u8g2_for_epaper.begin(display_epaper);
    u8g2_for_epaper.setFontMode(1);                  // 设置字体模式为透明
    u8g2_for_epaper.setFontDirection(0);             // 设置字体方向为默认
    u8g2_for_epaper.setForegroundColor(GxEPD_BLACK); // 设置前景色为黑色
    u8g2_for_epaper.setBackgroundColor(GxEPD_WHITE); // 设置背景色为白色

    // 显示初始界面
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE);      // 清屏为白色背景
        u8g2_for_epaper.setFont(u8g2_font_inb16_mr); // 设置大字体
        u8g2_for_epaper.setCursor(60, 40);
        u8g2_for_epaper.print("ESP32");
        u8g2_for_epaper.setCursor(45, 70);
        u8g2_for_epaper.print("HomeKit");
        u8g2_for_epaper.setFont(u8g2_font_9x15_tf); // 设置小字体
        u8g2_for_epaper.setCursor(60, 100);
        u8g2_for_epaper.print("启动中...");
    } while (display_epaper.nextPage());

    Serial.println("E-Paper initialized"); // 打印初始化成功信息
    lastEpaperUpdate = millis();           // 更新上次刷新时间

    return true;
}

// 更新墨水屏显示内容
void updateEPaper()
{
    unsigned long currentMillis = millis(); // 获取当前时间

    // 如果距离上次更新的时间超过了更新间隔，则进行更新
    if (currentMillis - lastEpaperUpdate >= EPAPER_UPDATE_INTERVAL)
    {
        switchEpaperPage(); // 切换页面

        // 根据当前页面类型进行绘制
        switch (currentEpaperPage)
        {
        case MAIN_PAGE:
            drawMainPage(); // 绘制主页面
            break;
        case EPAPER_WEATHER_PAGE:
            drawWeatherPage(currentWeather); // 绘制天气页面
            break;
        case SENSOR_DETAILS_PAGE:
            drawSensorDetailsPage(); // 绘制传感器详情页面
            break;
        }

        lastEpaperUpdate = currentMillis; // 更新上次刷新时间
    }
}

// 绘制主页面
void drawMainPage()
{
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 清屏为白色背景

        // 绘制标题
        drawTitle("系统状态");

        // 显示时间日期
        char timeStr[20];
        char dateStr[20];
        getTimeString(timeStr);
        getDateString(dateStr);

        u8g2_for_epaper.setFont(u8g2_font_inb19_mn); // 设置大字体显示时间
        u8g2_for_epaper.setCursor(60, 45);
        u8g2_for_epaper.print(timeStr);

        u8g2_for_epaper.setFont(u8g2_font_9x15_tf); // 设置小字体显示日期
        u8g2_for_epaper.setCursor(90, 65);
        u8g2_for_epaper.print(dateStr);

        // 显示温湿度
        if (dht11_ok)
        {
            u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
            u8g2_for_epaper.setCursor(20, 90);
            u8g2_for_epaper.print("温度: ");
            u8g2_for_epaper.print(temperature);
            u8g2_for_epaper.print("°C");

            u8g2_for_epaper.setCursor(170, 90);
            u8g2_for_epaper.print("湿度: ");
            u8g2_for_epaper.print(humidity);
            u8g2_for_epaper.print("%");
        }
        else
        {
            u8g2_for_epaper.setCursor(30, 90);
            u8g2_for_epaper.print("传感器错误");
        }

        // 显示页面指示器
        u8g2_for_epaper.setFont(u8g2_font_6x10_tf);
        u8g2_for_epaper.setCursor(130, 120);
        u8g2_for_epaper.print("页面: 1/3");

        // 绘制电池状态图标
        displayBatteryStatus(250, 15, 75); // 假设电池电量为75%

    } while (display_epaper.nextPage());
}

// 绘制天气页面
void drawWeatherPage(const WeatherData &weather)
{
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 清屏为白色背景

        // 绘制标题
        drawTitle("天气信息");

        if (weather.is_valid)
        {
            // 显示城市
            u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 2);
            u8g2_for_epaper.print("城市: ");
            u8g2_for_epaper.print(weather.city);

            // 显示天气状况
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 3);
            u8g2_for_epaper.print("天气: ");
            u8g2_for_epaper.print(weather.weather);

            // 显示温度
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 4);
            u8g2_for_epaper.print("温度: ");
            u8g2_for_epaper.print(weather.temperature);
            u8g2_for_epaper.print("°C");

            // 显示湿度
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 5);
            u8g2_for_epaper.print("湿度: ");
            u8g2_for_epaper.print(weather.humidity);
            u8g2_for_epaper.print("%");

            // 显示风向和风速
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 6);
            u8g2_for_epaper.print("风向: ");
            u8g2_for_epaper.print(weather.wind_direction);

            u8g2_for_epaper.setCursor(160, TOP_MARGIN + LINE_HEIGHT * 6);
            u8g2_for_epaper.print("风速: ");
            u8g2_for_epaper.print(weather.wind_speed);
        }
        else
        {
            // 如果没有有效的天气数据，显示提示信息
            u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
            u8g2_for_epaper.setCursor(20, 60);
            u8g2_for_epaper.print("无有效天气数据");
            u8g2_for_epaper.setCursor(20, 80);
            u8g2_for_epaper.print("请检查WiFi连接");
        }

        // 显示页面指示器
        u8g2_for_epaper.setFont(u8g2_font_6x10_tf);
        u8g2_for_epaper.setCursor(130, 120);
        u8g2_for_epaper.print("页面: 2/3");

        // 显示更新时间
        char timeStr[20];
        getTimeString(timeStr);
        u8g2_for_epaper.setCursor(5, 120);
        u8g2_for_epaper.print("更新: ");
        u8g2_for_epaper.print(timeStr);

    } while (display_epaper.nextPage());
}

// 绘制传感器详情页面
void drawSensorDetailsPage()
{
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 清屏为白色背景

        // 绘制标题
        drawTitle("传感器详情");

        // 显示温度
        if (dht11_ok)
        {
            drawValueWithUnit(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 2, temperature, "°C", "温度");
            drawValueWithUnit(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 3, humidity, "%", "湿度");
        }
        else
        {
            u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 2);
            u8g2_for_epaper.print("DHT11传感器错误");
        }

        // 显示光线强度
        if (bh1750_ok)
        {
            drawValueWithUnit(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 4, lightLevel, "lx", "光照");
        }
        else
        {
            u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
            u8g2_for_epaper.setCursor(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 4);
            u8g2_for_epaper.print("BH1750传感器错误");
        }

        // 显示气体浓度
        drawValueWithUnit(LEFT_MARGIN, TOP_MARGIN + LINE_HEIGHT * 5, gasLevel, "ppm", "气体");

        // 显示页面指示器
        u8g2_for_epaper.setFont(u8g2_font_6x10_tf);
        u8g2_for_epaper.setCursor(130, 120);
        u8g2_for_epaper.print("页面: 3/3");

    } while (display_epaper.nextPage());
}

// 切换墨水屏页面
void switchEpaperPage()
{
    // 按照顺序循环切换页面
    switch (currentEpaperPage)
    {
    case MAIN_PAGE:
        currentEpaperPage = EPAPER_WEATHER_PAGE;
        break;
    case EPAPER_WEATHER_PAGE:
        currentEpaperPage = SENSOR_DETAILS_PAGE;
        break;
    case SENSOR_DETAILS_PAGE:
        currentEpaperPage = MAIN_PAGE;
        break;
    }
}

// 显示电池状态
void displayBatteryStatus(int x, int y, int percentage)
{
    // 绘制电池外框
    display_epaper.drawRect(x, y, 30, 15, GxEPD_BLACK);
    display_epaper.drawRect(x + 30, y + 4, 3, 7, GxEPD_BLACK);

    // 根据电池百分比绘制电量
    int width = 26 * percentage / 100;
    if (width > 0)
    {
        display_epaper.fillRect(x + 2, y + 2, width, 11, GxEPD_BLACK);
    }
}

// 绘制带单位的值
void drawValueWithUnit(int x, int y, float value, const char *unit, const char *label)
{
    u8g2_for_epaper.setFont(u8g2_font_9x15_tf);
    u8g2_for_epaper.setCursor(x, y);
    u8g2_for_epaper.print(label);
    u8g2_for_epaper.print(": ");
    u8g2_for_epaper.print(value);
    u8g2_for_epaper.print(" ");
    u8g2_for_epaper.print(unit);
}

// 绘制标题
void drawTitle(const char *title)
{
    u8g2_for_epaper.setFont(u8g2_font_inb16_mr);
    u8g2_for_epaper.setCursor(115, 20);
    u8g2_for_epaper.print(title);
    display_epaper.drawLine(10, 25, 286, 25, GxEPD_BLACK); // 绘制标题下的分隔线
}
