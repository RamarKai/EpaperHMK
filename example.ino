// 头文件，使用函数的前提
#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>

#include "DHT.h"
#include <NeoPixelBus.h>

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

// 宏定义，定义各类变量等
#define DHTPIN 2      // 定义传感器接口
#define DHTTYPE DHT11 // 定义传感器类型
DHT dht(DHTPIN, DHTTYPE);

#define ENABLE_GxEPD2_GFX 0 // 使能GFX
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

const uint16_t PixelCount = 1; // 定义灯珠数量
const uint8_t PixelPin = 4;    // 定义灯珠接口
#define colorSaturation 128
// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);   // 定义红色
RgbColor green(0, colorSaturation, 0); // 定义绿色
RgbColor blue(0, 0, colorSaturation);  // 定义蓝色
RgbColor white(colorSaturation);       // 定义白色
RgbColor black(0);                     // 定义黑色

// 初始化时间参数
const char *ntpServer = "pool.ntp.org"; // 网络时间服务器
const long gmtOffset_sec = 8 * 3600;    // 时区设置函数，东八区（UTC/GMT+8:00）写成8*3600
const int daylightOffset_sec = 0;       // 夏令时填写3600，否则填0

// 天气参数
const char *host = "api.seniverse.com";
// 新建URL请求
String url = "/v3/weather/now.json?key=SWL_pIpiyT3moGyF5&location=zhejianghangzhou&language=zh-Hans&unit=c";
const String WDAY_NAMES[] = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

// 定义需要显示的内容
int h, t;
String weather, temperature, hour_min, year_mon_day; // 天气 温度 时间 年月日
String book1 = "天行健";
String book2 = "君子以自强不息";

// 函数声明
void WiFi_Connect();
void get_json();
void get_dht();
void get_tim();
void ws();

// 初始化
void setup()
{
    Serial.begin(115200); // 串口初始化
    delay(100);

    Serial.println();              // 串口打印初始化进程
    Serial.print("Connecting.. "); // 连接WIFI中
    WiFi_Connect();
    Serial.println("WiFi connected"); // 连接成功
    Serial.println("IP address: ");   // 打印IP
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // RTC时钟保持时间更新

    Serial.println("setup");              // 墨水屏初始化
    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    delay(500);
    display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    u8g2Fonts.begin(display);             // u8g2初始化
    delay(100);

    strip.Begin(); // LED初始化
    strip.Show();
    ws(); // LED演示

    Serial.println("setup done"); // 串口打印所有初始化结束
}

// 主循环
void loop()
{
    get_json();
    get_tim();
    get_dht();

    u8g2Fonts.setFontMode(1);                  // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(1);             // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color

    u8g2Fonts.setFont(u8g2_font_logisoso46_tf);
    display.setPartialWindow(24, 144, 64, 136); // 58:133
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        u8g2Fonts.setCursor(24, 144);
        u8g2Fonts.print(hour_min);
    } while (display.nextPage());

    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
    display.setPartialWindow(92, 144, 16, 144); // 12:136
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        u8g2Fonts.setCursor(92, 144);
        u8g2Fonts.print(year_mon_day);
    } while (display.nextPage());

    u8g2Fonts.setForegroundColor(GxEPD_WHITE); // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_BLACK); // apply Adafruit GFX color

    u8g2Fonts.setFont(u8g2_font_wqy14_t_gb2312);
    display.setPartialWindow(0, 0, 128, 128);
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_BLACK);

        u8g2Fonts.setCursor(100, 8);
        u8g2Fonts.print(weather);
        u8g2Fonts.setCursor(64, 8);
        u8g2Fonts.print(temperature);
        u8g2Fonts.setCursor(24, 8);
        u8g2Fonts.print(book1);
        u8g2Fonts.setCursor(8, 8);
        u8g2Fonts.print(book2);
    } while (display.nextPage());

    delay(8000);
}

//////////////////////////////////////////////////////////////////////////////////
// WiFi的初始化和连接
void WiFi_Connect()
{
    WiFi.begin("Epaper", "opopoopp");
    while (WiFi.status() != WL_CONNECTED)
    { // 这里是阻塞程序，直到连接成功
        delay(300);
        Serial.print(".");
    }
}

void get_json() // json获取心知天气api
{
    // 获取天气信息
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        return;
    }

    // 发送请求至服务器
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    delay(1000);

    // 获取JSON数据
    String answer;
    while (client.available())
    {
        String line = client.readStringUntil('\r');
        answer += line;
    }

    // 断开服务
    client.stop();
    Serial.println();
    Serial.println("closing connection");

    // 提取JSON
    String jsonAnswer;
    int jsonIndex;
    for (int i = 0; i < answer.length(); i++)
    {
        if (answer[i] == '{')
        {
            jsonIndex = i;
            break;
        }
    }
    jsonAnswer = answer.substring(jsonIndex);

    // 分析JSON
    // Stream& input;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonAnswer);

    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    JsonObject results_0 = doc["results"][0];

    JsonObject results_0_location = results_0["location"];
    const char *results_0_location_id = results_0_location["id"];                           // "WSSU6EXX52RE"
    const char *results_0_location_name = results_0_location["name"];                       // "福州"
    const char *results_0_location_country = results_0_location["country"];                 // "CN"
    const char *results_0_location_path = results_0_location["path"];                       // "福州,福州,福建,中国"
    const char *results_0_location_timezone = results_0_location["timezone"];               // "Asia/Shanghai"
    const char *results_0_location_timezone_offset = results_0_location["timezone_offset"]; // "+08:00"

    JsonObject results_0_now = results_0["now"];
    const char *results_0_now_text = results_0_now["text"];               // "多云"
    const char *results_0_now_code = results_0_now["code"];               // "4"
    const char *results_0_now_temperature = results_0_now["temperature"]; // "29"
    const char *results_0_last_update = results_0["last_update"];

    weather = String(results_0_location_name);
    weather += " ";
    weather += (results_0_now_text);
    weather += " ";

    temperature = String("外");
    temperature += results_0_now_temperature;
    temperature += "°C";
    temperature += " ";
}

void get_dht() // 获取传感器数据
{
    h = dht.readHumidity();    // 获取湿度
    t = dht.readTemperature(); // 获取温度

    weather += h;
    weather += "%RH";

    temperature += "内";
    temperature += t;
    temperature += "°C";
}

void get_tim() // 获取时间
{
    struct tm timeinfo; //
    if (!getLocalTime(&timeinfo))
    { // 一定要加这个条件判断，否则内存溢出
        Serial.println("Failed to obtain time");
        return;
    }

    hour_min = String(timeinfo.tm_hour);
    hour_min += ":";
    hour_min += (timeinfo.tm_min);

    year_mon_day = String(timeinfo.tm_year + 1900);
    year_mon_day += "年";
    year_mon_day += String(timeinfo.tm_mon + 1);
    year_mon_day += "月";
    year_mon_day += String(timeinfo.tm_mday);
    year_mon_day += "日";
    year_mon_day += " ";
    year_mon_day += WDAY_NAMES[timeinfo.tm_wday].c_str();
}

void ws() // led演示
{
    strip.SetPixelColor(0, red);
    strip.Show();
    delay(1000);
    strip.SetPixelColor(0, green);
    strip.Show();
    delay(1000);
    strip.SetPixelColor(0, blue);
    strip.Show();
    delay(1000);
    strip.SetPixelColor(0, black);
    strip.Show();
}
