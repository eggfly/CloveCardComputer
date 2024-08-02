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
* PCF8574 for game pads and some other IO pins
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
| BootBtn | IO0 | 左侧按钮3 |


## I2C Addresses

```
Wire0: Scanning for I2C devices ...
I2C device found at address 0x10
I2C device found at address 0x11
I2C device found at address 0x20
I2C device found at address 0x34
I2C device found at address 0x40
I2C device found at address 0x41
I2C device found at address 0x51
I2C device found at address 0x60
Wire1: Scanning for I2C devices ...
I2C device found at address 0x34
I2C device found at address 0x44
I2C device found at address 0x68
I2C device found at address 0x77
```

### I2C0: SDA=2, SCL=3


| Device | Address |  Bus | 备注 |
| -- | -- | -- | -- |
| RDA5807M | 16 (0x10), 17 (0x11), 96 (0x60) | I2C0 | FM Radio |
| PCF8574 | 32 (0x20) | I2C0 | ? |
| AXP2101 | 52 (0x34)  | I2C0 | 电源 IC |
| FT3168  | 56 (0x38)  | I2C0 | Screen Touch IC |
| INA219  | 64,65 (0x40, 0x41)  | I2C0 | Current sensor |
| PCF8563 | 81 (0x51)  | I2C0 | RTC 时钟 |



### I2C1: SDA=43, SCL=44

| Device | Address |  Bus | 备注 |
| -- | -- | -- | -- |
| TCA8418 | 52 (0x34)  | I2C1 | Keyboard scan IC; 注: 和AXP2101一样，需要另一个I2C总线 |
| SHT30   | 68 (0x44)  | I2C1 | 温湿度传感器 |
| BMI270  | 104 (0x68) | I2C1 | 运动传感器 (BMI2_I2C_PRIM_ADDR) |
| BME280  | 119 (0x77) | I2C1 | (0x76) 气压传感器 (BMP280_I2C_ADDRESS_1) |


```C
#define BMP280_I2C_ADDRESS_0  0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1  0x77 //!< I2C address when SDO pin is high
```


### LLM
* https://www.bilibili.com/video/BV1rZ42187Ki/
* git@github.com:MetaWu2077/Esp32_VoiceChat_LLMs.git
* https://www.bilibili.com/video/BV1xz421S7d6/
* https://github.com/MetaWu2077/Esp32_VoiceChat_LLMs/tree/main
