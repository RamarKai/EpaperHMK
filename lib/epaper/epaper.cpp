#include "epaper.h" // 包含墨水屏的头文件

// 创建GxEPD2显示器对象，使用GxEPD2_290显示器控制器，分辨率为128x296像素
GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display_epaper(GxEPD2_290(EPAPER_CS, EPAPER_DC, EPAPER_RST, EPAPER_BUSY));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; // 创建U8g2字体对象，用于显示中文等复杂字符

bool epaper_initialized = false; // 墨水屏初始化状态标志

// 定义自定义短语变量并设置默认值
String customPhrase = "天行健,君子以自强不息";

// 设置自定义短语
void setCustomPhrase(const char *phrase)
{
    customPhrase = String(phrase);
}

// 是否启用测试数据模式（固定室内温度和湿度）
bool useTestData = true;
float testIndoorTemperature = 20.0; // 测试室内温度为20度
float testIndoorHumidity = 35.0;    // 测试室内湿度为35%（干燥室内湿度）

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

// 获取空气质量评级
String getAirQualityRating(int value)
{
    if (value < 300)
    {
        return "优";
    }
    else if (value < 600)
    {
        return "良";
    }
    else if (value < 900)
    {
        return "中";
    }
    else
    {
        return "差";
    }
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
    // 保留一位小数显示温度
    String temperatureText = weather.is_valid ? (String(weather.temperature, 1) + "°C") : "--°C";
    String cityText = weather.is_valid ? weather.city : "未知";

    // 获取或使用测试传感器数据
    float indoorTemperature = useTestData ? testIndoorTemperature : temperature;
    float indoorHumidity = useTestData ? testIndoorHumidity : humidity;

    String humidityText = dht11_ok ? (String((int)indoorHumidity) + "%") : "--%";
    // 光照显示为整数，不显示小数点
    String lightText = bh1750_ok ? (String((int)lightLevel) + " lx") : "-- lx";

    // 获取空气质量评级
    String airQualityRating = getAirQualityRating(gasLevel);

    // 获取LED灯状态
    bool led_is_on = ledState;

    // 开始全屏刷新
    display_epaper.setFullWindow();
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 设置整个屏幕背景为白色

        // 计算左右分区的尺寸
        int leftWidth = display_epaper.width() / 3;      // 左侧占据1/3
        int rightWidth = display_epaper.width() * 2 / 3; // 右侧占据2/3
        int rightStart = leftWidth;                      // 右侧区域起始位置

        // 右侧区域 - 黑底白字 - 日期和时间
        display_epaper.fillRect(rightStart, 0, rightWidth, display_epaper.height(), GxEPD_BLACK);

        // 设置右侧文本属性
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setBackgroundColor(GxEPD_BLACK);

        // 显示日期和星期 - 更大字体
        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312); // 使用16pt字体
        String dateAndWeek = String(dateStr) + " " + weekDay;
        int16_t date_width = u8g2Fonts.getUTF8Width(dateAndWeek.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - date_width) / 2, 25);
        u8g2Fonts.print(dateAndWeek.c_str());

        // 显示时间 - 更大字体
        u8g2Fonts.setFont(u8g2_font_logisoso50_tf);
        int16_t time_width = u8g2Fonts.getUTF8Width(timeStr);
        u8g2Fonts.setCursor(rightStart + (rightWidth - time_width) / 2, 90);
        u8g2Fonts.print(timeStr);

        // 显示自定义短语
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        int16_t phrase_width = u8g2Fonts.getUTF8Width(customPhrase.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - phrase_width) / 2, 118);
        u8g2Fonts.print(customPhrase.c_str());

        // 左侧区域 - 白底黑字 - 传感器数据
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        // 第一行: 当前地点，天气情况和空气质量
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        String locationWeather = cityText + " " + weatherText + " " + airQualityRating;
        u8g2Fonts.setCursor(5, 20);
        u8g2Fonts.print(locationWeather.c_str());

        // 绘制灯泡图标在第一行最右侧
        int bulbX = leftWidth - 20; // 灯泡X坐标
        int bulbY = 15;             // 灯泡Y坐标
        int bulbRadius = 7;         // 灯泡圆形部分半径

        // 画灯泡外形（圆形部分）
        display_epaper.fillCircle(bulbX, bulbY, bulbRadius, GxEPD_BLACK);

        // 画灯泡底部（矩形部分）
        display_epaper.fillRect(bulbX - 3, bulbY + bulbRadius - 1, 6, 4, GxEPD_BLACK);

        // 如果LED灯开启，画亮起的灯泡内部
        if (led_is_on)
        {
            display_epaper.fillCircle(bulbX, bulbY, bulbRadius - 2, GxEPD_WHITE);
            // 画几条光线
            display_epaper.drawLine(bulbX - bulbRadius - 4, bulbY, bulbX - bulbRadius - 1, bulbY, GxEPD_BLACK);
            display_epaper.drawLine(bulbX + bulbRadius + 1, bulbY, bulbX + bulbRadius + 4, bulbY, GxEPD_BLACK);
            display_epaper.drawLine(bulbX, bulbY - bulbRadius - 4, bulbX, bulbY - bulbRadius - 1, GxEPD_BLACK);
        }

        // 第二行: 室外温度
        String outdoorTemp = "室外:" + temperatureText;
        u8g2Fonts.setCursor(5, 45);
        u8g2Fonts.print(outdoorTemp.c_str());

        // 第三行: 室内温度
        String indoorTemp = "室内:" + String(indoorTemperature, 1) + "°C";
        u8g2Fonts.setCursor(5, 70);
        u8g2Fonts.print(indoorTemp.c_str());

        // 第四行: 湿度
        String humidityInfo = "湿度:" + humidityText;
        u8g2Fonts.setCursor(5, 95);
        u8g2Fonts.print(humidityInfo.c_str());

        // 第五行: 光照
        String lightInfo = "光照:" + lightText;
        u8g2Fonts.setCursor(5, 120);
        u8g2Fonts.print(lightInfo.c_str());

        // 绘制分隔线
        display_epaper.drawLine(leftWidth, 0, leftWidth, display_epaper.height(), GxEPD_BLACK);

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

        // 计算左右区域的尺寸
        int leftWidth = display_epaper.width() / 3;      // 左侧占据1/3
        int rightWidth = display_epaper.width() * 2 / 3; // 右侧占据2/3
        int rightStart = leftWidth;                      // 右侧区域起始位置

        // 使用局部更新仅更新时间
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setBackgroundColor(GxEPD_BLACK);

        u8g2Fonts.setFont(u8g2_font_logisoso50_tf);
        int16_t time_width = u8g2Fonts.getUTF8Width(timeStr);
        int16_t x_pos = rightStart + (rightWidth - time_width) / 2;

        display_epaper.setPartialWindow(x_pos - 5, 40, time_width + 10, 55);
        display_epaper.firstPage();
        do
        {
            display_epaper.fillScreen(GxEPD_BLACK); // 确保背景是黑色
            u8g2Fonts.setCursor(x_pos, 90);
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
        display_epaper.fillScreen(GxEPD_WHITE); // 设置整个屏幕背景为白色

        // 计算左右分区的尺寸
        int leftWidth = display_epaper.width() / 3;      // 左侧占据1/3
        int rightWidth = display_epaper.width() * 2 / 3; // 右侧占据2/3
        int rightStart = leftWidth;                      // 右侧区域起始位置

        // 右侧区域 - 黑底白字
        display_epaper.fillRect(rightStart, 0, rightWidth, display_epaper.height(), GxEPD_BLACK);

        // 设置右侧文本属性
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setBackgroundColor(GxEPD_BLACK);

        // 右侧标题
        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        String title = "ESP32 智能家居";
        int16_t title_width = u8g2Fonts.getUTF8Width(title.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - title_width) / 2, 25);
        u8g2Fonts.print(title.c_str());

        // 右侧系统名称 - 大字体
        u8g2Fonts.setFont(u8g2_font_logisoso22_tf);
        String main_title = "传感器监控";
        int16_t main_title_width = u8g2Fonts.getUTF8Width(main_title.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - main_title_width) / 2, 70);
        u8g2Fonts.print(main_title.c_str());

        // 右侧初始化状态
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        String init_text = "系统初始化中";
        int16_t init_text_width = u8g2Fonts.getUTF8Width(init_text.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - init_text_width) / 2, 100);
        u8g2Fonts.print(init_text.c_str());

        String wait_text = "请等待初始化完成";
        int16_t wait_text_width = u8g2Fonts.getUTF8Width(wait_text.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - wait_text_width) / 2, 124);
        u8g2Fonts.print(wait_text.c_str());

        // 左侧区域 - 白底黑字 - 版本信息
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312); // 改为14pt字体

        // 版本和时间信息
        u8g2Fonts.setCursor(5, 30);
        u8g2Fonts.print("固件版本:");

        u8g2Fonts.setCursor(5, 60);
        u8g2Fonts.print("v2.1");

        u8g2Fonts.setCursor(5, 90);
        u8g2Fonts.print(__DATE__);

        u8g2Fonts.setCursor(5, 120);
        u8g2Fonts.print(__TIME__);

        // 绘制分隔线
        display_epaper.drawLine(leftWidth, 0, leftWidth, display_epaper.height(), GxEPD_BLACK);

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
        display_epaper.fillScreen(GxEPD_WHITE); // 设置整个屏幕背景为白色

        // 计算左右分区的尺寸
        int leftWidth = display_epaper.width() / 3;      // 左侧占据1/3
        int rightWidth = display_epaper.width() * 2 / 3; // 右侧占据2/3
        int rightStart = leftWidth;                      // 右侧区域起始位置

        // 右侧区域 - 黑底白字
        display_epaper.fillRect(rightStart, 0, rightWidth, display_epaper.height(), GxEPD_BLACK);

        // 设置右侧文本属性
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setBackgroundColor(GxEPD_BLACK);

        // 显示错误标题
        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        String error_title = "系统错误";
        int16_t title_width = u8g2Fonts.getUTF8Width(error_title.c_str());
        u8g2Fonts.setCursor(rightStart + (rightWidth - title_width) / 2, 25);
        u8g2Fonts.print(error_title.c_str());

        // 显示错误信息
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312); // 改为14pt字体

        // 计算文本行数并适当显示
        const char *ptr = errorMsg;
        int yPos = 60; // 从60像素处开始显示错误信息

        while (*ptr)
        {
            char buffer[28]; // 稍微调整缓冲区大小
            int i = 0;

            // 提取一行文本（调整为适合14pt字体宽度）
            while (*ptr && i < 24 && *ptr != '\n')
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
            u8g2Fonts.setCursor(rightStart + 10, yPos);
            u8g2Fonts.print(buffer);

            // 更新Y位置，增加行高
            yPos += 22; // 调整为适合14pt字体的行高

            // 防止超出显示区域
            if (yPos > 120)
            {
                break;
            }
        }

        // 左侧区域 - 白底黑字 - 错误提示
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        // 错误图标 - 更大的感叹号
        display_epaper.fillCircle(leftWidth / 2, 35, 20, GxEPD_BLACK);
        display_epaper.fillCircle(leftWidth / 2, 35, 17, GxEPD_WHITE);
        u8g2Fonts.setFont(u8g2_font_logisoso22_tf);
        u8g2Fonts.setCursor(leftWidth / 2 - 5, 42);
        u8g2Fonts.print("!");

        // 错误提示
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312); // 改为14pt字体
        u8g2Fonts.setCursor(8, 80);
        u8g2Fonts.print("错误代码:");

        // 生成简单错误代码
        char errorCode[8];
        sprintf(errorCode, "E%04X", (uint16_t)((uint32_t)errorMsg & 0xFFFF));
        u8g2Fonts.setCursor(15, 110);
        u8g2Fonts.print(errorCode);

        // 绘制分隔线
        display_epaper.drawLine(leftWidth, 0, leftWidth, display_epaper.height(), GxEPD_BLACK);

    } while (display_epaper.nextPage());
}

// 部分更新墨水屏上的传感器数据
void updateEPaperSensorData()
{
    // 检查墨水屏是否已初始化
    if (!epaper_initialized)
    {
        Serial.println("E-Paper not initialized, cannot update sensor data");
        return;
    }

    // 获取天气数据
    WeatherData weather = getWeatherData();
    // 保留一位小数显示温度
    String temperatureText = weather.is_valid ? (String(weather.temperature, 1) + "°C") : "--°C";
    String cityText = weather.is_valid ? weather.city : "未知";
    String weatherText = weather.is_valid ? weather.weather : "无天气数据";

    // 获取空气质量评级
    String airQualityRating = getAirQualityRating(gasLevel);

    // 获取或使用测试传感器数据
    float indoorTemperature = useTestData ? testIndoorTemperature : temperature;
    float indoorHumidity = useTestData ? testIndoorHumidity : humidity;

    String humidityText = dht11_ok ? (String((int)indoorHumidity) + "%") : "--%";
    // 光照显示为整数，不显示小数点
    String lightText = bh1750_ok ? (String((int)lightLevel) + " lx") : "-- lx";

    // 获取LED灯状态
    bool led_is_on = ledState;

    // 计算左侧区域的宽度
    int leftWidth = display_epaper.width() / 3; // 左侧占据1/3

    // 设置部分更新区域（只更新左侧区域）
    display_epaper.setPartialWindow(0, 0, leftWidth, display_epaper.height());
    display_epaper.firstPage();
    do
    {
        display_epaper.fillScreen(GxEPD_WHITE); // 设置背景为白色

        // 设置文本属性
        u8g2Fonts.setFontMode(1);
        u8g2Fonts.setFontDirection(0);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        // 第一行: 当前地点，天气情况和空气质量
        u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
        String locationWeather = cityText + " " + weatherText + " " + airQualityRating;
        u8g2Fonts.setCursor(5, 20);
        u8g2Fonts.print(locationWeather.c_str());

        // 绘制灯泡图标在第一行最右侧
        int bulbX = leftWidth - 20; // 灯泡X坐标
        int bulbY = 15;             // 灯泡Y坐标
        int bulbRadius = 7;         // 灯泡圆形部分半径

        // 画灯泡外形（圆形部分）
        display_epaper.fillCircle(bulbX, bulbY, bulbRadius, GxEPD_BLACK);

        // 画灯泡底部（矩形部分）
        display_epaper.fillRect(bulbX - 3, bulbY + bulbRadius - 1, 6, 4, GxEPD_BLACK);

        // 如果LED灯开启，画亮起的灯泡内部
        if (led_is_on)
        {
            display_epaper.fillCircle(bulbX, bulbY, bulbRadius - 2, GxEPD_WHITE);
            // 画几条光线
            display_epaper.drawLine(bulbX - bulbRadius - 4, bulbY, bulbX - bulbRadius - 1, bulbY, GxEPD_BLACK);
            display_epaper.drawLine(bulbX + bulbRadius + 1, bulbY, bulbX + bulbRadius + 4, bulbY, GxEPD_BLACK);
            display_epaper.drawLine(bulbX, bulbY - bulbRadius - 4, bulbX, bulbY - bulbRadius - 1, GxEPD_BLACK);
        }

        // 第二行: 室外温度
        String outdoorTemp = "室外:" + temperatureText;
        u8g2Fonts.setCursor(5, 45);
        u8g2Fonts.print(outdoorTemp.c_str());

        // 第三行: 室内温度
        String indoorTemp = "室内:" + String(indoorTemperature, 1) + "°C";
        u8g2Fonts.setCursor(5, 70);
        u8g2Fonts.print(indoorTemp.c_str());

        // 第四行: 湿度
        String humidityInfo = "湿度:" + humidityText;
        u8g2Fonts.setCursor(5, 95);
        u8g2Fonts.print(humidityInfo.c_str());

        // 第五行: 光照
        String lightInfo = "光照:" + lightText;
        u8g2Fonts.setCursor(5, 120);
        u8g2Fonts.print(lightInfo.c_str());

        // 绘制分隔线
        display_epaper.drawLine(leftWidth - 1, 0, leftWidth - 1, display_epaper.height(), GxEPD_BLACK);

    } while (display_epaper.nextPage());
}
