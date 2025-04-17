#include "key.h" // 包含按键模块头文件

// 定义全局变量
ButtonState button1State = BUTTON_RELEASED; // 按钮1的当前状态，初始为释放状态
ButtonState button2State = BUTTON_RELEASED; // 按钮2的当前状态，初始为释放状态

// 按钮去抖动相关变量
unsigned long lastDebounceTime1 = 0; // 按钮1最后一次状态改变的时间
unsigned long lastDebounceTime2 = 0; // 按钮2最后一次状态改变的时间
unsigned long debounceDelay = 50;    // 去抖动延迟时间（毫秒）

// 上一次读取的按钮状态
int lastButtonState1 = HIGH; // 按钮1上一次的状态
int lastButtonState2 = HIGH; // 按钮2上一次的状态

/**
 * @brief 初始化按钮
 *
 * 设置按钮引脚为输入模式，启用内部上拉电阻
 */
void initButtons()
{
    pinMode(BUTTON1_PIN, INPUT_PULLUP); // 设置按钮1引脚为输入模式，启用内部上拉电阻
    pinMode(BUTTON2_PIN, INPUT_PULLUP); // 设置按钮2引脚为输入模式，启用内部上拉电阻

    // 初始读取按钮状态
    lastButtonState1 = digitalRead(BUTTON1_PIN); // 读取按钮1的初始状态
    lastButtonState2 = digitalRead(BUTTON2_PIN); // 读取按钮2的初始状态

    Serial.println("Buttons initialized"); // 打印按钮初始化完成信息
}

/**
 * @brief 更新按钮状态
 *
 * 读取按钮状态并进行去抖动处理
 */
void updateButtonStates()
{
    // 读取当前按钮状态
    int reading1 = digitalRead(BUTTON1_PIN); // 读取按钮1当前状态
    int reading2 = digitalRead(BUTTON2_PIN); // 读取按钮2当前状态

    // 按钮1去抖动处理
    if (reading1 != lastButtonState1)
    {
        lastDebounceTime1 = millis(); // 记录状态变化时间
    }

    // 按钮2去抖动处理
    if (reading2 != lastButtonState2)
    {
        lastDebounceTime2 = millis(); // 记录状态变化时间
    }

    // 如果按钮1状态稳定超过去抖动时间，则更新按钮状态
    if ((millis() - lastDebounceTime1) > debounceDelay)
    {
        if (reading1 == LOW)
        { // 由于使用上拉电阻，LOW表示按下
            button1State = BUTTON_PRESSED;
        }
        else
        {
            button1State = BUTTON_RELEASED;
        }
    }

    // 如果按钮2状态稳定超过去抖动时间，则更新按钮状态
    if ((millis() - lastDebounceTime2) > debounceDelay)
    {
        if (reading2 == LOW)
        { // 由于使用上拉电阻，LOW表示按下
            button2State = BUTTON_PRESSED;
        }
        else
        {
            button2State = BUTTON_RELEASED;
        }
    }

    // 保存本次读取的状态
    lastButtonState1 = reading1;
    lastButtonState2 = reading2;
}

/**
 * @brief 获取按钮1的状态
 *
 * @return ButtonState 按钮1的当前状态
 */
ButtonState getButton1State()
{
    return button1State; // 返回按钮1的当前状态
}

/**
 * @brief 获取按钮2的状态
 *
 * @return ButtonState 按钮2的当前状态
 */
ButtonState getButton2State()
{
    return button2State; // 返回按钮2的当前状态
}

/**
 * @brief 检查按钮1是否被按下
 *
 * @return bool 如果按钮1被按下则返回true，否则返回false
 */
bool isButton1Pressed()
{
    return (button1State == BUTTON_PRESSED); // 检查按钮1是否处于按下状态
}

/**
 * @brief 检查按钮2是否被按下
 *
 * @return bool 如果按钮2被按下则返回true，否则返回false
 */
bool isButton2Pressed()
{
    return (button2State == BUTTON_PRESSED); // 检查按钮2是否处于按下状态
}
