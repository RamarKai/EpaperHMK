#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <BH1750.h>
#include <Wire.h>

class LightSensor {
public:
    // 构造函数,可选择I2C地址(默认0x23)
    LightSensor(uint8_t address = 0x23);
    
    // 初始化传感器
    // 返回值: 初始化成功返回true,失败返回false
    bool begin(TwoWire* wire = &Wire);
    
    // 读取光照强度
    // 返回值: 光照强度(单位:lux),-1表示读取失败
    float readLightLevel();
    
    // 设置测量模式
    // mode: 测量模式(详见BH1750.h中的Mode枚举)
    // 返回值: 设置成功返回true,失败返回false
    bool setMode(BH1750::Mode mode);
    
    // 设置测量时间寄存器值(可调整灵敏度)
    // mtreg: 测量时间寄存器值(31-254)
    // 返回值: 设置成功返回true,失败返回false
    bool setMTreg(uint8_t mtreg);
    
    // 检查是否可以进行新的测量
    // maxWait: 是否使用最大等待时间
    // 返回值: 可以测量返回true,否则返回false
    bool isReady(bool maxWait = false);

private:
    BH1750 sensor;        // BH1750传感器对象
    bool initialized;      // 初始化状态标志
};

#endif 