#include "time_manager.h"

// 全局变量
static Preferences timePrefs;            // 用于存储时间信息的Preferences对象
static unsigned long lastTimeUpdate = 0; // 上次从NTP服务器更新时间的时间戳
static unsigned long lastTimeSave = 0;   // 上次保存时间到本地存储的时间戳
static bool timeInitialized = false;     // 时间模块是否已初始化（改为static限制作用域）
static char timeStringBuffer[64];        // 用于存储格式化时间字符串的缓冲区

// 初始化时间管理模块
bool initTimeManager()
{
    // 打开Preferences存储命名空间
    if (!timePrefs.begin(PREFS_NAMESPACE, false))
    {
        Serial.println("Failed to open Preferences for time storage");
        return false;
    }

    // 尝试从NTP服务器同步时间
    bool syncSuccess = syncTimeFromNTP();

    // 如果NTP同步失败，则尝试从本地存储恢复时间
    if (!syncSuccess)
    {
        syncSuccess = restoreTimeFromLocal();
        if (syncSuccess)
        {
            Serial.println("Restored time from local storage");
        }
        else
        {
            Serial.println("Failed to restore time from local storage");
            // 如果恢复也失败，则设置一个默认时间（2023年1月1日00:00:00）
            struct tm defaultTime;
            defaultTime.tm_year = 2023 - 1900; // 年份需要减去1900
            defaultTime.tm_mon = 0;            // 月份从0开始（0为1月）
            defaultTime.tm_mday = 1;           // 日期
            defaultTime.tm_hour = 0;           // 小时
            defaultTime.tm_min = 0;            // 分钟
            defaultTime.tm_sec = 0;            // 秒
            time_t defaultTimestamp = mktime(&defaultTime);
            timeval defaultTv = {.tv_sec = defaultTimestamp};
            settimeofday(&defaultTv, NULL);
            Serial.println("Set default time: 2023-01-01 00:00:00");
        }
    }

    // 初始化时间戳
    lastTimeUpdate = millis();
    lastTimeSave = millis();

    // 保存初始时间到本地存储
    saveTimeToLocal();

    // 打印当前时间
    char timeStr[32];
    if (getFormattedTime(timeStr, sizeof(timeStr)))
    {
        Serial.print("Current time: ");
        Serial.println(timeStr);
    }

    timeInitialized = true;
    return true;
}

// 从NTP服务器同步时间
bool syncTimeFromNTP()
{
    // 检查WiFi是否连接
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Cannot sync time from NTP: WiFi not connected");
        return false;
    }

    // 配置NTP服务器
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

    Serial.println("Waiting for NTP time sync...");

    // 等待NTP同步，最多等待10秒
    time_t nowSecs = time(nullptr);
    uint8_t attempts = 0;
    const uint8_t maxAttempts = 20; // 最多尝试20次，每次等待500毫秒

    while (nowSecs < 8 * 3600 * 2 && attempts < maxAttempts)
    {
        delay(500);
        Serial.print(".");
        nowSecs = time(nullptr);
        attempts++;
    }

    Serial.println();

    if (attempts >= maxAttempts)
    {
        Serial.println("Failed to sync time from NTP");
        return false;
    }

    // 获取当前时间并打印
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return false;
    }

    Serial.println("Time synchronized from NTP server");
    return true;
}

// 保存当前时间到本地存储
bool saveTimeToLocal()
{
    time_t now = time(nullptr);

    // 保存时间戳到Preferences
    if (!timePrefs.putULong(TIME_KEY, now))
    {
        Serial.println("Failed to save time to local storage");
        return false;
    }

    Serial.println("Time saved to local storage");
    return true;
}

// 从本地存储恢复时间
bool restoreTimeFromLocal()
{
    // 从Preferences获取保存的时间戳
    time_t savedTime = timePrefs.getULong(TIME_KEY, 0);

    if (savedTime == 0)
    {
        Serial.println("No saved time found in local storage");
        return false;
    }

    // 设置系统时间
    struct timeval tv;
    tv.tv_sec = savedTime;
    tv.tv_usec = 0;

    if (settimeofday(&tv, NULL) != 0)
    {
        Serial.println("Failed to set time from local storage");
        return false;
    }

    return true;
}

// 获取当前时间的格式化字符串
bool getFormattedTime(char *buffer, size_t size, const char *format)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        strncpy(buffer, "Time not available", size);
        return false;
    }

    // 使用strftime格式化时间
    if (strftime(buffer, size, format, &timeinfo) == 0)
    {
        strncpy(buffer, "Format error", size);
        return false;
    }

    return true;
}

// 获取当前Unix时间戳
time_t getCurrentTime()
{
    return time(nullptr);
}

// 定期检查和更新时间
void updateTimeManager()
{
    // 如果时间模块未初始化，则返回
    if (!timeInitialized)
    {
        return;
    }

    unsigned long currentMillis = millis();

    // 定期从NTP服务器更新时间
    if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            if (syncTimeFromNTP())
            {
                Serial.println("Time updated from NTP server");
            }
        }
        lastTimeUpdate = currentMillis;
    }

    // 定期保存时间到本地存储
    if (currentMillis - lastTimeSave >= TIME_SAVE_INTERVAL)
    {
        if (saveTimeToLocal())
        {
            Serial.println("Time saved to local storage");
        }
        lastTimeSave = currentMillis;
    }
}

// 处理time命令
const char *handleTimeCommand()
{
    // 获取当前时间，格式化为字符串
    if (getFormattedTime(timeStringBuffer, sizeof(timeStringBuffer)))
    {
        // 返回格式化的时间字符串
        return timeStringBuffer;
    }
    else
    {
        // 如果获取时间失败，返回错误信息
        return "Error: Time not available";
    }
}

// 检查时间是否已设置
bool isTimeSet()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        return false;
    }

    // 检查是否是默认时间（2023-01-01 00:00:00）或无效时间
    if (timeinfo.tm_year <= (2023 - 1900) &&
        timeinfo.tm_mon == 0 &&
        timeinfo.tm_mday == 1 &&
        timeinfo.tm_hour == 0 &&
        timeinfo.tm_min == 0 &&
        timeinfo.tm_sec < 10)
    {
        return false;
    }

    return true;
}

// 获取当前时间字符串
String getCurrentTimeStr()
{
    char timeStr[9]; // HH:MM:SS + null terminator
    if (getFormattedTime(timeStr, sizeof(timeStr), "%H:%M:%S"))
    {
        return String(timeStr);
    }
    return String("--:--:--");
}

// 获取当前日期字符串
String getCurrentDateStr()
{
    char dateStr[11]; // YYYY-MM-DD + null terminator
    if (getFormattedTime(dateStr, sizeof(dateStr), "%Y-%m-%d"))
    {
        return String(dateStr);
    }
    return String("----/--/--");
}