#include "dht11.h"   // 包含DHT11传感器的头文件
#include <Arduino.h> // 包含Arduino核心库

DHT dht(DHT_PIN, DHT_TYPE); // 创建DHT对象，设置引脚和传感器类型
float temperature = 0.0;    // 初始化温度变量为0.0
float humidity = 0.0;       // 初始化湿度变量为0.0
bool dht11_ok = false;      // 初始化DHT11状态变量为false，表示尚未初始化或工作不正常

bool initDHT11()
{                // 初始化DHT11传感器的函数实现
    dht.begin(); // 开始DHT传感器
    delay(2000); // 等待2秒钟，确保传感器稳定

    // 读取一次数据，检查传感器是否工作正常
    float h = dht.readHumidity();    // 读取湿度
    float t = dht.readTemperature(); // 读取温度

    // 检查读取的数据是否为有效数值
    if (isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!"); // 打印错误信息
        dht11_ok = false;                                  // 设置状态为不正常
        return false;                                      // 返回初始化失败
    }

    humidity = h;                                     // 更新湿度变量
    temperature = t;                                  // 更新温度变量
    dht11_ok = true;                                  // 设置状态为正常
    Serial.println("DHT11 initialized successfully"); // 打印成功信息
    return true;                                      // 返回初始化成功
}

float readTemperature()
{ // 读取DHT11传感器的温度数据的函数实现
    if (!dht11_ok)
    {                  // 如果传感器状态不正常
        return -999.0; // 返回一个明显的错误值
    }

    float t = dht.readTemperature(); // 读取温度

    if (isnan(t))
    {                                                                  // 如果读取的数据不是有效数值
        Serial.println("Failed to read temperature from DHT sensor!"); // 打印错误信息
        dht11_ok = false;                                              // 设置状态为不正常
        return -999.0;                                                 // 返回错误值
    }

    temperature = t;    // 更新温度变量
    return temperature; // 返回温度值
}

float readHumidity()
{ // 读取DHT11传感器的湿度数据的函数实现
    if (!dht11_ok)
    {                  // 如果传感器状态不正常
        return -999.0; // 返回一个明显的错误值
    }

    float h = dht.readHumidity(); // 读取湿度

    if (isnan(h))
    {                                                               // 如果读取的数据不是有效数值
        Serial.println("Failed to read humidity from DHT sensor!"); // 打印错误信息
        dht11_ok = false;                                           // 设置状态为不正常
        return -999.0;                                              // 返回错误值
    }

    humidity = h;    // 更新湿度变量
    return humidity; // 返回湿度值
}

bool isDHT11Working()
{ // 检查DHT11传感器是否正常工作的函数实现
    // 确保DHT传感器对象已初始化且传感器物理连接正常
    // 添加额外的安全检查，避免可能的崩溃
    if (!dht11_ok)
    {
        // 如果传感器已知状态不正常，直接返回false，不尝试读取
        return false;
    }

    // 注意：即使传感器状态标记为正常，也可能存在物理连接问题
    // 使用try-catch块捕获可能的异常（虽然Arduino通常不支持异常处理，
    // 但添加这些代码作为一种保护措施，防止未来可能的异常处理支持）

    // 小心地尝试读取数据，检查传感器是否工作正常
    float h = dht.readHumidity();    // 读取湿度
    float t = dht.readTemperature(); // 读取温度

    // 检查读取的数据是否为有效数值
    if (isnan(h) || isnan(t))
    {
        dht11_ok = false; // 设置状态为不正常
        return false;     // 返回传感器不正常
    }

    humidity = h;    // 更新湿度变量
    temperature = t; // 更新温度变量
    dht11_ok = true; // 设置状态为正常
    return true;     // 返回传感器正常
}

// 温度传感器服务的构造函数实现
DEV_TemperatureSensor::DEV_TemperatureSensor() : Service::TemperatureSensor()
{
    // 创建当前温度特性，初始值为0，最小值-50，最大值100，步长0.1
    currentTemperature = new Characteristic::CurrentTemperature(0);
    currentTemperature->setRange(-50, 100); // 设置温度范围
    currentTemperature->setUnit(0);         // 设置单位为摄氏度

    Serial.println("HomeKit Temperature Sensor Service initialized"); // 打印初始化成功信息
}

// 温度传感器服务的循环函数实现
void DEV_TemperatureSensor::loop()
{
    // 只有当DHT11传感器初始化成功并且工作正常时，才更新温度值
    if (dht11_ok)
    {
        if (isDHT11Working())
        {
            currentTemperature->setVal(temperature); // 更新HomeKit温度值
        }
    }
}

// 湿度传感器服务的构造函数实现
DEV_HumiditySensor::DEV_HumiditySensor() : Service::HumiditySensor()
{
    // 创建当前相对湿度特性，初始值为0，最小值0，最大值100，步长0.1
    currentHumidity = new Characteristic::CurrentRelativeHumidity(0);

    Serial.println("HomeKit Humidity Sensor Service initialized"); // 打印初始化成功信息
}

// 湿度传感器服务的循环函数实现
void DEV_HumiditySensor::loop()
{
    // 只有当DHT11传感器初始化成功并且工作正常时，才更新湿度值
    if (dht11_ok)
    {
        if (isDHT11Working())
        {
            currentHumidity->setVal(humidity); // 更新HomeKit湿度值
        }
    }
}