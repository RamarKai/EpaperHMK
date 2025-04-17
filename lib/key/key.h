#ifndef KEY_H // 防止头文件被重复包含的宏定义开始
#define KEY_H // 定义宏以标记该头文件已被包含

#include <Arduino.h> // 包含Arduino基础库

// 按钮引脚定义
#define BUTTON1_PIN 13 // 定义按钮1连接的GPIO引脚，改为GPIO13，避免与MQ2传感器冲突
#define BUTTON2_PIN 14 // 定义按钮2连接的GPIO引脚，改为GPIO14，选择未使用的引脚

// 定义按钮状态
typedef enum
{
    BUTTON_RELEASED = 0, // 按钮释放状态
    BUTTON_PRESSED = 1   // 按钮按下状态
} ButtonState;

// 函数声明
void initButtons();            // 初始化按钮
ButtonState getButton1State(); // 获取按钮1的状态
ButtonState getButton2State(); // 获取按钮2的状态
bool isButton1Pressed();       // 检查按钮1是否被按下
bool isButton2Pressed();       // 检查按钮2是否被按下
void updateButtonStates();     // 更新按钮状态

// 声明外部变量
extern ButtonState button1State; // 按钮1的当前状态
extern ButtonState button2State; // 按钮2的当前状态

#endif // 防止头文件被重复包含的宏定义结束
