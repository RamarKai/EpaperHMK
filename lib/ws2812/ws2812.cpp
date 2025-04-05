#include "ws2812.h" // 包含WS2812B LED控制的头文件

CRGB leds[NUM_LEDS]; // 定义LED数组，用于存储LED的颜色数据

bool ledState = false;           // 定义LED状态变量，初始为关闭
CRGB currentColor = CRGB::Black; // 定义当前颜色变量，初始为黑色(关闭)

// LED设备实例，用于在其他模块中访问和更新HomeKit状态
DEV_WS2812B *ws2812Device = nullptr;

void initWS2812B()
{                                                           // 初始化WS2812B LED的函数
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS); // 配置FastLED，指定LED类型、引脚和颜色顺序
    FastLED.setBrightness(255);                             // 设置LED亮度为最大值255
    FastLED.show();                                         // 显示初始状态（全黑）
    Serial.println("WS2812B initialized");                  // 打印初始化成功信息
}

void setLEDColor(CRGB color, bool update)
{                         // 设置LED颜色的函数
    currentColor = color; // 更新当前颜色变量
    leds[0] = color;      // 设置第一个LED的颜色
    if (update)
    {                   // 如果需要立即更新
        FastLED.show(); // 显示更新后的颜色
    }
    ledState = (color != CRGB::Black); // 如果颜色不是黑色，则LED状态为开启
}

void setLEDState(bool on, bool update)
{ // 设置LED开关状态的函数
    if (on)
    { // 如果要开启LED
        if (currentColor == CRGB::Black)
        {                               // 如果当前颜色是黑色(关闭状态)
            currentColor = CRGB::White; // 默认使用白色
        }
        leds[0] = currentColor; // 设置LED颜色为当前颜色
    }
    else
    {                          // 如果要关闭LED
        leds[0] = CRGB::Black; // 设置LED颜色为黑色(关闭)
    }

    if (update)
    {                   // 如果需要立即更新
        FastLED.show(); // 显示更新后的状态
    }

    ledState = on; // 更新LED状态变量
}

// 与HomeKit同步LED状态的函数
void syncLEDWithHomeKit(bool on)
{
    // 确保ws2812Device及其特性已完全初始化
    if (ws2812Device == nullptr)
    {
        Serial.println("Error: WS2812B device not initialized");
        return;
    }

    if (ws2812Device->power == nullptr)
    {
        Serial.println("Error: Power characteristic not initialized");
        return;
    }

    ws2812Device->power->setVal(on);                 // 更新HomeKit的电源状态
    Serial.print("Synced LED state with HomeKit: "); // 打印同步状态信息
    Serial.println(on ? "ON" : "OFF");               // 打印开/关状态
}

// 增加LED亮度的函数
void increaseLEDBrightness(float percentage)
{
    // 确保LED已开启且ws2812Device及其特性已完全初始化
    if (!ledState)
    {
        Serial.println("Cannot increase brightness: LED is off");
        return;
    }

    if (ws2812Device == nullptr)
    {
        Serial.println("Cannot increase brightness: Device not initialized");
        return;
    }

    if (ws2812Device->brightness == nullptr)
    {
        Serial.println("Cannot increase brightness: Brightness characteristic not initialized");
        return;
    }

    // 获取当前亮度
    float currentBrightness = ws2812Device->brightness->getVal();

    // 计算新亮度值（增加指定百分比）
    float newBrightness = currentBrightness + (percentage);

    // 确保亮度不超过100%
    if (newBrightness > 100.0)
    {
        newBrightness = 100.0;
    }

    // 更新亮度
    syncLEDBrightnessWithHomeKit(newBrightness);

    // 调整HSV值并应用到LED
    updateLEDWithCurrentHomeKitValues();

    Serial.print("Increased brightness to: ");
    Serial.println(newBrightness);
}

// 减少LED亮度的函数
void decreaseLEDBrightness(float percentage)
{
    // 确保LED已开启且ws2812Device及其特性已完全初始化
    if (!ledState)
    {
        Serial.println("Cannot decrease brightness: LED is off");
        return;
    }

    if (ws2812Device == nullptr)
    {
        Serial.println("Cannot decrease brightness: Device not initialized");
        return;
    }

    if (ws2812Device->brightness == nullptr)
    {
        Serial.println("Cannot decrease brightness: Brightness characteristic not initialized");
        return;
    }

    // 获取当前亮度
    float currentBrightness = ws2812Device->brightness->getVal();

    // 计算新亮度值（减少指定百分比）
    float newBrightness = currentBrightness - (percentage);

    // 确保亮度不小于1%
    if (newBrightness < 1.0)
    {
        newBrightness = 1.0;
    }

    // 更新亮度
    syncLEDBrightnessWithHomeKit(newBrightness);

    // 调整HSV值并应用到LED
    updateLEDWithCurrentHomeKitValues();

    Serial.print("Decreased brightness to: ");
    Serial.println(newBrightness);
}

// 与HomeKit同步LED亮度的函数
void syncLEDBrightnessWithHomeKit(float brightness)
{
    // 确保ws2812Device及其特性已完全初始化
    if (ws2812Device == nullptr)
    {
        Serial.println("Error: WS2812B device not initialized");
        return;
    }

    if (ws2812Device->brightness == nullptr)
    {
        Serial.println("Error: Brightness characteristic not initialized");
        return;
    }

    ws2812Device->brightness->setVal(brightness); // 更新HomeKit的亮度状态
    Serial.print("Synced LED brightness with HomeKit: ");
    Serial.println(brightness);
}

// 使用当前HomeKit的HSV值更新LED的函数
void updateLEDWithCurrentHomeKitValues()
{
    // 确保ws2812Device及其所有特性已完全初始化
    if (ws2812Device == nullptr)
    {
        Serial.println("Cannot update LED: Device not initialized");
        return;
    }

    if (ws2812Device->hue == nullptr ||
        ws2812Device->saturation == nullptr ||
        ws2812Device->brightness == nullptr)
    {
        Serial.println("Cannot update LED: Characteristics not initialized");
        return;
    }

    // 获取当前HomeKit的HSV值
    float h = ws2812Device->hue->getVal();
    float s = ws2812Device->saturation->getVal();
    float v = ws2812Device->brightness->getVal();

    // 将HSV值转换为FastLED使用的范围(0-255)
    CHSV hsv(h * 255 / 360, s * 255 / 100, v * 255 / 100);

    // 将HSV颜色转换为RGB颜色并存储
    hsv2rgb_rainbow(hsv, currentColor);

    // 更新LED颜色
    leds[0] = currentColor;
    FastLED.show();
}

DEV_WS2812B::DEV_WS2812B() : Service::LightBulb()
{                                     // DEV_WS2812B类的构造函数
    power = new Characteristic::On(); // 创建开关特性

    brightness = new Characteristic::Brightness(100); // 创建亮度特性，初始值为100%
    brightness->setRange(0, 100);                     // 设置亮度范围为0-100%

    hue = new Characteristic::Hue(0); // 创建色调特性，初始值为0度

    saturation = new Characteristic::Saturation(0); // 创建饱和度特性，初始值为0%

    // 保存设备引用
    ws2812Device = this;                             // 将this指针保存到全局变量
    Serial.println("WS2812B device instance saved"); // 打印保存成功信息
}

boolean DEV_WS2812B::update()
{                                      // 当HomeKit发送控制命令时的更新函数
    boolean isOn = power->getNewVal(); // 获取新的开关状态

    if (isOn)
    {                                      // 如果要开启LED
        float h = hue->getNewVal();        // 获取新的色调值(0-360度)
        float s = saturation->getNewVal(); // 获取新的饱和度值(0-100%)
        float v = brightness->getNewVal(); // 获取新的亮度值(0-100%)

        CHSV hsv(h * 255 / 360, s * 255 / 100, v * 255 / 100); // 将HSV值转换为FastLED使用的范围(0-255)
        hsv2rgb_rainbow(hsv, currentColor);                    // 将HSV颜色转换为RGB颜色并存储
    }
    else
    {                               // 如果要关闭LED
        currentColor = CRGB::Black; // 设置当前颜色为黑色
    }

    leds[0] = currentColor; // 更新LED颜色
    FastLED.show();         // 显示更新后的颜色

    ledState = isOn; // 更新LED状态变量

    return (true); // 返回true表示更新成功
}