#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

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

// I2C地址
#define BH1750_ADDR 0x23  // BH1750默认地址

#endif // PIN_CONFIG_H 