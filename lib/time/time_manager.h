#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <Preferences.h>

// NTP服务器配置
#define NTP_SERVER1 "ntp.aliyun.com"     // 阿里云NTP服务器
#define NTP_SERVER2 "cn.pool.ntp.org"    // 中国NTP服务器池
#define NTP_SERVER3 "time.windows.com"   // 微软时间服务器

// 时区配置(GMT+8)
#define GMT_OFFSET_SEC 8 * 3600
#define DAYLIGHT_OFFSET_SEC 0

// 时间保存相关配置
#define TIME_UPDATE_INTERVAL 3600000     // 时间更新间隔(毫秒)，默认每小时更新一次
#define TIME_SAVE_INTERVAL   6000000      // 保存本地时间间隔(毫秒)，默认每100分钟保存一次

// 本地持久化存储的命名空间和键
#define PREFS_NAMESPACE "time_data"
#define TIME_KEY "last_time"

/**
 * @brief 初始化时间管理模块
 * 
 * 从NTP服务器同步时间，并从本地存储中恢复上次保存的时间（如果有）
 * 
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool initTimeManager();

/**
 * @brief 从NTP服务器同步时间
 * 
 * 当WiFi连接时，从配置的NTP服务器同步当前时间
 * 
 * @return true 同步成功
 * @return false 同步失败
 */
bool syncTimeFromNTP();

/**
 * @brief 保存当前时间到本地存储
 * 
 * 将当前时间戳保存到非易失性存储中
 * 
 * @return true 保存成功
 * @return false 保存失败
 */
bool saveTimeToLocal();

/**
 * @brief 从本地存储恢复时间
 * 
 * 当无法从NTP服务器同步时，从本地存储恢复上次保存的时间
 * 
 * @return true 恢复成功
 * @return false 恢复失败或没有保存的时间
 */
bool restoreTimeFromLocal();

/**
 * @brief 获取当前时间的格式化字符串
 * 
 * @param buffer 输出缓冲区
 * @param size 缓冲区大小
 * @param format 时间格式，同strftime函数
 * @return true 获取成功
 * @return false 获取失败
 */
bool getFormattedTime(char* buffer, size_t size, const char* format = "%Y-%m-%d %H:%M:%S");

/**
 * @brief 获取当前Unix时间戳
 * 
 * @return time_t 当前Unix时间戳
 */
time_t getCurrentTime();

/**
 * @brief 定期检查和更新时间
 * 
 * 定期从NTP服务器同步时间，并保存到本地存储
 * 应在主循环中定期调用此函数
 */
void updateTimeManager();

/**
 * @brief 处理time命令
 * 
 * 当收到"time"命令时，返回当前时间信息
 * 
 * @return const char* 要返回的时间信息字符串
 */
const char* handleTimeCommand();

#endif // TIME_MANAGER_H 