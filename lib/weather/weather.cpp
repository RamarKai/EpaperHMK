#include "weather.h"

WeatherData currentWeather;

const char* WEATHER_API_ENDPOINT = "https://api.seniverse.com/v3/weather/now.json";

bool initWeatherService() {
    if (WEATHER_API_KEY[0] == '\0') {
        Serial.println("Weather API key or UID not set!");
        return false;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, weather service requires network access!");
        return false;
    }
    
    Serial.println("Weather service initialized");
    return true;
}

bool updateWeatherData() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, cannot update weather!");
        return false;
    }
    
    HTTPClient http;
    
    String url = String(WEATHER_API_ENDPOINT) + 
                 "?key=" + WEATHER_API_KEY + 
                 "&location=" + WEATHER_LOCATION + 
                 "&language=zh-Hans" +
                 "&unit=c";
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    http.begin(url);
    
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("API Response:");
        Serial.println(payload);
        
        JsonDocument doc;
        
        DeserializationError error = deserializeJson(doc, payload);
        
        if (error) {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }
        
        JsonObject results_0 = doc["results"][0];
        JsonObject now = results_0["now"];
        
        currentWeather.city = results_0["location"]["name"].as<String>();
        currentWeather.weather = now["text"].as<String>();
        currentWeather.temperature = now["temperature"].as<float>();
        currentWeather.humidity = now["humidity"].as<int>();
        currentWeather.wind_speed = now["wind_speed"].as<float>();
        currentWeather.wind_direction = now["wind_direction"].as<String>();
        currentWeather.is_valid = true;
        currentWeather.last_update = millis();
        
        http.end();
        Serial.println("Weather data updated successfully");
        return true;
    } else {
        Serial.print("HTTP request failed with error code: ");
        Serial.println(httpCode);
        http.end();
        return false;
    }
}

WeatherData getWeatherData() {
    return currentWeather;
}

bool checkAndUpdateWeather() {
    unsigned long currentTime = millis();
    
    if (!currentWeather.is_valid || 
        (currentTime - currentWeather.last_update > WEATHER_UPDATE_INTERVAL)) {
        return updateWeatherData();
    }
    
    return false;
}