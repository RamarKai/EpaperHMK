# ESP32 E-Paper智能设备接线图

本文档提供了ESP32智能设备的所有模块接线方式，包括各类传感器和显示模块。

## 硬件清单

1. ESP32开发板 (ESP32-DOIT DevKit V1)
2. SSD1306 OLED显示屏 (128x64像素) - I2C接口
3. BH1750环境光传感器 - I2C接口
4. MQ2气体传感器 - 模拟接口
5. WS2812B RGB LED - 数字接口
6. DHT11温湿度传感器 - 数字接口
7. 外部通信设备 (连接到串口2)
8. E029A01-FPC-A1 墨水屏 (128x296像素) - SPI接口
9. 两个按钮 - 数字输入（用于控制切换）

## 接线详情

### 1. SSD1306 OLED显示屏

| ESP32引脚 | OLED引脚 | 说明 |
|-----------|--------|------|
| GPIO21    | SDA    | I2C数据线 |
| GPIO22    | SCL    | I2C时钟线 |
| 3.3V      | VCC    | 电源 |
| GND       | GND    | 接地 |

*I2C地址: 0x3C*

### 2. BH1750环境光传感器

| ESP32引脚 | BH1750引脚 | 说明 |
|-----------|---------|------|
| GPIO4     | SDA     | I2C数据线 |
| GPIO5     | SCL     | I2C时钟线 |
| 3.3V      | VCC     | 电源 |
| GND       | GND     | 接地 |

*注意: BH1750使用独立的I2C总线*

### 3. MQ2气体传感器

| ESP32引脚 | MQ2引脚 | 说明 |
|-----------|-------|------|
| GPIO34    | AO    | 模拟输出 |
| 5V        | VCC   | 电源 |
| GND       | GND   | 接地 |

*注意: MQ2传感器需要5V供电*

### 4. WS2812B RGB LED

| ESP32引脚 | WS2812B引脚 | 说明 |
|-----------|----------|------|
| GPIO2     | DIN      | 数据输入 |
| 5V        | VCC      | 电源 |
| GND       | GND      | 接地 |

*注意: 项目配置为控制1个WS2812B LED*

### 5. DHT11温湿度传感器

| ESP32引脚 | DHT11引脚 | 说明 |
|-----------|---------|------|
| GPIO27    | DATA    | 数据 |
| 3.3V      | VCC     | 电源 |
| GND       | GND     | 接地 |

*注意: DHT11原先使用的GPIO4与BH1750的SDA引脚冲突，已更改为GPIO27*

### 6. 外部串口通信 (Serial2)

| ESP32引脚 | 外部设备 | 说明 |
|-----------|--------|------|
| GPIO16    | RX     | 接收数据 (ESP32角度) |
| GPIO17    | TX     | 发送数据 (ESP32角度) |
| GND       | GND    | 共地 |

*通信参数: 9600波特率, 8位数据位, 无校验位, 1位停止位*

### 7. E029A01-FPC-A1 墨水屏

| ESP32引脚 | 墨水屏引脚 | 说明 |
|-----------|----------|------|
| GPIO25    | BUSY     | 忙信号 |
| GPIO26    | RST      | 复位 |
| GPIO32    | DC       | 数据/命令选择 |
| GPIO33    | CS       | 片选 |
| GPIO18    | SCK      | SPI时钟 |
| GPIO23    | MOSI     | SPI数据输入 |
| 3.3V      | VCC      | 电源 |
| GND       | GND      | 接地 |

*注意: 墨水屏使用SPI接口，仅使用MOSI而不使用MISO*

### 8. 按钮接口

| ESP32引脚 | 设备 | 说明 |
|-----------|------|------|
| GPIO13    | 按钮1 | 控制按钮1（接地时触发） |
| GPIO14    | 按钮2 | 控制按钮2（接地时触发） |
| GND       | 按钮公共端 | 按钮按下时接地 |

*注意: 按钮使用内部上拉电阻，按下时连接到GND*

## 引脚冲突说明

已解决的引脚冲突：
- 之前的GPIO4引脚冲突（DHT11和BH1750共用）已通过将DHT11数据引脚重新分配到GPIO27解决
- 此方案保留了BH1750的原有I2C引脚配置（GPIO4/GPIO5），同时确保DHT11可以正常工作
- 原计划使用的按钮引脚GPIO34与MQ2传感器的模拟输入引脚冲突，已重新分配按钮到GPIO13和GPIO14
- 所有SPI设备（墨水屏）和I2C设备（OLED、BH1750）使用的引脚没有冲突

## 电源需求

- **5V需求设备**: WS2812B LED, MQ2气体传感器
- **3.3V需求设备**: ESP32核心, OLED显示屏, BH1750光线传感器, DHT11温湿度传感器, E029A01-FPC-A1墨水屏, 按钮（通过内部上拉电阻）

## 接线图

```
ESP32 DOIT DevKit V1
┌────────────────────────────┐
│                            │
│           [USB]            │
│                            │
└────────┬──────────────┬────┘
         │              │
         │              │
     3V3 ●              ● 5V
         │              │
         │              │     ┌─────────────┐
     GND ●──────────────┼─────┤ OLED SSD1306│
         │              │     │             │
 GPIO21  ●──────────────┼─────┤ SDA         │
         │              │     │             │
 GPIO22  ●──────────────┼─────┤ SCL         │
         │              │     │             │
         │              └─────┤ VCC         │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
         ├────────────────────┤ BH1750      │
         │                    │             │
  GPIO4  ●────────────────────┤ SDA         │
         │                    │             │
  GPIO5  ●────────────────────┤ SCL         │
         │                    │             │
     GND ●────────────────────┤ GND         │
         │                    │             │
     3V3 ●────────────────────┤ VCC         │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
 GPIO34  ●────────────────────┤ MQ2 AO      │
         │                    │             │
     GND ●────────────────────┤ GND         │
         │                    │             │
      5V ●────────────────────┤ VCC         │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
  GPIO2  ●────────────────────┤ WS2812B DIN │
         │                    │             │
     GND ●────────────────────┤ GND         │
         │                    │             │
      5V ●────────────────────┤ VCC         │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
 GPIO27  ●────────────────────┤ DHT11 DATA  │
         │                    │             │
     GND ●────────────────────┤ GND         │
         │                    │             │
     3V3 ●────────────────────┤ VCC         │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
 GPIO16  ●────────────────────┤ 外部设备 RX  │
         │                    │             │
 GPIO17  ●────────────────────┤ 外部设备 TX  │
         │                    │             │
     GND ●────────────────────┤ GND         │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
 GPIO25  ●────────────────────┤ 墨水屏 BUSY  │
         │                    │             │
 GPIO26  ●────────────────────┤ 墨水屏 RST   │
         │                    │             │
 GPIO32  ●────────────────────┤ 墨水屏 DC    │
         │                    │             │
 GPIO33  ●────────────────────┤ 墨水屏 CS    │
         │                    │             │
 GPIO18  ●────────────────────┤ 墨水屏 SCK   │
         │                    │             │
 GPIO23  ●────────────────────┤ 墨水屏 MOSI  │
         │                    │             │
     GND ●────────────────────┤ 墨水屏 GND   │
         │                    │             │
     3V3 ●────────────────────┤ 墨水屏 VCC   │
         │                    └─────────────┘
         │
         │                    ┌─────────────┐
 GPIO13  ●────────────────────┤ 按钮1        │
         │                    │             │
 GPIO14  ●────────────────────┤ 按钮2        │
         │                    │             │
     GND ●────────────────────┤ 按钮公共端   │
         │                    └─────────────┘
         │
    [其他引脚]
         │
└────────┴────────────────────┘
```

## 注意事项

1. **硬件更新**: DHT11传感器已从GPIO4移至GPIO27，确保实际硬件接线与此配置一致。
2. **电源**: 确保ESP32有足够的电源供应所有设备，特别是WS2812B LED和MQ2气体传感器。
3. **I2C地址**: 确保在同一I2C总线上的设备有不同的I2C地址，以避免冲突。
4. **线路长度**: 对于I2C设备，连接线不宜过长，以避免信号衰减。
5. **墨水屏刷新**: E029A01-FPC-A1墨水屏更新速度较慢，设计为每分钟刷新一次，以节省功耗。
6. **按钮连接**: 按钮使用内部上拉电阻，按下时要连接到GND。按钮应该包含去抖动电路或在软件中实现去抖动处理。

## 建议改进

1. **已实施的改进**:
   - DHT11的数据引脚已经从GPIO4更改为GPIO27，解决了与BH1750的引脚冲突
   - 添加了E029A01-FPC-A1墨水屏，提供了低功耗的信息显示方案
   - 添加了两个按钮（GPIO13和GPIO14），用于控制和交互

2. **电源保护**:
   - 考虑为WS2812B LED添加一个级联电容(100μF)，以平滑电源
   - 为模拟传感器添加滤波电容，以提高测量稳定性
   - 墨水屏VCC与GND之间可添加0.1μF和10μF电容，提高稳定性

3. **按钮优化**:
   - 可考虑在按钮电路中添加硬件去抖电路（如RC滤波器）
   - 按钮可配合使用10KΩ外部上拉电阻，以提高稳定性
