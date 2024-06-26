# Version 2 of Clove Card Computer

## Hardware

This ESP32-S3 card computer includes:

* ESP32-S3R8 with 8MB internal Octal SPI PSRAM
* 32MB external QIO Flash
* 2.0" 240*536 AMOLED screen QSPI RM67162 (with FT3168 Touch)
* BV6802W AMOLED Power IC
* Keyboard: WenQuXing NC1020 compatible 50 keys
* RGB keyboard backlight
* TCA8418 I2C Controlled Keypad Scan IC
* Molex TF-Card
* USB Type-C, Li-ion battery and Grove I2C port
* AXP2101 power IC
* PCM5102 DAC and headphone jack
* RDA5807M FM radio and FSA5157 analog switch
* NS4150 amplifier and iPhone speaker
* PCF8563 RTC
* BMI270 IMU 6-axis sensor
* BME280 pressure & temperature & humidity sensor
* SHT30 humidity and temperature sensor
* SPM1423 MEMS microphone (digital I2S)
* INA219 High Side DC Current/Voltage/Power Sensor 

## IO Pins

TODO: CHANGE

| Function | IO Pin | 备注 |
| -- | -- | -- |
| BootBtn/MIC_DATA | IO0 | 顶部按钮1 & 麦克风 I2S Data |
| ROW1-ROW7 | IO1-IO7 | Q10键盘 ROW1-7 |
| MIC_CLK | IO8 | 麦克风 I2S CLK Pin |
| LCD_RST | IO9 | 屏幕 Reset |
| COL1-COL5 | IO10-IO14 | Q10键盘 COL1-5 |
| LCD_SIO0 | IO15 | 屏幕 SIO0 |
| LCD_SI1  | IO16 | 屏幕 SI1 |
| LCD_SI2  | IO17 | 屏幕 SI2 |
| LCD_SI3  | IO18 | 屏幕 SI3 |
| USB D- | IO19 | 连到 USB Type-C |
| USB D+ | IO20 | 连到 USB Type-C |
| LCD_CS | IO21 | 屏幕 QSPI: 片选 |
| N/A | IO33-37 | 内部已占用 |
| PMIC_IRQ | IO38 | PMIC 中断 |
| SD_MISO | IO39 | SD MISO |
| SD_SCLK | IO40 | SD SCLK |
| SD_MOSI | IO41 | SD MOSI |
| SD_CS   | IO42 | SD 片选 |
| I2C_SDA | IO43 | 原来是 U0TXD，现在是 SDA，外部上拉 |
| I2C_SCL | IO44 | 原来是 U0RXD，现在是 SCL，外部上拉 |
| LCD_SCLK| IO45 | 屏幕 QSPI: 时钟 |
| I2S_DIN | IO46 | I2S Data |
| I2S_BCK | IO47 | I2S BCK  |
| I2S_LRCK| IO48 | I2S LRCK |

## Others

TODO: CHANGE

| Function | 备注 |
| -- | -- |
| AXP Power Button | 按钮2,通过I2C和IRQ读取 |
| Charge LED | 通过I2C读取或者控制 |
| ALDO3 | 默认3.3V，给PCM5102的DVDD和AVDD的LDO供电 |


## I2C Addresses

### I2C0: SDA=2, SCL=3


| Device | Address |  Bus | 备注 |
| -- | -- | -- | -- |
| RDA5807M | 16 (0x10), 17 (0x11), 96 (0x60) | I2C0 | FM Radio |
| ? | 32 (0x20) | I2C0 | ? |
| AXP2101 | 52 (0x34)  | I2C0 | 电源 IC |
| FT3168  | 56 (0x38)  | I2C0 | Screen Touch IC |
| INA219  | 64,65 (0x40, 0x41)  | I2C0 | Current sensor |
| PCF8563 | 81 (0x51)  | I2C0 | RTC 时钟 |



### I2C1: SDA=43, SCL=44

| Device | Address |  Bus | 备注 |
| -- | -- | -- | -- |
| RDA5807M | 16 (0x10), 17 (0x11), 96 (0x60) | I2C0 | FM Radio |
| TCA8418 | 52 (0x34)  | I2C1 | Keyboard scan IC; 注: 和AXP2101一样，需要另一个I2C总线 |


```C
#define BMP280_I2C_ADDRESS_0  0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1  0x77 //!< I2C address when SDO pin is high
```


### LLM
* https://www.bilibili.com/video/BV1rZ42187Ki/
* git@github.com:MetaWu2077/Esp32_VoiceChat_LLMs.git
* https://www.bilibili.com/video/BV1xz421S7d6/
* https://github.com/MetaWu2077/Esp32_VoiceChat_LLMs/tree/main
