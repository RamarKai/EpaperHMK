#include "weather.h" // 包含天气服务的头文件

WeatherData currentWeather; // 定义全局变量存储当前天气数据

const char *WEATHER_API_ENDPOINT = "https://api.seniverse.com/v3/weather/now.json"; // 定义心知天气API接口地址

// 心知天气API服务器根证书 - DigiCert全球根证书
const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
    "-----END CERTIFICATE-----\n";

bool initWeatherService()
{ // 初始化天气服务的函数
    if (WEATHER_API_KEY[0] == '\0')
    {                                                      // 检查API密钥是否为空
        Serial.println("Weather API key or UID not set!"); // 如果API密钥为空则打印错误信息
        return false;                                      // 返回false表示初始化失败
    }

    if (WiFi.status() != WL_CONNECTED)
    {                                                                                   // 检查WiFi是否已连接
        Serial.println("WiFi not connected, weather service requires network access!"); // 如果WiFi未连接则打印错误信息
        return false;                                                                   // 返回false表示初始化失败
    }

    Serial.println("Weather service initialized"); // 打印初始化成功信息
    return true;                                   // 返回true表示初始化成功
}

bool updateWeatherData()
{ // 更新天气数据的函数
    if (WiFi.status() != WL_CONNECTED)
    {                                                                 // 检查WiFi是否已连接
        Serial.println("WiFi not connected, cannot update weather!"); // 如果WiFi未连接则打印错误信息
        return false;                                                 // 返回false表示更新失败
    }

    WiFiClientSecure *client = new WiFiClientSecure; // 创建安全WiFi客户端对象
    if (client)
    {
        // 设置根证书
        client->setCACert(rootCACertificate);

        // 设置连接超时时间
        client->setTimeout(15000); // 15秒

        HTTPClient https; // 创建HTTPS客户端对象

        String url = String(WEATHER_API_ENDPOINT) +
                     "?key=" + WEATHER_API_KEY +
                     "&location=" + WEATHER_LOCATION +
                     "&language=zh-Hans" +
                     "&unit=c"; // 构建完整的API请求URL，包含API密钥、位置等参数

        Serial.print("Requesting URL: "); // 打印将要请求的URL信息
        Serial.println(url);              // 打印完整的请求URL

        Serial.print("Connecting to ");
        Serial.println(WEATHER_API_ENDPOINT);

        if (https.begin(*client, url))
        { // 使用安全客户端初始化HTTPS请求
            // 开始连接
            int httpCode = https.GET(); // 发送GET请求并获取响应码

            if (httpCode == HTTP_CODE_OK)
            {                                       // 如果HTTP请求成功(返回200)
                String payload = https.getString(); // 获取响应内容
                Serial.println("API Response:");    // 打印响应标题
                Serial.println(payload);            // 打印完整的响应内容

                JsonDocument doc; // 创建JSON文档对象

                DeserializationError error = deserializeJson(doc, payload); // 解析JSON响应

                if (error)
                {                                          // 如果JSON解析失败
                    Serial.print("JSON parsing failed: "); // 打印JSON解析失败信息
                    Serial.println(error.c_str());         // 打印具体的错误信息
                    https.end();                           // 结束HTTP会话
                    delete client;
                    return false; // 返回false表示更新失败
                }

                JsonObject results_0 = doc["results"][0]; // 获取结果数组的第一个元素
                JsonObject now = results_0["now"];        // 获取当前天气数据对象

                currentWeather.city = results_0["location"]["name"].as<String>();   // 提取并保存城市名称
                currentWeather.weather = now["text"].as<String>();                  // 提取并保存天气情况
                currentWeather.temperature = now["temperature"].as<float>();        // 提取并保存温度
                currentWeather.humidity = now["humidity"].as<int>();                // 提取并保存湿度
                currentWeather.wind_speed = now["wind_speed"].as<float>();          // 提取并保存风速
                currentWeather.wind_direction = now["wind_direction"].as<String>(); // 提取并保存风向
                currentWeather.is_valid = true;                                     // 标记数据为有效
                currentWeather.last_update = millis();                              // 记录更新时间

                https.end(); // 结束HTTPS会话
                delete client;
                Serial.println("Weather data updated successfully"); // 打印更新成功信息
                return true;                                         // 返回true表示更新成功
            }
            else
            {                                                          // 如果HTTP请求失败
                Serial.print("HTTP request failed with error code: "); // 打印HTTP请求失败信息
                Serial.println(httpCode);                              // 打印HTTP错误码
                https.end();                                           // 结束HTTP会话
                delete client;
                return false; // 返回false表示更新失败
            }
        }
        else
        {
            Serial.println("Unable to connect to server");
            delete client;
            return false;
        }
    }
    else
    {
        Serial.println("Unable to create secure client");
        return false;
    }
}

WeatherData getWeatherData()
{                          // 获取天气数据的函数
    return currentWeather; // 返回当前天气数据
}

bool checkAndUpdateWeather()
{                                         // 检查并更新天气数据的函数
    unsigned long currentTime = millis(); // 获取当前时间

    if (!currentWeather.is_valid || // 如果天气数据无效
        (currentTime - currentWeather.last_update > WEATHER_UPDATE_INTERVAL))
    {                               // 或者距离上次更新已经超过了更新间隔
        return updateWeatherData(); // 更新天气数据并返回更新结果
    }

    return false; // 如果不需要更新则返回false
}