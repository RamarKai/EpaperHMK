#include "command.h"
#include "ws2812.h"          // 包含WS2812控制相关头文件
#include "time_manager.h"    // 包含时间管理模块头文件
#include "weather.h"         // 包含天气服务模块头文件
#include "lightsensor.h"     // 包含光线传感器模块头文件
#include "mq2sensor.h"       // 包含MQ2气体传感器模块头文件

// 定义命令缓冲区
static CommandBuffer serial2Cmd = {0};

// 命令处理回调数组
static const struct {
    const char* cmdPrefix;           // 命令前缀
    CommandHandlerCallback handler;  // 处理函数
} cmdHandlers[] = {
    {"on", handleLEDCommand},        // LED开启命令
    {"off", handleLEDCommand},       // LED关闭命令
    {"up", handleLEDCommand},        // LED亮度增加命令
    {"down", handleLEDCommand},      // LED亮度减少命令
    {"time", handleTimeWeatherCommand}, // 时间查询命令
    {"weather", handleTimeWeatherCommand}, // 天气查询命令
    {"temp", handleSensorCommand},   // 温度查询命令
    {"hum", handleSensorCommand},    // 湿度查询命令
    {"light", handleSensorCommand},  // 光照查询命令
    {"air", handleSensorCommand},    // 空气质量查询命令
};

// 初始化串口2
void initSerial2() {
    // 使用指定的引脚和波特率初始化串口2
    Serial2.begin(SERIAL2_BAUD, SERIAL_8N1, SERIAL2_RX_PIN, SERIAL2_TX_PIN);
    
    // 输出初始化信息
    Serial.println("Serial2 initialized for command communication");
    
    // 初始化命令缓冲区
    serial2Cmd.index = 0;
    serial2Cmd.started = false;
}

// 处理串口2接收到的命令
void handleSerial2Commands() {
    // 检查串口2是否有可用数据
    while (Serial2.available() > 0) {
        // 读取一个字节数据
        uint8_t receivedByte = Serial2.read();
        
        // 检测命令起始字节
        if (receivedByte == CMD_START_BYTE) {
            serial2Cmd.started = true;
            serial2Cmd.index = 0;
            continue;
        }
        
        // 检测命令结束字节
        if (receivedByte == CMD_END_BYTE) {
            if (serial2Cmd.started && serial2Cmd.index > 0) {
                // 确保命令缓冲区以null结尾，便于字符串操作
                serial2Cmd.buffer[serial2Cmd.index] = 0;
                
                // 打印接收到的命令（调试用）
                Serial.print("Serial2 received command: ");
                Serial.println((char*)serial2Cmd.buffer);
                
                // 处理命令
                processSerialCommand((char*)serial2Cmd.buffer, serial2Cmd.index);
                
                // 重置命令缓冲区状态
                serial2Cmd.started = false;
                serial2Cmd.index = 0;
            }
            continue;
        }
        
        // 将字节添加到命令缓冲区
        if (serial2Cmd.started && serial2Cmd.index < MAX_CMD_LEN - 1) {
            serial2Cmd.buffer[serial2Cmd.index++] = receivedByte;
        }
    }
}

// 通过串口2发送响应数据
void sendSerial2Response(const char* response) {
    Serial2.write(CMD_START_BYTE);  // 发送起始字节
    Serial2.print(response);        // 发送响应内容
    Serial2.write(CMD_END_BYTE);    // 发送结束字节
    Serial2.println();              // 额外的换行（方便阅读）
}

// 处理接收到的命令
void processSerialCommand(const char* cmd, size_t len) {
    // 遍历命令处理回调数组，查找匹配的命令
    for (size_t i = 0; i < sizeof(cmdHandlers) / sizeof(cmdHandlers[0]); i++) {
        size_t prefixLen = strlen(cmdHandlers[i].cmdPrefix);
        
        // 检查命令前缀是否匹配
        if (len >= prefixLen && strncmp(cmd, cmdHandlers[i].cmdPrefix, prefixLen) == 0) {
            // 调用对应的处理函数
            cmdHandlers[i].handler(cmd, len);
            return;
        }
    }
    
    // 未找到匹配的命令处理函数
    Serial.println("Unknown command");
    sendSerial2Response("Unknown Command");
}

// 处理LED相关命令
void handleLEDCommand(const char* cmd, size_t len) {
    // 检查是否是 "on" 命令
    if (strncmp(cmd, "on", len) == 0) {
        // 命令为"on"，打开WS2812灯珠
        setLEDState(true);  // 调用WS2812控制函数打开LED
        syncLEDWithHomeKit(true);  // 同步更新HomeKit状态为开启
        Serial.println("Serial2 command: Turn ON LED");
        
        // 发送响应
        sendSerial2Response("LED ON");
    }
    // 检查是否是 "off" 命令
    else if (strncmp(cmd, "off", len) == 0) {
        // 命令为"off"，关闭WS2812灯珠
        setLEDState(false);  // 调用WS2812控制函数关闭LED
        syncLEDWithHomeKit(false);  // 同步更新HomeKit状态为关闭
        Serial.println("Serial2 command: Turn OFF LED");
        
        // 发送响应
        sendSerial2Response("LED OFF");
    }
    // 检查是否是 "up" 命令
    else if (strncmp(cmd, "up", len) == 0) {
        // 命令为"up"，增加LED亮度20%
        increaseLEDBrightness(20.0);  // 调用增加亮度函数
        Serial.println("Serial2 command: Increase brightness by 20%");
        
        // 发送响应
        sendSerial2Response("Brightness UP 20%");
    }
    // 检查是否是 "down" 命令
    else if (strncmp(cmd, "down", len) == 0) {
        // 命令为"down"，减少LED亮度20%
        decreaseLEDBrightness(20.0);  // 调用减少亮度函数
        Serial.println("Serial2 command: Decrease brightness by 20%");
        
        // 发送响应
        sendSerial2Response("Brightness DOWN 20%");
    }
}

// 处理时间和天气相关命令
void handleTimeWeatherCommand(const char* cmd, size_t len) {
    // 检查是否是 "time" 命令
    if (strncmp(cmd, "time", len) == 0) {
        // 命令为"time"，获取当前时间
        Serial.println("Serial2 command: Get current time");
        
        // 获取当前时间并发送响应
        const char* timeResponse = handleTimeCommand();
        sendSerial2Response(timeResponse);
    }
    // 检查是否是 "weather" 命令
    else if (strncmp(cmd, "weather", len) == 0) {
        // 命令为"weather"，获取当前天气信息
        Serial.println("Serial2 command: Get weather information");
        
        // 获取天气数据
        WeatherData weather = getWeatherData();
        
        // 检查天气数据是否有效
        if (weather.is_valid) {
            // 格式化天气信息
            char weatherResponse[128];
            sprintf(weatherResponse, "City: %s, Weather: %s, Temp: %.1fC, Humidity: %d%%", 
                    weather.city.c_str(), weather.weather.c_str(), 
                    weather.temperature, weather.humidity);
            
            // 发送天气响应
            sendSerial2Response(weatherResponse);
        } else {
            // 天气数据无效
            sendSerial2Response("Weather data not available");
        }
    }
}

// 处理传感器相关命令
void handleSensorCommand(const char* cmd, size_t len) {
    // 检查是否是 "temp" 命令
    if (strncmp(cmd, "temp", len) == 0) {
        // 命令为"temp"，获取当前温度信息
        Serial.println("Serial2 command: Get temperature information");
        
        // 获取天气数据中的温度信息
        WeatherData weather = getWeatherData();
        
        // 检查天气数据是否有效
        if (weather.is_valid) {
            // 格式化温度信息
            char tempResponse[64];
            sprintf(tempResponse, "Temperature: %.1f°C", weather.temperature);
            
            // 发送温度响应
            sendSerial2Response(tempResponse);
        } else {
            // 天气数据无效
            sendSerial2Response("Temperature data not available");
        }
    }
    // 检查是否是 "hum" 命令
    else if (strncmp(cmd, "hum", len) == 0) {
        // 命令为"hum"，获取当前湿度信息
        Serial.println("Serial2 command: Get humidity information");
        
        // 获取天气数据中的湿度信息
        WeatherData weather = getWeatherData();
        
        // 检查天气数据是否有效
        if (weather.is_valid) {
            // 格式化湿度信息
            char humResponse[64];
            sprintf(humResponse, "Humidity: %d%%", weather.humidity);
            
            // 发送湿度响应
            sendSerial2Response(humResponse);
        } else {
            // 天气数据无效
            sendSerial2Response("Humidity data not available");
        }
    }
    // 检查是否是 "light" 命令
    else if (strncmp(cmd, "light", len) == 0) {
        // 命令为"light"，获取当前光强信息
        Serial.println("Serial2 command: Get light level information");
        
        // 检查BH1750传感器是否正常
        if (bh1750_ok) {
            // 读取当前光照强度
            float currentLight = readBH1750();
            
            // 格式化光照强度信息
            char lightResponse[64];
            sprintf(lightResponse, "Light Level: %.2f lux", currentLight);
            
            // 发送光照强度响应
            sendSerial2Response(lightResponse);
        } else {
            // 光照传感器异常
            sendSerial2Response("Light sensor not available");
        }
    }
    // 检查是否是 "air" 命令
    else if (strncmp(cmd, "air", len) == 0) {
        // 命令为"air"，获取当前空气质量信息
        Serial.println("Serial2 command: Get air quality information");
        
        // 读取MQ2传感器数据
        int rawGasLevel = readMQ2Sensor();
        
        // 将原始数据转换为空气质量等级
        int airQualityLevel = convertToAirQuality(rawGasLevel);
        
        // 获取空气质量描述
        const char* airQualityDesc;
        switch (airQualityLevel) {
            case 1: airQualityDesc = "Excellent"; break;
            case 2: airQualityDesc = "Good"; break;
            case 3: airQualityDesc = "Fair"; break;
            case 4: airQualityDesc = "Inferior"; break;
            case 5: airQualityDesc = "Poor"; break;
            default: airQualityDesc = "Unknown";
        }
        
        // 格式化空气质量信息
        char airResponse[64];
        sprintf(airResponse, "Air Quality: Level %d (%s), Raw: %d", 
                airQualityLevel, airQualityDesc, rawGasLevel);
        
        // 发送空气质量响应
        sendSerial2Response(airResponse);
    }
}