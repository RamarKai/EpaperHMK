/**
 * @file lightsensor.cpp
 * @brief BH1750光照传感器相关功能实现
 * @details 包含BH1750传感器初始化、读取和HomeSpan服务实现
 */

#include "lightsensor.h"  // 包含对应的头文件

// 创建BH1750专用I2C实例
TwoWire I2C_BH1750 = TwoWire(1);  // BH1750使用的I2C，使用第二个I2C控制器

// 全局变量
float lightLevel = 0;    // 光照强度全局变量，用于在不同组件间共享光照数据
bool bh1750_ok = false; // BH1750状态标志，指示传感器是否正常工作

/**
 * @brief 初始化BH1750光照传感器
 * @details 配置I2C接口并设置BH1750为连续高分辨率模式
 * @return 无返回值，但会设置全局的bh1750_ok标志
 */
void initBH1750() {
    // 初始化BH1750专用I2C总线
    I2C_BH1750.begin(BH1750_SDA, BH1750_SCL);
    
    // 初始化BH1750传感器
    I2C_BH1750.beginTransmission(0x23); // 0x23是BH1750的默认I2C地址
    I2C_BH1750.write(0x01);  // 发送上电指令
    if(I2C_BH1750.endTransmission() == 0) {
        delay(10); // 短暂延时等待传感器稳定
        I2C_BH1750.beginTransmission(0x23);
        I2C_BH1750.write(0x10);  // 设置连续高分辨率模式（1lx分辨率）
        if(I2C_BH1750.endTransmission() == 0) {
            Serial.println("BH1750 initialized successfully");
            bh1750_ok = true; // 标记传感器就绪
        } else {
            Serial.println("Error configuring BH1750");
            bh1750_ok = false;
        }
    } else {
        Serial.println("Error initializing BH1750");
        bh1750_ok = false;
    }
}

/**
 * @brief 读取BH1750传感器的光照强度
 * @details 通过I2C读取BH1750传感器数据并转换为光照强度（lux）
 * @return float 返回光照强度值（lux），出错时返回-1
 */
float readBH1750() {
    uint16_t value = 0;
    
    // 开始传输，设置测量模式
    I2C_BH1750.beginTransmission(0x23);  // BH1750默认地址
    I2C_BH1750.write(0x10);  // 连续高分辨率模式，1lx分辨率
    if(I2C_BH1750.endTransmission() != 0) {
        return -1; // 通信错误，返回错误值
    }
    
    delay(120);  // 等待测量完成，高分辨率模式需要约120ms
    
    // 读取两个字节的测量结果
    if(I2C_BH1750.requestFrom(0x23, 2) == 2) {
        // 读取高字节
        value = I2C_BH1750.read();
        value <<= 8; // 左移8位，为低字节留出空间
        // 读取低字节并组合
        value |= I2C_BH1750.read();
        // 根据BH1750数据手册，将原始数据除以1.2得到光照强度（lux）
        float lux = value / 1.2;
        return lux;
    }
    return -1; // 读取失败，返回错误值
}

/**
 * @brief DEV_LightSensor构造函数实现
 * @details 初始化光照传感器服务
 */
DEV_LightSensor::DEV_LightSensor() : Service::LightSensor() {
    lightLevel = new Characteristic::CurrentAmbientLightLevel(0.0);  // 创建光照级别特征
    lightLevel->setRange(0, 100000);     // 设置范围，支持0-100000 lux
}

/**
 * @brief DEV_LightSensor loop函数实现
 * @details 每秒读取一次光照数据，并在数值变化时更新HomeKit特征值
 */
void DEV_LightSensor::loop() {
    if(!bh1750_ok) return;  // 如果传感器未就绪，直接返回
    
    // 每1秒读取一次，避免过于频繁地读取影响性能
    unsigned long currentTime = millis();
    if(currentTime - lastReadTime >= 1000) {
        float lux = readBH1750();  // 读取光照强度
        if(lux >= 0) {  // 只处理有效值（非错误返回）
            if(lux != lightLevel->getVal()) {
                lightLevel->setVal(lux);      // 更新HomeKit特征值
                ::lightLevel = lux;           // 更新全局变量，用于OLED显示等
            }
        }
        lastReadTime = currentTime; // 更新最后读取时间
    }
}
