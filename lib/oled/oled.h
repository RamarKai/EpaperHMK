#ifndef OLED_H
#define OLED_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <FastLED.h>
#include "weather.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define OLED_SDA 21
#define OLED_SCL 22

#define PAGE_SWITCH_INTERVAL 5000

enum DisplayPage {
    SENSOR_PAGE = 0,
    WEATHER_PAGE = 1
};

extern Adafruit_SSD1306 display;
extern TwoWire I2C_OLED;
extern DisplayPage currentPage;

bool initOLED();

void updateDisplay();

void updateSensorPage();

void updateWeatherPage(const WeatherData &weather);

bool switchDisplayPage();

void showStartupScreen();

void showError(const char* errorMsg);

#endif