#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>

// 命令起始和结束字节
#define CMD_START_BYTE 0xAA  // 命令起始字节
#define CMD_END_BYTE   0x55  // 命令结束字节
#define MAX_CMD_LEN    32    // 最大命令长度

// 串口2定义
#define SERIAL2_RX_PIN 16    // 串口2的接收引脚
#define SERIAL2_TX_PIN 17    // 串口2的发送引脚
#define SERIAL2_BAUD   9600  // 串口2的波特率

// 定义命令处理回调函数类型
typedef void (*CommandHandlerCallback)(const char* cmd, size_t len);

// 命令缓冲区结构体
typedef struct {
    uint8_t buffer[MAX_CMD_LEN];  // 缓冲区
    uint8_t index;                // 当前索引
    bool started;                 // 命令接收状态标志
} CommandBuffer;

/**
 * @brief 初始化串口2
 * 
 * 初始化串口2并设置相应的引脚和波特率。
 */
void initSerial2();

/**
 * @brief 处理串口2接收到的命令
 * 
 * 检查串口2是否有可用数据，并处理接收到的命令。
 * 如果接收到完整命令，则执行处理逻辑。
 */
void handleSerial2Commands();

/**
 * @brief 通过串口2发送响应数据
 * 
 * @param response 要发送的响应字符串
 * 
 * 通过串口2发送带有起始字节和结束字节的响应数据。
 */
void sendSerial2Response(const char* response);

/**
 * @brief 处理接收到的命令
 * 
 * @param cmd 命令字符串
 * @param len 命令长度
 * 
 * 根据命令类型分派到相应的处理函数。
 */
void processSerialCommand(const char* cmd, size_t len);

/**
 * @brief 处理接收到的LED相关命令
 * 
 * @param cmd 命令字符串
 * @param len 命令长度
 * 
 * 处理LED控制相关的命令，如开关、调节亮度等
 */
void handleLEDCommand(const char* cmd, size_t len);

/**
 * @brief 处理接收到的传感器相关命令
 * 
 * @param cmd 命令字符串
 * @param len 命令长度
 * 
 * 处理传感器数据查询相关的命令，如温度、湿度、光照等
 */
void handleSensorCommand(const char* cmd, size_t len);

/**
 * @brief 处理接收到的时间和天气相关命令
 * 
 * @param cmd 命令字符串
 * @param len 命令长度
 * 
 * 处理时间查询和天气查询相关的命令
 */
void handleTimeWeatherCommand(const char* cmd, size_t len);

#endif // COMMAND_H 