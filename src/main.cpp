#include "HomeSpan.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

// OLED显示屏配置
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// 引脚定义
#define LED_PIN 2        // WS2812B LED连接引脚
#define NUM_LEDS 1      // WS2812B LED数量
#define MQ2_PIN 34      // MQ2气体传感器模拟输入引脚
#define OLED_SDA 21     // OLED的SDA引脚
#define OLED_SCL 22     // OLED的SCL引脚
#define BH1750_SDA 4    // BH1750的SDA引脚
#define BH1750_SCL 5    // BH1750的SCL引脚

// 创建两个I2C实例
TwoWire I2C_OLED = TwoWire(0);    // OLED使用的I2C
TwoWire I2C_BH1750 = TwoWire(1);  // BH1750使用的I2C

// 创建对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET); // OLED显示屏对象
CRGB leds[NUM_LEDS];    // WS2812B LED对象

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

// I2C扫描函数
void scanI2CDevices() {
    // 扫描OLED的I2C总线
    Serial.println("Scanning OLED I2C bus...");
    byte error, address;
    int nDevices = 0;
    
    for(address = 1; address < 127; address++) {
        I2C_OLED.beginTransmission(address);
        error = I2C_OLED.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            nDevices++;
        }
    }
    
    // 扫描BH1750的I2C总线
    Serial.println("Scanning BH1750 I2C bus...");
    nDevices = 0;
    
    for(address = 1; address < 127; address++) {
        I2C_BH1750.beginTransmission(address);
        error = I2C_BH1750.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            nDevices++;
        }
    }
}

// WS2812B LED控制类
struct DEV_WS2812B : Service::LightBulb {
    SpanCharacteristic *power;              // 开关特征
    SpanCharacteristic *brightness;         // 亮度特征
    SpanCharacteristic *hue;                // 色调特征
    SpanCharacteristic *saturation;         // 饱和度特征
    
    DEV_WS2812B() : Service::LightBulb() {
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
        leds[0] = currentColor;
        FastLED.show();
        
        ledState = isOn;  // 更新全局状态
        return(true);
    }
};

// 光照传感器类
struct DEV_LightSensor : Service::LightSensor {
    SpanCharacteristic *lightLevel;          // 光照级别特征
    unsigned long lastReadTime = 0;          // 上次读取时间

    DEV_LightSensor() : Service::LightSensor() {
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

    DEV_AirQualitySensor() : Service::AirQualitySensor() {
        airQuality = new Characteristic::AirQuality(0);  // 创建空气质量特征
    }

    void loop() {
        int reading = analogRead(MQ2_PIN);    // 读取模拟值
        gasLevel = reading;                   // 更新全局变量
        
        // 根据气体浓度确定空气质量等级
        int quality;
        if(reading < 1500) quality = 1;      // EXCELLENT
        else if(reading < 2000) quality = 2;  // GOOD
        else if(reading < 3000) quality = 3;  // FAIR
        else if(reading < 4000) quality = 4;  // INFERIOR
        else quality = 5;                     // POOR
        
        if(quality != airQuality->getVal())
            airQuality->setVal(quality);      // 更新特征值
    }
};

// 更新OLED显示
void updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // 显示光照强度
    display.setCursor(0,0);
    display.print("Light: ");
    if(bh1750_ok) {
        display.print(lightLevel);
        display.println(" lx");
    } else {
        display.println("Error");
    }
    
    // 显示气体浓度
    display.setCursor(0,16);
    display.print("Gas: ");
    display.println(gasLevel);
    
    // 显示LED状态
    display.setCursor(0,32);
    display.print("LED: ");
    if(ledState) {
        display.print("ON (RGB:");
        display.print(currentColor.r);
        display.print(",");
        display.print(currentColor.g);
        display.print(",");
        display.print(currentColor.b);
        display.println(")");
    } else {
        display.println("OFF");
    }
    
    display.display();
}

void setup() {
    // 初始化串口通信
    Serial.begin(115200);
    Serial.println("Starting setup...");

    // 初始化两个I2C总线
    I2C_OLED.begin(OLED_SDA, OLED_SCL);
    I2C_BH1750.begin(BH1750_SDA, BH1750_SCL);
    
    scanI2CDevices();  // 扫描I2C设备
    
    // 初始化OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.clearDisplay();
    display.display();
    Serial.println("OLED initialized");
    
    // 初始化BH1750
    I2C_BH1750.beginTransmission(0x23);
    I2C_BH1750.write(0x01);  // 上电
    if(I2C_BH1750.endTransmission() == 0) {
        delay(10);
        I2C_BH1750.beginTransmission(0x23);
        I2C_BH1750.write(0x10);  // 设置连续高分辨率模式
        if(I2C_BH1750.endTransmission() == 0) {
            Serial.println("BH1750 initialized successfully");
            bh1750_ok = true;
        } else {
            Serial.println("Error configuring BH1750");
            bh1750_ok = false;
        }
    } else {
        Serial.println("Error initializing BH1750");
        bh1750_ok = false;
    }
    
    // 初始化WS2812B
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255);
    FastLED.show();
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
    updateDisplay();                        // 更新显示
    delay(100);                            // 短暂延时以避免过于频繁刷新
}
