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

// 声明全局变量（外部链接）
extern uint8_t cmdBuffer[MAX_CMD_LEN]; // 命令缓冲区
extern uint8_t cmdIndex;               // 命令缓冲区索引
extern bool cmdStarted;                // 命令接收状态

// 串口2的命令缓冲区和状态变量
extern uint8_t serial2CmdBuffer[MAX_CMD_LEN]; // 串口2命令缓冲区
extern uint8_t serial2CmdIndex;               // 串口2命令缓冲区索引
extern bool serial2CmdStarted;                // 串口2命令接收状态

/**
 * @brief 处理已接收完成的命令
 * 
 * 解析接收到的命令缓冲区内容，并执行相应的操作。
 * 命令处理完成后会重置命令缓冲区。
 */
void processCommand();

/**
 * @brief 发送响应数据
 * 
 * @param response 要发送的响应字符串
 * 
 * 发送带有起始字节和结束字节的响应数据。
 */
void sendResponse(const char* response);

/**
 * @brief 处理接收到的字节
 * 
 * @param byte 接收到的字节
 * @return true 如果命令接收完成
 * @return false 如果命令未接收完成
 * 
 * 将接收到的字节加入命令缓冲区，识别起始和结束字节。
 * 当检测到完整命令时返回true。
 */
bool handleReceivedByte(uint8_t byte);

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
 * @brief 处理串口2接收到的字节
 * 
 * @param byte 串口2接收到的字节
 * @return true 如果命令接收完成
 * @return false 如果命令未接收完成
 * 
 * 将串口2接收到的字节加入命令缓冲区，识别起始和结束字节。
 * 当检测到完整命令时返回true。
 */
bool handleSerial2ReceivedByte(uint8_t byte);

/**
 * @brief 处理串口2接收到的命令
 * 
 * 解析串口2接收到的命令缓冲区内容，并执行相应的操作。
 * 命令处理完成后会重置命令缓冲区。
 */
void processSerial2Command();

/**
 * @brief 通过串口2发送响应数据
 * 
 * @param response 要发送的响应字符串
 * 
 * 通过串口2发送带有起始字节和结束字节的响应数据。
 */
void sendSerial2Response(const char* response);

#endif // COMMAND_H 