# ESP32 E-Paper HMK 智能家居监控系统

## 项目概述

ESP32 E-Paper HMK是一个基于ESP32的智能家居环境监控与控制系统，集成多种传感器和显示模块，可实时监测室内环境参数并通过Apple HomeKit进行智能控制。本系统采用低功耗设计，特别是结合E-Paper墨水屏的使用，非常适合长时间稳定运行的场景。

系统核心使用ESP32微控制器，搭载双核处理器和内置Wi-Fi/蓝牙功能，提供强大的计算能力和网络连接能力。通过多种传感器采集环境数据，并在OLED和E-Paper墨水屏上直观显示，同时支持通过HomeKit平台远程监控和控制，真正实现智能家居的互联互通。

### 应用场景

- **家庭环境监测**：实时监控室内温湿度、空气质量和光照强度
- **智能家居控制中心**：通过HomeKit连接其他智能设备
- **天气信息站**：显示当前及未来天气预报
- **物联网教学演示**：适合学习ESP32、传感器应用和物联网技术

![ESP32 E-Paper HMK系统](系统实物图.jpg)

## 主要功能

### 1. 环境监测功能

- **温湿度监测 (DHT11)**
  - 测量范围：温度0-50℃（±2℃），湿度20-90%RH（±5%RH）
  - 采样频率：每2秒更新一次
  - 数据处理：支持数据平滑处理，减少波动
  
- **环境光照强度监测 (BH1750)**
  - 测量范围：1-65535 lux
  - 高精度：分辨率可达1 lux
  - 自动模式切换：支持高精度和高速两种测量模式
  
- **空气质量监测 (MQ2)**
  - 检测对象：烟雾、丙烷、丁烷、甲烷、酒精、氢气等
  - 警报阈值：可自定义危险阈值
  - 实时响应：气体浓度变化快速反应

### 2. 显示系统

- **OLED显示屏 (SSD1306)**
  - 分辨率：128x64像素，单色显示
  - 信息显示：实时环境数据、系统状态、设置菜单等
  - 刷新率：快速刷新，无延迟显示
  - 界面设计：多页面轮播，清晰直观
  
- **E-Paper墨水屏**
  - 低功耗：仅在内容变化时消耗能量
  - 信息持久性：断电后仍保持显示内容
  - 显示内容：日期时间、天气预报、环境数据汇总
  - 刷新间隔：默认30秒，可根据需要调整

### 3. 智能互联功能

- **Apple HomeKit集成**
  - 无需额外网关：直接与iOS设备连接
  - 加密通信：采用HomeKit安全标准
  - 自动化支持：可设置基于传感器数据的自动化规则
  - 远程访问：通过Home应用远程查看状态和控制设备
  
- **WiFi连接功能**
  - 自动重连：网络断开后自动尝试重新连接
  - OTA更新：支持通过WiFi网络更新固件
  - 网络时间同步：通过NTP服务保持准确时间
  - 网络状态指示：通过OLED和LED指示网络连接状态
  
- **串口通信接口**
  - 命令控制：支持通过串口发送控制命令
  - 数据输出：可通过串口获取传感器数据
  - 调试信息：提供系统运行状态和诊断信息
  - 外部设备联动：可与其他微控制器或设备通信

### 4. 其他辅助功能

- **天气预报服务**
  - 数据来源：接入免费天气API
  - 预报内容：当前天气、温度、湿度、气压及未来预报
  - 自动更新：默认每5分钟自动更新数据
  - 节能请求：仅在WiFi已连接状态下更新
  
- **RGB指示灯 (WS2812B)**
  - 空气质量指示：颜色变化直观反映空气质量
  - 系统状态指示：反映系统不同工作状态
  - 可自定义模式：支持多种颜色和动画效果
  - 低光害设计：夜间自动降低亮度
  
- **时间管理系统**
  - NTP同步：通过网络自动校准时间
  - 本地时钟：WiFi断开时继续保持时间计数
  - 时区支持：可设置本地时区
  - 格式化显示：按照用户习惯格式化时间显示

## 硬件需求

### 主要组件详细规格

1. **ESP32开发板 (ESP32-DOIT DevKit V1)**
   - 微处理器：Tensilica LX6双核，主频高达240MHz
   - 内存：520KB SRAM
   - 存储：集成4MB闪存
   - 无线连接：WiFi 802.11 b/g/n + Bluetooth 4.2
   - 供电：USB供电或3.3V直流输入
   - 尺寸：51mm x 28mm

2. **SSD1306 OLED显示屏**
   - 分辨率：128x64像素
   - 接口：I2C (可选SPI)
   - 供电：3.3V
   - 显示颜色：单色（白色）
   - 视角：>160°
   - 控制器：SSD1306
   - I2C地址：0x3C (默认)

3. **E-Paper墨水屏**
   - 接口：SPI
   - 分辨率：取决于具体型号(常见2.9" 296x128像素)
   - 颜色：黑白（可选三色）
   - 供电：3.3V
   - 刷新时间：约2-3秒(全刷)
   - 功耗：刷新时约20mA，待机时<0.01mA

4. **BH1750环境光传感器**
   - 接口：I2C
   - 测量范围：1-65535 lux
   - 分辨率：1 lux
   - 供电：3.3V-5V
   - 功耗：<0.2mA
   - I2C地址：0x23 (默认)

5. **MQ2气体传感器**
   - 接口：模拟输出
   - 检测气体：烟雾、LPG、丙烷、甲烷、酒精、氢气
   - 供电：5V
   - 预热时间：约24小时(首次)，约10分钟(常规)
   - 工作温度：-10℃至50℃
   - 灵敏度：可调节（通过电位器）

6. **WS2812B RGB LED**
   - 控制方式：单线数字控制
   - 电压：5V
   - 每LED功耗：约60mA（全亮白色时）
   - 更新频率：800kHz
   - 颜色范围：16777216色(24位)
   - 可级联：是(本项目仅使用1个)

7. **DHT11温湿度传感器**
   - 接口：单总线数字接口
   - 测量范围：温度0-50℃，湿度20-90%RH
   - 精度：温度±2℃，湿度±5%RH
   - 供电：3.3V-5V
   - 采样率：1次/秒以下
   - 尺寸：15.5mm x 12mm x 5.5mm

### 连接配件

- **面包板或PCB板**
  - 推荐：标准830孔面包板或定制PCB
  - 连接线：多种颜色杜邦线(公对公、公对母、母对母)
  - 建议PCB尺寸：至少80mm x 60mm

- **电源系统**
  - 主电源：5V/2A USB电源适配器
  - 可选备用电源：18650锂电池(3.7V)+升压模块
  - 电源滤波：100μF电解电容 + 0.1μF陶瓷电容
  - 均流元件：建议在WS2812B电源线上加100-300Ω电阻

- **外壳与安装**
  - 推荐材质：3D打印ABS或亚克力板
  - 通风设计：预留MQ2传感器通风孔
  - 屏幕开口：根据OLED和E-Paper尺寸预留开口
  - 壁挂支架：可选安装支架或壁挂设计

- **其他可选配件**
  - 无源蜂鸣器：用于警报提示
  - 按钮开关：用于手动控制和模式切换
  - 防护二极管：用于电源反接保护
  - 散热片：ESP32长时间运行时建议使用

## 软件环境

- PlatformIO
- Arduino框架
- 必要库：
  - HomeSpan (HomeKit集成)
  - Adafruit SSD1306 (OLED显示)
  - GxEPD2 (E-Paper驱动)
  - FastLED (WS2812B控制)
  - DHT sensor library
  - ArduinoJson
  - U8g2_for_Adafruit_GFX

## 软件环境

### 开发环境

- **PlatformIO**
  - 版本要求：PlatformIO Core ≥ 6.0.0
  - IDE选择：推荐VSCode + PlatformIO插件
  - 项目类型：Arduino框架
  - 构建设置：见platformio.ini

- **ESP32开发环境**
  - ESP32 Arduino Core 版本: ≥ 2.0.5
  - 分区方案: huge_app.csv (提供更大的应用空间)
  - 编译优化: -O2 (平衡空间和性能)

### 必要库依赖

1. **HomeSpan (HomeKit集成)**
   - 版本: 1.9.1
   - 功能: 提供Apple HomeKit协议实现
   - 配置: 需设置HomeKit配对码

2. **Adafruit SSD1306 (OLED显示)**
   - 版本: 2.5.13
   - 功能: 控制OLED显示屏
   - 依赖: Adafruit GFX Library

3. **GxEPD2 (E-Paper驱动)**
   - 版本: 1.5.9
   - 功能: 驱动各类电子墨水屏
   - 支持型号: 兼容多种常见墨水屏

4. **FastLED (WS2812B控制)**
   - 版本: 3.9.14
   - 功能: 控制WS2812B RGB LED
   - 配置: 支持颜色混合和过渡效果

5. **DHT Sensor Library**
   - 版本: 1.4.6
   - 功能: 读取DHT11温湿度传感器
   - 依赖: Adafruit Unified Sensor

6. **ArduinoJson**
   - 版本: 7.3.1
   - 功能: 处理JSON数据格式
   - 用途: 天气API数据解析

7. **U8g2_for_Adafruit_GFX**
   - 版本: 1.8.0
   - 功能: 增强图形库，提供更丰富的字体支持
   - 用途: 改善E-Paper和OLED上的文字显示

### 额外推荐工具

- **ESP32 Flash Download Tool**: 用于固件烧录
- **MQTT Explorer**: 调试MQTT消息(可选功能)
- **Serial Monitor**: 监控串口输出 (如Arduino IDE自带或PlatformIO的串口监视器)
- **Wireshark**: 用于网络通信调试

## 设置指南

### 1. 硬件连接

请参照项目根目录下的`Connect.md`文件进行硬件连接。该文件提供了详细的接线图和说明。按照以下步骤进行硬件连接：

1. 确认所有硬件组件完好无损
2. 按照`Connect.md`中的接线表逐一连接每个组件
3. 特别注意电源连接：
   - 3.3V设备和5V设备不要接错
   - 确保共地正确连接
4. 检查所有连接是否牢固，避免松动导致接触不良
5. 在上电前，再次检查所有连接的正确性

### 2. 开发环境设置

#### VSCode + PlatformIO安装

1. **安装VSCode**
   - 访问 https://code.visualstudio.com/ 下载适合您操作系统的VSCode
   - 按照默认设置完成安装

2. **安装PlatformIO插件**
   - 打开VSCode
   - 转到扩展视图 (`Ctrl+Shift+X` 或点击左侧菜单栏的扩展图标)
   - 搜索 "PlatformIO IDE"
   - 点击"安装"按钮并等待安装完成
   - 安装完成后，按提示重启VSCode

3. **获取项目代码**
   - 克隆本仓库: `git clone `
   - 或下载ZIP压缩包并解压

4. **打开项目**
   - 在VSCode中，选择 "文件" > "打开文件夹"
   - 导航到并选择项目文件夹
   - PlatformIO将自动识别项目并设置环境

### 3. 项目配置

在开始编译前，您需要进行一些必要的配置：

#### 创建配置文件

1. 在项目根目录下创建 `include/config.h` 文件
2. 添加以下内容，并根据您的需求修改：

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
#define WIFI_SSID "您的WiFi名称"
#define WIFI_PASSWORD "您的WiFi密码"

// HomeKit配置
#define HOMEKIT_SETUP_CODE "111-22-333" // 自定义HomeKit配对码
#define HOMEKIT_DEVICE_NAME "ESP32环境监测器" // 设备在HomeKit中显示的名称

// 天气API配置
#define WEATHER_API_KEY "您的天气API密钥"
#define CITY_ID "101010100" // 城市ID (例如北京)
#define WEATHER_UPDATE_INTERVAL 300000 // 天气更新间隔(毫秒)

// 传感器配置
#define MQ2_ALERT_THRESHOLD 800 // MQ2传感器警报阈值
#define TEMPERATURE_OFFSET 0 // 温度校准偏移值
#define HUMIDITY_OFFSET 0 // 湿度校准偏移值

#endif // CONFIG_H
```

#### 调整引脚定义（可选）

如果您的硬件连接与默认不同，需要修改相应的引脚定义：

1. 查找项目中的引脚定义文件 (通常在对应的库文件内)
2. 根据您的实际连接，修改相应的引脚定义

### 4. 编译与上传

1. **检查platformio.ini**
   - 确认platformio.ini中的环境设置与您的ESP32开发板匹配
   - 默认配置适用于ESP32-DOIT DevKit V1

2. **编译项目**
   - 点击VSCode底部状态栏的"Build"按钮(✓图标)
   - 或使用快捷键 `Ctrl+Alt+B`
   - 等待编译完成，确保没有错误

3. **连接ESP32**
   - 使用USB数据线将ESP32开发板连接到计算机
   - 等待设备被识别

4. **上传程序**
   - 点击VSCode底部状态栏的"Upload"按钮(→图标)
   - 或使用快捷键 `Ctrl+Alt+U`
   - 等待上传完成

5. **监控串口输出**
   - 点击VSCode底部状态栏的"Serial Monitor"按钮
   - 或使用快捷键 `Ctrl+Alt+S`
   - 波特率设置为115200
   - 查看系统启动和运行信息

### 5. 首次使用

成功上传程序后，ESP32将自动运行。以下是首次使用的步骤：

1. **首次启动流程**
   - ESP32上电后，OLED屏幕显示启动画面
   - 系统初始化各传感器和显示模块
   - ESP32尝试连接配置的WiFi网络
   - 连接成功后，OLED显示IP地址
   - E-Paper屏幕初始化并显示欢迎信息

2. **WiFi连接**
   - 如果WiFi连接失败，系统将每30秒尝试重新连接
   - 连接状态通过OLED屏幕和RGB LED指示
   - WiFi连接成功后，系统将同步网络时间并更新天气数据

3. **HomeKit配对**
   - 在您的iOS设备上，打开"家庭"应用
   - 点击右上角的"+"，选择"添加配件"
   - 扫描HomeKit码或手动输入配对码(默认为111-22-333)
   - 按照提示完成配对过程
   - 配对成功后，设备将显示在"家庭"应用中

4. **系统状态确认**
   - 查看OLED显示的传感器数据是否正常
   - 确认E-Paper屏幕能够正常刷新
   - 通过HomeKit应用查看设备状态
   - 检查RGB LED是否正常显示

## 使用说明

### OLED显示界面

OLED屏幕会循环显示以下信息:
- 当前温度和湿度
- 环境光照强度
- 空气质量状态
- 系统状态 (WiFi连接、HomeKit状态等)

### E-Paper显示界面

墨水屏会显示:
- 当前日期和时间
- 天气预报信息
- 关键环境参数

### HomeKit控制

通过Apple Home应用，您可以:
- 查看所有传感器的实时数据
- 控制RGB指示灯
- 接收异常环境参数的通知

### 串口命令

系统支持通过串口2发送命令进行交互:
- `status`: 返回系统状态信息
- `weather`: 立即更新天气数据
- `reset`: 重启设备
- 更多命令请参考`command.h`文件

## 使用说明

### 系统功能详解

#### OLED显示界面操作

OLED屏幕采用循环轮播方式展示多种信息，每页停留约5秒：

1. **启动页面**
   - 显示系统名称和版本号
   - 显示初始化过程和状态

2. **主信息页**
   - 顶部行：日期和时间
   - 中间区域：温度(℃)和湿度(%)
   - 底部行：WiFi连接状态和IP地址

3. **光照与空气质量页**
   - 顶部行：光照强度(lux)
   - 中间区域：当前空气质量状态
   - 底部行：MQ2传感器原始值和状态

4. **系统状态页**
   - 顶部行：运行时间
   - 中间区域：各模块运行状态
   - 底部行：内存使用情况和CPU温度

5. **天气信息页**
   - 顶部行：天气更新时间
   - 中间区域：当前天气和温度
   - 底部行：未来天气预报简述

#### E-Paper墨水屏显示详情

墨水屏刷新频率较低(默认30秒)，主要显示不频繁变化的信息：

1. **屏幕布局**
   - 顶部区域：日期、时间和系统状态
   - 中部区域：当前天气图标和详细信息
   - 左下区域：室内环境信息汇总
   - 右下区域：未来天气预报简述

2. **显示风格**
   - 采用简约黑白设计
   - 大字体显示关键信息
   - 使用图标直观表示天气状况
   - 分区清晰，信息层次分明

3. **刷新策略**
   - 定时刷新：默认每30秒刷新一次
   - 条件刷新：天气数据更新时刷新
   - 系统状态变化：重要状态改变时刷新

#### HomeKit功能使用

配对成功后，可通过Apple Home应用进行以下操作：

1. **查看传感器数据**
   - 实时温度与湿度
   - 环境光照强度(lux)
   - 空气质量状态和等级

2. **控制设备**
   - RGB指示灯开关和颜色调节
   - 传感器读取频率调整(高级功能)
   - 系统功能模式切换

3. **自动化设置**
   - 基于温度触发的自动化(如温度超过30℃触发通知)
   - 基于空气质量的自动化(如空气质量下降开启空气净化器)
   - 基于光照强度的自动化(如光照低于100lux开启灯光)

4. **场景集成**
   - 将设备添加到"离家"场景(自动进入低功耗模式)
   - 将设备添加到"回家"场景(恢复正常监测频率)
   - 创建"睡眠模式"(降低指示灯亮度和屏幕刷新频率)

5. **远程监控**
   - 通过HomePod、Apple TV或iPad作为家庭中枢
   - 在外部网络通过Home应用查看设备状态
   - 接收重要状态变化的推送通知

#### 串口命令系统

系统提供了丰富的串口命令集，通过串口2(GPIO16/17)以9600波特率进行通信：

1. **基本信息命令**
   - `status`：显示系统完整状态信息
   - `version`：显示固件版本号
   - `uptime`：显示系统运行时间
   - `memory`：显示内存使用情况

2. **传感器操作命令**
   - `read temp`：读取当前温度
   - `read humid`：读取当前湿度
   - `read light`：读取当前光照强度
   - `read gas`：读取当前MQ2传感器值
   - `calibrate [sensor] [offset]`：校准传感器偏移值

3. **控制命令**
   - `led [r] [g] [b]`：设置RGB LED颜色(0-255)
   - `display [on|off]`：控制OLED显示开关
   - `refresh`：强制刷新E-Paper显示
   - `brightness [0-100]`：设置OLED亮度

4. **网络命令**
   - `wifi status`：显示WiFi连接状态
   - `wifi scan`：扫描可用WiFi网络
   - `weather update`：立即更新天气数据
   - `ntp sync`：立即同步网络时间

5. **系统命令**
   - `reset`：重启设备
   - `factory`：恢复出厂设置
   - `sleep [seconds]`：进入深度睡眠模式
   - `debug [on|off]`：开启/关闭调试信息输出

命令格式：以换行符(\n)结束，响应以JSON格式返回

### 日常维护与管理

1. **定期检查**
   - 每月检查所有连接是否牢固
   - 检查MQ2传感器是否需要清洁或更换
   - 验证WiFi连接稳定性

2. **固件更新**
   - 定期检查仓库是否有新版本
   - 通过PlatformIO执行OTA更新
   - 更新后验证所有功能正常运行

3. **传感器校准**
   - 每3-6个月校准一次温湿度传感器
   - 使用标准仪器对比校准
   - 通过串口命令或config.h调整偏移值

4. **电源管理**
   - 检查电源线连接是否松动
   - 如使用电池供电，定期检查电池电量
   - 避免频繁断电，可能导致文件系统损坏

5. **清洁维护**
   - 使用干燥软布轻轻清洁屏幕
   - 定期清除灰尘，特别是传感器附近
   - 避免液体接触任何电子元件

## 故障排除

### 常见问题

1. **设备不能连接WiFi**
   - 检查WiFi凭据是否正确
   - 确认WiFi信号强度足够

2. **传感器数据异常**
   - 检查传感器连接是否牢固
   - 验证传感器是否正常工作

3. **HomeKit无法添加设备**
   - 确保iOS设备与ESP32在同一WiFi网络
   - 尝试重启ESP32和iOS设备

### 调试方法

- 使用串口监视器查看调试信息 (波特率: 115200)
- 检查OLED显示的错误消息
- 查看HomeSpan调试输出

## 项目结构

```
ESP32-EPaper-HMK/
│
├── lib/                  # 库文件
│   ├── command/          # 串口命令处理
│   ├── dht11/            # DHT11温湿度传感器
│   ├── epaper/           # E-Paper墨水屏
│   ├── lightsensor/      # BH1750光线传感器
│   ├── mq2sensor/        # MQ2气体传感器
│   ├── oled/             # SSD1306 OLED显示
│   ├── time/             # 时间管理
│   ├── weather/          # 天气服务
│   └── ws2812/           # WS2812B LED控制
│
├── src/                  # 源代码
│   └── main.cpp          # 主程序
│
├── include/              # 头文件
│
├── platformio.ini        # PlatformIO配置
│
└── Connect.md            # 硬件连接文档
```

## 项目结构

### 文件系统组织

```
ESP32-EPaper-HMK/
│
├── lib/                  # 库文件目录
│   ├── command/          # 串口命令处理模块
│   │   ├── command.cpp   # 命令处理实现
│   │   └── command.h     # 命令处理定义
│   │
│   ├── dht11/            # DHT11温湿度传感器模块
│   │   ├── dht11.cpp     # DHT11功能实现
│   │   └── dht11.h       # DHT11接口定义
│   │
│   ├── epaper/           # E-Paper墨水屏模块
│   │   ├── epaper.cpp    # 墨水屏驱动实现
│   │   ├── epaper.h      # 墨水屏接口定义
│   │   └── icons/        # 天气和状态图标
│   │
│   ├── lightsensor/      # BH1750光线传感器模块
│   │   ├── lightsensor.cpp # 光线传感器实现
│   │   └── lightsensor.h # 光线传感器接口
│   │
│   ├── mq2sensor/        # MQ2气体传感器模块
│   │   ├── mq2sensor.cpp # MQ2传感器实现
│   │   └── mq2sensor.h   # MQ2传感器接口
│   │
│   ├── oled/             # SSD1306 OLED显示模块
│   │   ├── oled.cpp      # OLED显示实现
│   │   ├── oled.h        # OLED显示接口
│   │   └── fonts/        # 自定义字体文件
│   │
│   ├── time/             # 时间管理模块
│   │   ├── time_manager.cpp # 时间管理实现
│   │   └── time_manager.h # 时间管理接口
│   │
│   ├── weather/          # 天气服务模块
│   │   ├── weather.cpp   # 天气服务实现
│   │   └── weather.h     # 天气服务接口
│   │
│   └── ws2812/           # WS2812B LED控制模块
│       ├── ws2812.cpp    # WS2812B控制实现
│       └── ws2812.h      # WS2812B控制接口
│
├── src/                  # 源代码主目录
│   └── main.cpp          # 主程序入口点
│
├── include/              # 全局头文件目录
│   ├── config.h          # 用户配置文件(需自行创建)
│   └── globals.h         # 全局变量和常量定义
│
├── test/                 # 测试代码目录
│   └── test_main.cpp     # 单元测试代码
│
├── .pio/                 # PlatformIO生成的构建文件夹(自动生成)
│
├── .vscode/              # VSCode配置文件夹
│
├── platformio.ini        # PlatformIO项目配置
│
├── README.md             # 项目说明文档
│
└── Connect.md            # 硬件连接文档
```

### 模块功能说明

1. **主程序(src/main.cpp)**
   - 系统初始化和主循环
   - 集成各功能模块
   - 管理系统定时事件

2. **传感器模块**
   - **dht11**: DHT11温湿度传感器控制
   - **lightsensor**: BH1750环境光传感器控制
   - **mq2sensor**: MQ2气体传感器数据采集和分析

3. **显示模块**
   - **oled**: SSD1306 OLED显示屏控制
   - **epaper**: E-Paper墨水屏控制和渲染

4. **通信模块**
   - **command**: 串口命令解析和处理
   - **weather**: 天气API通信和数据处理

5. **控制和服务模块**
   - **ws2812**: WS2812B RGB LED控制
   - **time**: 网络时间同步和时间管理

6. **HomeKit集成**
   - HomeSpan框架在main.cpp中实现
   - 定义各类服务和特征值

### 代码风格和规范

- 使用Arduino风格的setup()和loop()函数
- 模块化设计，每个功能独立封装
- 函数和变量采用驼峰命名法
- 注释丰富，便于理解和维护
- 错误处理机制统一

## 贡献

欢迎贡献代码、报告问题或提出改进建议。请遵循以下步骤:
1. Fork本仓库
2. 创建您的特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交您的更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 打开Pull Request

## 贡献指南

我们非常欢迎社区成员为本项目做出贡献。以下是详细的贡献指南：

### 贡献流程

1. **准备工作**
   - 首先Fork本仓库到您的GitHub账户
   - 将Fork后的仓库克隆到本地: `git clone https://github.com/您的用户名/ESP32-EPaper-HMK.git`
   - 添加上游仓库: `git remote add upstream https://github.com/原作者/ESP32-EPaper-HMK.git`

2. **创建分支**
   - 保持您的`main`分支与上游同步: `git pull upstream main`
   - 创建新的特性分支: `git checkout -b feature/your-feature-name`
   - 或创建bug修复分支: `git checkout -b bugfix/issue-number`

3. **开发工作**
   - 在您的分支上进行开发
   - 遵循项目的代码风格和规范
   - 编写或更新相关文档
   - 添加必要的测试代码

4. **提交更改**
   - 保持提交信息清晰明了
   - 使用规范的提交信息格式: `类型(范围): 简短描述`
   - 类型可以是: feat, fix, docs, style, refactor, test, chore
   - 例如: `feat(weather): 添加新的天气预报API支持`

5. **推送与创建Pull Request**
   - 推送到您的Fork仓库: `git push origin feature/your-feature-name`
   - 在GitHub上创建Pull Request
   - 填写Pull Request表单，详细描述您的更改

### 贡献类型

1. **新功能开发**
   - 在开始前，先创建Issue讨论新功能
   - 确保新功能符合项目整体设计理念
   - 实现时考虑与现有系统的集成

2. **Bug修复**
   - 修复前先确认Bug并在Issue中讨论
   - 提供详细的Bug复现步骤
   - 确保修复不会引入新的问题

3. **性能优化**
   - 提供优化前后的性能测试数据
   - 说明优化的思路和方法

4. **文档改进**
   - 更新或补充说明文档
   - 改进代码注释
   - 添加示例或教程

5. **硬件适配**
   - 支持新的传感器或显示设备
   - 提供详细的接线和使用说明

### 代码审查标准

提交的代码需符合以下标准才能被合并:

1. **功能完整**：实现了预期的全部功能
2. **代码质量**：代码整洁、命名规范、注释充分
3. **兼容性**：不破坏现有功能，兼容目标硬件
4. **性能**：不引入明显的性能问题
5. **测试**：包含必要的测试代码或测试说明
6. **文档**：更新相关文档

### 版本发布规则

本项目采用语义化版本规则(SemVer):

- **主版本号**：当进行不兼容的API更改时
- **次版本号**：当添加向后兼容的功能时
- **修订号**：当进行向后兼容的问题修复时

## 许可证

本项目采用MIT许可证 - 查看LICENSE文件了解详情。

## 许可证详情

MIT License

Copyright (c) 2023 [项目作者]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## 鸣谢

- HomeSpan团队 - 提供优秀的HomeKit集成库
- Adafruit - 提供多种传感器和显示屏库
- PlatformIO - 提供易用的嵌入式开发环境
- 所有贡献者和使用者

## 鸣谢与参考

### 开源项目与库

- **[HomeSpan](https://github.com/HomeSpan/HomeSpan)** - 优秀的ESP32 HomeKit集成库
  - 作者: Gregg E. Berman
  - 许可证: MIT
  - 用途: 提供HomeKit协议实现和设备配置

- **[Adafruit GFX & SSD1306](https://github.com/adafruit/Adafruit-GFX-Library)** - 显示驱动库
  - 作者: Adafruit Industries
  - 许可证: BSD
  - 用途: OLED显示屏驱动和图形渲染

- **[GxEPD2](https://github.com/ZinggJM/GxEPD2)** - E-Paper驱动库
  - 作者: Jean-Marc Zingg
  - 许可证: GNU GPL v3.0
  - 用途: 墨水屏驱动和控制

- **[FastLED](https://github.com/FastLED/FastLED)** - LED控制库
  - 作者: Daniel Garcia等
  - 许可证: MIT
  - 用途: WS2812B RGB LED控制

- **[ArduinoJson](https://github.com/bblanchon/ArduinoJson)** - JSON解析库
  - 作者: Benoit Blanchon
  - 许可证: MIT
  - 用途: 天气API数据解析

### 技术参考与资源

- **ESP32技术文档** - Espressif Systems
  - 官方资料: https://docs.espressif.com/
  - 参考内容: ESP32硬件设计、接口说明

- **Apple HomeKit技术规范**
  - 官方资料: https://developer.apple.com/homekit/
  - 参考内容: HomeKit协议和认证要求

- **电子墨水屏技术资料** - Waveshare/Good Display
  - 技术资料: 各型号E-Paper数据手册
  - 参考内容: 屏幕初始化和刷新方法

### 社区支持

- **Arduino社区** - 提供许多示例代码和问题解决方案
- **ESP32论坛** - 技术问题讨论和解决
- **GitHub贡献者** - 代码贡献和问题反馈

### 特别感谢

- 所有测试用户 - 提供宝贵的使用反馈
- 开源硬件社区 - 分享设计理念和经验
- 所有项目贡献者 - 帮助改进代码和文档

本项目将继续保持开源和活跃维护，欢迎更多社区成员参与贡献！ 