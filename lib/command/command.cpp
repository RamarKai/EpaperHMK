#include "command.h"
#include "ws2812.h"  // 包含WS2812控制相关头文件

// 定义全局变量
uint8_t cmdBuffer[MAX_CMD_LEN]; // 命令缓冲区
uint8_t cmdIndex = 0;           // 命令缓冲区索引
bool cmdStarted = false;        // 命令接收状态

// 定义串口2相关的全局变量
uint8_t serial2CmdBuffer[MAX_CMD_LEN]; // 串口2命令缓冲区
uint8_t serial2CmdIndex = 0;           // 串口2命令缓冲区索引
bool serial2CmdStarted = false;        // 串口2命令接收状态

// 处理接收到的命令
void processCommand() {
  // 确保命令缓冲区以null结尾，便于字符串操作
  cmdBuffer[cmdIndex] = 0;
  
  // 打印接收到的命令（调试用）
  Serial.print("Received command: ");
  for (int i = 0; i < cmdIndex; i++) {
    Serial.print(cmdBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // 命令解析结构
  if (cmdIndex > 0) {
    // 命令码在cmdBuffer[0]
    uint8_t cmdCode = cmdBuffer[0];
    
    // 命令数据从cmdBuffer[1]开始，长度为cmdIndex-1
    // 用户可以在此处自行实现命令处理逻辑
    
    // 示例：打印命令码
    char response[32];
    sprintf(response, "Command code: 0x%02X received", cmdCode);
    sendResponse(response);
  }
  
  // 重置命令缓冲区
  cmdIndex = 0;
}

// 发送响应数据
void sendResponse(const char* response) {
  Serial.write(CMD_START_BYTE);  // 发送起始字节
  Serial.print(response);        // 发送响应内容
  Serial.write(CMD_END_BYTE);    // 发送结束字节
  Serial.println();              // 额外的换行（方便阅读）
}

// 处理接收到的字节
bool handleReceivedByte(uint8_t byte) {
  // 检测命令起始字节
  if (byte == CMD_START_BYTE) {
    cmdStarted = true;
    cmdIndex = 0;
    return false;
  }
  
  // 如果命令已经开始，处理后续字节
  if (cmdStarted) {
    // 检测命令结束字节
    if (byte == CMD_END_BYTE) {
      cmdStarted = false;
      if (cmdIndex > 0) {
        return true;  // 命令接收完成
      }
      return false;   // 空命令，忽略
    }
    
    // 将字节添加到命令缓冲区
    if (cmdIndex < MAX_CMD_LEN - 1) {  // 保留1字节用于null终止符
      cmdBuffer[cmdIndex++] = byte;
    } else {
      // 命令太长，重置
      cmdStarted = false;
      cmdIndex = 0;
    }
  }
  
  return false;  // 命令未接收完成
}

// 初始化串口2
void initSerial2() {
  // 使用指定的引脚和波特率初始化串口2
  Serial2.begin(SERIAL2_BAUD, SERIAL_8N1, SERIAL2_RX_PIN, SERIAL2_TX_PIN);
  
  // 输出初始化信息
  Serial.println("Serial2 initialized for command communication");
}

// 处理串口2接收到的命令
void handleSerial2Commands() {
  // 检查串口2是否有可用数据
  while (Serial2.available() > 0) {
    // 读取一个字节数据
    uint8_t receivedByte = Serial2.read();
    
    // 处理接收到的字节，检查是否命令已完成
    if (handleSerial2ReceivedByte(receivedByte)) {
      // 如果命令接收完成，处理命令
      processSerial2Command();
    }
  }
}

// 处理串口2接收到的字节
bool handleSerial2ReceivedByte(uint8_t byte) {
  // 检测命令起始字节
  if (byte == CMD_START_BYTE) {
    serial2CmdStarted = true;
    serial2CmdIndex = 0;
    return false;
  }
  
  // 如果命令已经开始，处理后续字节
  if (serial2CmdStarted) {
    // 检测命令结束字节
    if (byte == CMD_END_BYTE) {
      serial2CmdStarted = false;
      if (serial2CmdIndex > 0) {
        return true;  // 命令接收完成
      }
      return false;   // 空命令，忽略
    }
    
    // 将字节添加到命令缓冲区
    if (serial2CmdIndex < MAX_CMD_LEN - 1) {  // 保留1字节用于null终止符
      serial2CmdBuffer[serial2CmdIndex++] = byte;
    } else {
      // 命令太长，重置
      serial2CmdStarted = false;
      serial2CmdIndex = 0;
    }
  }
  
  return false;  // 命令未接收完成
}

// 处理串口2接收到的命令
void processSerial2Command() {
  // 确保命令缓冲区以null结尾，便于字符串操作
  serial2CmdBuffer[serial2CmdIndex] = 0;
  
  // 打印接收到的命令（调试用）
  Serial.print("Serial2 received command: ");
  for (int i = 0; i < serial2CmdIndex; i++) {
    Serial.print((char)serial2CmdBuffer[i]);
  }
  Serial.println();
  
  // 命令解析处理
  if (serial2CmdIndex > 0) {
    // 检查是否是ASCII命令 "on"
    if (serial2CmdIndex == 2 && serial2CmdBuffer[0] == 'o' && serial2CmdBuffer[1] == 'n') {
      // 命令为"on"，打开WS2812灯珠
      setLEDState(true);  // 调用WS2812控制函数打开LED
      syncLEDWithHomeKit(true);  // 同步更新HomeKit状态为开启
      Serial.println("Serial2 command: Turn ON LED");
      
      // 发送响应
      sendSerial2Response("LED ON");
    }
    // 检查是否是ASCII命令 "off"
    else if (serial2CmdIndex == 3 && serial2CmdBuffer[0] == 'o' && serial2CmdBuffer[1] == 'f' && serial2CmdBuffer[2] == 'f') {
      // 命令为"off"，关闭WS2812灯珠
      setLEDState(false);  // 调用WS2812控制函数关闭LED
      syncLEDWithHomeKit(false);  // 同步更新HomeKit状态为关闭
      Serial.println("Serial2 command: Turn OFF LED");
      
      // 发送响应
      sendSerial2Response("LED OFF");
    }
    // 检查是否是ASCII命令 "up"
    else if (serial2CmdIndex == 2 && serial2CmdBuffer[0] == 'u' && serial2CmdBuffer[1] == 'p') {
      // 命令为"up"，增加LED亮度20%
      increaseLEDBrightness(20.0);  // 调用增加亮度函数
      Serial.println("Serial2 command: Increase brightness by 20%");
      
      // 发送响应
      sendSerial2Response("Brightness UP 20%");
    }
    // 检查是否是ASCII命令 "down"
    else if (serial2CmdIndex == 4 && serial2CmdBuffer[0] == 'd' && serial2CmdBuffer[1] == 'o' && 
             serial2CmdBuffer[2] == 'w' && serial2CmdBuffer[3] == 'n') {
      // 命令为"down"，减少LED亮度20%
      decreaseLEDBrightness(20.0);  // 调用减少亮度函数
      Serial.println("Serial2 command: Decrease brightness by 20%");
      
      // 发送响应
      sendSerial2Response("Brightness DOWN 20%");
    }
    // 其他命令处理
    else {
      Serial.println("Serial2 unknown command");
      
      // 发送未知命令响应
      sendSerial2Response("Unknown Command");
    }
  }
  
  // 重置命令缓冲区
  serial2CmdIndex = 0;
}

// 通过串口2发送响应数据
void sendSerial2Response(const char* response) {
  Serial2.write(CMD_START_BYTE);  // 发送起始字节
  Serial2.print(response);        // 发送响应内容
  Serial2.write(CMD_END_BYTE);    // 发送结束字节
  Serial2.println();              // 额外的换行（方便阅读）
}