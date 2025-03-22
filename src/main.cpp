#include "HomeSpan.h"
#include "PinConfig.h"
#include "OLED_Display.h"
#include "BH1750_Sensor.h"
#include "MQ2_Sensor.h"
#include "WS2812B_LED.h"
#include "LED_Service.h"
#include "LightSensor_Service.h"
#include "AirQuality_Service.h"

// 创建两个I2C实例
TwoWire I2C_OLED = TwoWire(0);    // OLED使用的I2C
TwoWire I2C_BH1750 = TwoWire(1);  // BH1750使用的I2C

// 创建对象
OLED_Display display;
BH1750_Sensor lightSensor;
MQ2_Sensor gasSensor;
WS2812B_LED led;

// 全局变量
float lightLevel = 0;    // 光照强度
int gasLevel = 0;       // 气体浓度
bool ledState = false;   // LED状态
bool bh1750_ok = false; // BH1750状态标志
CRGB currentColor = CRGB::Black;  // 当前LED颜色

// 手动实现BH1750读取
float readBH1750() {
    uint16_t value = 0;
    I2C_BH1750.beginTransmission(0x23);  // BH1750默认地址
    I2C_BH1750.write(0x10);  // 连续高分辨率模式
    if(I2C_BH1750.endTransmission() != 0) {
        return -1;
    }
    
    delay(120);  // 等待测量完成
    
    if(I2C_BH1750.requestFrom(0x23, 2) == 2) {
        value = I2C_BH1750.read();
        value <<= 8;
        value |= I2C_BH1750.read();
        float lux = value / 1.2;
        return lux;
    }
    return -1;
}

// WS2812B LED控制类
struct DEV_WS2812B : Service::LightBulb {
    SpanCharacteristic *power;              // 开关特征
    SpanCharacteristic *brightness;         // 亮度特征
    SpanCharacteristic *hue;                // 色调特征
    SpanCharacteristic *saturation;         // 饱和度特征
    
    DEV_WS2812B(WS2812B_LED& led) : Service::LightBulb() {
        power = new Characteristic::On();                    // 创建开关特征
        brightness = new Characteristic::Brightness(100);    // 创建亮度特征
        brightness->setRange(0, 100);                       // 设置亮度范围
        hue = new Characteristic::Hue(0);                   // 创建色调特征
        saturation = new Characteristic::Saturation(0);     // 创建饱和度特征
    }

    boolean update() {
        boolean isOn = power->getNewVal();
        
        if(isOn) {
            // 获取HSV值
            float h = hue->getNewVal();
            float s = saturation->getNewVal();
            float v = brightness->getNewVal();
            
            // 将HSV转换为RGB
            CHSV hsv(h * 255/360, s * 255/100, v * 255/100);
            hsv2rgb_rainbow(hsv, currentColor);
        } else {
            currentColor = CRGB::Black;
        }
        
        // 更新LED显示
        led.setColor(currentColor);
        ledState = isOn;  // 更新全局状态
        return(true);
    }
};

// 光照传感器类
struct DEV_LightSensor : Service::LightSensor {
    SpanCharacteristic *lightLevel;          // 光照级别特征
    unsigned long lastReadTime = 0;          // 上次读取时间

    DEV_LightSensor(BH1750_Sensor& sensor) : Service::LightSensor() {
        lightLevel = new Characteristic::CurrentAmbientLightLevel(0.0);  // 创建光照级别特征
        lightLevel->setRange(0, 100000);     // 设置范围
    }

    void loop() {
        if(!bh1750_ok) return;  // 如果传感器未就绪，直接返回
        
        // 每1秒读取一次
        unsigned long currentTime = millis();
        if(currentTime - lastReadTime >= 1000) {
            float lux = readBH1750();  // 读取光照强度
            if(lux >= 0) {  // 只处理有效值
                if(lux != lightLevel->getVal()) {
                    lightLevel->setVal(lux);      // 更新特征值
                    ::lightLevel = lux;           // 更新全局变量
                }
            }
            lastReadTime = currentTime;
        }
    }
};

// 空气质量传感器类
struct DEV_AirQualitySensor : Service::AirQualitySensor {
    SpanCharacteristic *airQuality;          // 空气质量特征

    DEV_AirQualitySensor(MQ2_Sensor& sensor) : Service::AirQualitySensor() {
        airQuality = new Characteristic::AirQuality(0);  // 创建空气质量特征
    }

    void loop() {
        int reading = analogRead(MQ2_PIN);    // 读取模拟值
        gasLevel = reading;                   // 更新全局变量
        
        // 根据气体浓度确定空气质量等级
        int quality;
        if(reading < 1000) quality = 1;      // EXCELLENT
        else if(reading < 2000) quality = 2;  // GOOD
        else if(reading < 3000) quality = 3;  // FAIR
        else if(reading < 4000) quality = 4;  // INFERIOR
        else quality = 5;                     // POOR
        
        if(quality != airQuality->getVal())
            airQuality->setVal(quality);      // 更新特征值
    }
};

void setup() {
    // 初始化串口通信
    Serial.begin(115200);
    Serial.println("Starting setup...");

    // 初始化两个I2C总线
    I2C_OLED.begin(OLED_SDA, OLED_SCL);
    I2C_BH1750.begin(BH1750_SDA, BH1750_SCL);
    
    // 初始化OLED
    if(!display.begin()) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    Serial.println("OLED initialized");
    
    // 初始化BH1750
    if(lightSensor.begin()) {
        Serial.println("BH1750 initialized successfully");
        bh1750_ok = true;
    } else {
        Serial.println("Error initializing BH1750");
        bh1750_ok = false;
    }
    
    // 初始化MQ2
    gasSensor.begin();
    Serial.println("MQ2 initialized");
    
    // 初始化WS2812B
    led.begin();
    Serial.println("WS2812B initialized");
    
    // 初始化GPIO
    pinMode(MQ2_PIN, INPUT);
    
    // 初始化HomeSpan
    homeSpan.begin(Category::Lighting, "ESP32 Sensors");

    // 创建主配件
    new SpanAccessory();                                                          
        new Service::AccessoryInformation();
            new Characteristic::Identify();                
            new Characteristic::Name("ESP32 Sensors");    
            new Characteristic::Manufacturer("DIY");      
            new Characteristic::SerialNumber("123-ABC");  
            new Characteristic::Model("ESP32");          
            new Characteristic::FirmwareRevision("1.0"); 
            
        new DEV_WS2812B();                  // 添加WS2812B LED服务
        new DEV_LightSensor();              // 添加光照传感器服务
        new DEV_AirQualitySensor();         // 添加空气质量传感器服务
}

void loop() {
    homeSpan.poll();                        // HomeSpan主循环
    display.updateDisplay(lightLevel, gasLevel, ledState, currentColor.r, currentColor.g, currentColor.b);    // 更新显示
    delay(100);                            // 短暂延时以避免过于频繁刷新
}
