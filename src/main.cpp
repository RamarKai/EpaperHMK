#include "HomeSpan.h"
#include "lightsensor.h"
#include "mq2sensor.h"
#include "oled.h"
#include "ws2812.h"
#include "weather.h"

unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 100;

unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 300000;

bool weatherServiceInitialized = false;
bool weatherInitialUpdateDone = false;
bool wifiPreviouslyConnected = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting setup...");
    
    if(!initOLED()) {
        Serial.println("OLED initialization failed!");
        for(;;);
    }
    
    showStartupScreen();
    
    initBH1750();
    
    initMQ2Sensor();
    
    initWS2812B();
    
    homeSpan.begin(Category::Lighting, "ESP32 Sensors");

    new SpanAccessory();                                                          
        new Service::AccessoryInformation();
            new Characteristic::Identify();                
            new Characteristic::Name("ESP32 Sensors");    
            new Characteristic::Manufacturer("DIY");      
            new Characteristic::SerialNumber("123-ABC");  
            new Characteristic::Model("ESP32");          
            new Characteristic::FirmwareRevision("1.0"); 
            
        new DEV_WS2812B();
        new DEV_LightSensor();
        new DEV_AirQualitySensor();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("HomeSpan Starting...");
    display.setCursor(0, 16);
    display.println("Waiting for WiFi...");
    display.display();
    
    lastDisplayUpdate = millis();
    lastWeatherUpdate = millis();
}

void loop() {
    homeSpan.poll();
    
    unsigned long currentMillis = millis();
    
    if(currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        updateDisplay();
        lastDisplayUpdate = currentMillis;
    }
    
    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    
    if (wifiConnected && !wifiPreviouslyConnected) {
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("WiFi Connected!");
        display.setCursor(0, 16);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.display();
        delay(2000);
        
        if (!weatherServiceInitialized) {
            if (initWeatherService()) {
                Serial.println("Weather service initialized");
                weatherServiceInitialized = true;
                
            } else {
                Serial.println("Weather service initialization failed");
                showError("Weather service init failed");
                delay(2000);
            }
        }
    }
    else if (!wifiConnected && wifiPreviouslyConnected) {
        Serial.println("WiFi disconnected!");
    }
    
    wifiPreviouslyConnected = wifiConnected;
    
    if (wifiConnected && weatherServiceInitialized) {
        if (!weatherInitialUpdateDone) {
            Serial.println("Performing initial weather data update...");
            if (updateWeatherData()) {
                Serial.println("Initial weather data updated successfully");
                weatherInitialUpdateDone = true;
            } else {
                Serial.println("Failed to get initial weather data");
            }
            lastWeatherUpdate = currentMillis;
        }
        else if (currentMillis - lastWeatherUpdate >= weatherUpdateInterval) {
            Serial.println("Updating weather data...");
            if (checkAndUpdateWeather()) {
                Serial.println("Weather data updated successfully");
            } else {
                Serial.println("Weather data update failed");
            }
            lastWeatherUpdate = currentMillis;
        }
    }
}
