#include "lightsensor.h"               // 包含光线传感器的头文件

TwoWire I2C_BH1750 = TwoWire(1);    // 创建TwoWire对象，指定使用ESP32的第二个I2C控制器

float lightLevel = 0;                // 定义全局变量存储光照强度值，初始化为0
bool bh1750_ok = false;              // 定义标志变量表示BH1750传感器状态，初始化为false

void initBH1750() {                  // BH1750传感器初始化函数
    I2C_BH1750.begin(BH1750_SDA, BH1750_SCL);  // 初始化I2C总线，设置SDA和SCL引脚
    
    I2C_BH1750.beginTransmission(0x23);        // 开始与地址为0x23的BH1750通信
    I2C_BH1750.write(0x01);                    // 发送传感器复位命令
    if(I2C_BH1750.endTransmission() == 0) {    // 结束传输并检查是否成功
        delay(10);                             // 等待10毫秒让传感器复位
        I2C_BH1750.beginTransmission(0x23);    // 再次开始与BH1750通信
        I2C_BH1750.write(0x10);                // 发送配置命令：连续高分辨率模式
        if(I2C_BH1750.endTransmission() == 0) {  // 结束传输并检查是否成功
            Serial.println("BH1750 initialized successfully");  // 打印初始化成功信息
            bh1750_ok = true;                  // 设置传感器状态为正常
        } else {
            Serial.println("Error configuring BH1750");  // 打印配置错误信息
            bh1750_ok = false;                 // 设置传感器状态为异常
        }
    } else {
        Serial.println("Error initializing BH1750");  // 打印初始化错误信息
        bh1750_ok = false;                     // 设置传感器状态为异常
    }
}

float readBH1750() {                  // 读取BH1750传感器数据的函数
    uint16_t value = 0;               // 定义变量存储原始数据
    
    I2C_BH1750.beginTransmission(0x23);  // 开始与BH1750通信
    I2C_BH1750.write(0x10);              // 发送测量命令：连续高分辨率模式
    if(I2C_BH1750.endTransmission() != 0) {  // 结束传输并检查是否失败
        return -1;                        // 通信失败则返回-1
    }
    
    delay(120);                           // 等待120毫秒让传感器完成测量
    
    if(I2C_BH1750.requestFrom(0x23, 2) == 2) {  // 从BH1750请求2字节数据并检查是否成功
        value = I2C_BH1750.read();        // 读取高字节
        value <<= 8;                      // 左移8位准备合并低字节
        value |= I2C_BH1750.read();       // 读取并合并低字节
        float lux = value / 1.2;          // 根据BH1750数据手册计算光照强度(lux)
        return lux;                       // 返回光照强度值
    }
    return -1;                            // 读取失败则返回-1
}

DEV_LightSensor::DEV_LightSensor() : Service::LightSensor() {  // LightSensor服务的构造函数
    lightLevel = new Characteristic::CurrentAmbientLightLevel(0.0);  // 创建光照强度特性并设置初始值为0
    lightLevel->setRange(0, 100000);      // 设置光照强度的范围为0-100000 lux
}

void DEV_LightSensor::loop() {            // 周期性执行的循环函数
    if(!bh1750_ok) return;                // 如果传感器不正常则直接返回
    
    unsigned long currentTime = millis();  // 获取当前时间
    if(currentTime - lastReadTime >= 1000) {  // 检查是否距离上次读取已经过去了1秒
        float lux = readBH1750();          // 读取传感器值
        if(lux >= 0) {                     // 检查读取是否成功
            if(lux != lightLevel->getVal()) {  // 检查值是否发生变化
                lightLevel->setVal(lux);    // 更新HomeKit的光照强度特性值
                ::lightLevel = lux;         // 更新全局变量的光照强度值
            }
        }
        lastReadTime = currentTime;        // 更新上次读取时间
    }
}
