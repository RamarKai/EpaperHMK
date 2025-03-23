#include "oled.h"

TwoWire I2C_OLED = TwoWire(0);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET);

DisplayPage currentPage = SENSOR_PAGE;
unsigned long lastPageSwitchTime = 0;

extern float lightLevel;
extern bool bh1750_ok;
extern int gasLevel;
extern bool ledState;
extern CRGB currentColor;
extern WeatherData currentWeather;

bool initOLED() {
    I2C_OLED.begin(OLED_SDA, OLED_SCL);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    display.clearDisplay();
    display.display();
    Serial.println("OLED initialized");
    
    lastPageSwitchTime = millis();
    
    return true;
}

void updateDisplay() {
    switchDisplayPage();
    
    switch(currentPage) {
        case SENSOR_PAGE:
            updateSensorPage();
            break;
        case WEATHER_PAGE:
            updateWeatherPage(currentWeather);
            break;
    }
}

void updateSensorPage() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("-- SENSORS DATA --");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);
    
    display.setCursor(0, 12);
    display.print("Light: ");
    if(bh1750_ok) {
        display.print(lightLevel);
        display.println(" lx");
    } else {
        display.println("Error");
    }
    
    display.setCursor(0, 24);
    display.print("Gas: ");
    display.println(gasLevel);
    
    display.setCursor(0, 36);
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
    
    display.setCursor(0, 56);
    display.print("Page: 1/2");
    
    display.display();
}

void updateWeatherPage(const WeatherData &weather) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("-- WEATHER DATA --");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);
    
    if(weather.is_valid) {
        display.setCursor(0, 12);
        display.print("City: ");
        display.println(weather.city);
        
        display.setCursor(0, 22);
        display.print("Weather: ");
        display.println(weather.weather);
        
        display.setCursor(0, 32);
        display.print("Temp: ");
        display.print(weather.temperature);
        display.println(" C");
        
        display.setCursor(0, 42);
        display.print("Humidity: ");
        display.print(weather.humidity);
        display.println("%");
    } else {
        display.setTextSize(1);
        display.setCursor(0, 24);
        display.println("No valid weather data");
        display.setCursor(0, 36);
        display.println("Please check WiFi");
        display.setCursor(0, 46);
        display.println("and API key settings");
    }
    
    display.setCursor(0, 56);
    display.print("Page: 2/2");
    
    display.display();
}

bool switchDisplayPage() {
    unsigned long currentTime = millis();
    
    if(currentTime - lastPageSwitchTime >= PAGE_SWITCH_INTERVAL) {
        switch(currentPage) {
            case SENSOR_PAGE:
                currentPage = WEATHER_PAGE;
                break;
            case WEATHER_PAGE:
                currentPage = SENSOR_PAGE;
                break;
        }
        
        lastPageSwitchTime = currentTime;
        return true;
    }
    
    return false;
}

void showStartupScreen() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.println("ESP32");
    display.setCursor(10, 25);
    display.println("HomeKit");
    display.setTextSize(1);
    display.setCursor(10, 45);
    display.println("Initializing...");
    display.display();
    delay(2000);
}

void showError(const char* errorMsg) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("ERROR:");
    display.setCursor(0, 16);
    display.println(errorMsg);
    display.display();
}