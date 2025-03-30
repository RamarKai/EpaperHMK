# EpaperHMK - ESP32智能家居传感器系统

## 📝 项目概述

EpaperHMK是一个基于ESP32的智能家居传感器系统，集成了多种传感器和显示功能，并支持HomeKit集成，让您可以通过iPhone、iPad或Mac设备方便地监控和控制您的家居环境。

通过OLED显示屏，系统能够实时展示各种环境参数，包括温度、湿度、光照强度和空气质量。同时，系统还能通过WiFi连接获取天气预报信息，并与Apple HomeKit生态系统无缝集成。

## ✨ 主要特性

- **多传感器支持**：
  - DHT11 温湿度传感器
  - BH1750 光照强度传感器
  - MQ2气体传感器（检测空气质量）
  
- **实时显示系统**：
  - SSD1306 OLED 显示屏，实时展示传感器数据和天气信息
  - 自动页面切换功能
  
- **HomeKit集成**：
  - 通过HomeSpan库实现Apple HomeKit协议支持
  - 在苹果设备上监控所有传感器数据
  - 无需额外的Hub设备，ESP32直接与Apple设备通信
  
- **天气服务**：
  - 使用心知天气API自动获取天气信息
  - 显示当前天气状况、温度、湿度和风力信息
  
- **时间管理**：
  - NTP网络时间同步
  - 本地时间显示
  
- **WS2812B LED支持**：
  - 可控制彩色LED指示灯
  - 支持通过HomeKit控制亮度和颜色

- **语音播报功能**：
  - 通过串口2与次级单片机通信
  - 播报天气情况、温湿度等实时数据
  - 支持多种语音播报内容和场景

- **命令行界面**：
  - 通过串口2提供命令行接口，方便调试和控制

## 🛠️ 硬件需求

- ESP32开发板 (如ESP32-DOIT-DEVKIT-V1)
- DHT11温湿度传感器
- BH1750光照传感器
- MQ2气体传感器
- SSD1306 OLED显示屏 (I2C接口, 128x64像素)
- WS2812B LED灯带 (可选)
- 辅助单片机（声音模块）
- 面包板和连接线
- 5V电源适配器

### 接线图

| 传感器/模块 | ESP32 引脚 |
|------------|-----------|
| DHT11      | GPIO4     |
| BH1750     | SDA: GPIO21, SCL: GPIO22 |
| MQ2        | GPIO34    |
| SSD1306    | SDA: GPIO21, SCL: GPIO22 |
| WS2812B    | GPIO13    |
| 串口2-RX   | GPIO16    |
| 串口2-TX   | GPIO17    |

## 📚 软件依赖

- [PlatformIO](https://platformio.org/) 开发环境
- [Arduino 框架](https://www.arduino.cc/) 
- [HomeSpan](https://github.com/HomeSpan/HomeSpan) (用于Apple HomeKit集成)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) (用于OLED显示屏)
- [Adafruit DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library) (用于DHT11传感器)
- [ArduinoJson](https://arduinojson.org/) (用于解析JSON数据)
- [FastLED](https://github.com/FastLED/FastLED) (用于WS2812B LED控制)

## 🔧 安装指南

### 开发环境设置

1. 安装 [Visual Studio Code](https://code.visualstudio.com/)
2. 安装 [PlatformIO IDE Extension](https://platformio.org/platformio-ide)
3. 克隆本仓库到本地：
   ```
   git clone https://github.com/your-username/EpaperHMK-Cursor.git
   ```
4. 使用VS Code打开项目文件夹
5. 配置`platformio.ini`中的上传端口（如有必要）
6. 修改`lib/weather/weather.h`中的`WEATHER_API_KEY`和`WEATHER_LOCATION`为您自己的信息

### 编译与上传

使用PlatformIO IDE的工具栏按钮，或通过命令行：

```bash
# 编译
pio run

# 上传
pio run --target upload

# 监视串口输出
pio device monitor
```

## 📱 使用方法

### 初始配置

1. 上传固件后，ESP32将创建一个名为"ESP32 Sensors"的HomeKit配件
2. 在iPhone的"家庭"应用中选择"添加配件"
3. 扫描设备生成的HomeKit代码（默认代码参见HomeSpan文档）
4. 按照屏幕上的说明完成设置

### OLED显示界面

显示屏会自动在以下页面之间切换：

1. **传感器页面**：显示当前温度、湿度、光照和空气质量数据
2. **天气页面**：显示当前城市的天气信息，包括温度、湿度和天气状况

### 串口命令

通过串口2（默认波特率9600）可发送以下命令：

- `led:on` - 打开LED
- `led:off` - 关闭LED
- `led:color:R,G,B` - 设置LED颜色（RGB值范围0-255）
- `page:sensor` - 切换到传感器页面
- `page:weather` - 切换到天气页面
- `weather:update` - 手动更新天气数据
- `speak:temp` - 播报当前温度数据
- `speak:humidity` - 播报当前湿度数据
- `speak:weather` - 播报当前天气情况
- `speak:time` - 播报当前时间
- `speak:all` - 播报所有环境数据摘要

### 语音播报功能

系统通过串口2（GPIO16/17，9600波特率）与辅助单片机通信，用于语音播报关键信息。通信协议采用简单的起始/结束字节包装格式：

- 起始字节: 0xAA
- 结束字节: 0x55
- 消息格式: `<起始字节><命令内容><结束字节>`

语音播报功能可以在多种场景中使用，如：
- 定时播报室内温湿度数据
- 天气状况更新时播报当日天气预报
- 环境数据超过阈值时进行语音提醒
- 系统启动时播报欢迎信息和系统状态
- 接收到HomeKit指令时进行语音确认
- 通过命令手动触发特定信息的播报

## 📡 API接口

### 天气API

项目使用心知天气API获取天气数据。请在`lib/weather/weather.h`中替换`WEATHER_API_KEY`为您自己的API密钥。

```cpp
#define WEATHER_API_KEY "YOUR_API_KEY"  // 替换为您的心知天气API密钥
#define WEATHER_LOCATION "fujianfuzhou"  // 替换为您的城市编码
```

## 🧩 项目结构

```
EpaperHMK/
├── lib/                    # 项目库文件
│   ├── command/            # 命令处理模块
│   ├── dht11/              # DHT11温湿度传感器模块
│   ├── lightsensor/        # BH1750光线传感器模块
│   ├── mq2sensor/          # MQ2气体传感器模块
│   ├── oled/               # OLED显示屏模块
│   ├── time/               # 时间管理模块
│   ├── weather/            # 天气服务模块
│   └── ws2812/             # WS2812B LED控制模块
├── src/                    # 源代码
│   └── main.cpp            # 主程序入口
├── include/                # 包含文件
├── .gitignore              # Git忽略文件
└── platformio.ini          # PlatformIO配置文件
```

## 📝 许可证

MIT License

## 🙏 鸣谢

- [HomeSpan](https://github.com/HomeSpan/HomeSpan) - 提供HomeKit集成功能
- [Adafruit](https://github.com/adafruit) - 提供多种传感器库
- [心知天气](https://www.seniverse.com/) - 提供天气API服务

## 🔗 相关链接

- [ESP32官方文档](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [HomeSpan文档](https://github.com/HomeSpan/HomeSpan/wiki)
- [PlatformIO文档](https://docs.platformio.org/)

## 🤝 贡献

欢迎提交问题和改进建议！请随时创建Issue或Pull Request。

## 📞 联系方式

如有任何问题，请通过GitHub Issue与我联系。 