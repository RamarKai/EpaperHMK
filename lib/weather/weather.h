#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WEATHER_API_KEY "SWL_pIpiyT3moGyF5"

#define WEATHER_UPDATE_INTERVAL 600000

#define WEATHER_LOCATION "fujianfuzhou"

struct WeatherData {
    String city;
    String weather;
    float temperature;
    int humidity;
    float wind_speed;
    String wind_direction;
    bool is_valid;
    unsigned long last_update;

    WeatherData() : 
        city("Unknown"), 
        weather("Unknown"), 
        temperature(0), 
        humidity(0), 
        wind_speed(0), 
        wind_direction("Unknown"), 
        is_valid(false), 
        last_update(0) {}
};

bool initWeatherService();

bool updateWeatherData();

WeatherData getWeatherData();

bool checkAndUpdateWeather();

extern WeatherData currentWeather;

#endif 