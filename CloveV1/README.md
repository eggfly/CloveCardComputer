# Version 1 of Clove Card Computer

## Hardware

This ESP32-S3 card computer includes:

* ESP32-S3R8 with 8MB internal Octal SPI PSRAM
* 32MB external QIO Flash
* 2.0" 240*536 AMOLED screen (Without Touch)
* BV6802W AMOLED Power IC
* 7.5*7.5mm 5-way Navigation switch and AW9523 IO expanders
* BlackBerry Q10 QWERTY Keyboard
* Molex TF-Card
* USB Type-C, Li-ion battery and Grove I2C port
* AXP2101 power IC
* MAX98357 amplifier and SMT speaker
* PCM5102 DAC and headphone jack
* PCF8563 RTC
* BMI270 IMU 6-axis sensor
* BME280 pressure & temperature & humidity sensor
* SHT30 humidity and temperature sensor
* SPM1423 MEMS microphone (digital I2S)


## IO Pins


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


| Function | 备注 |
| -- | -- |
| AXP Power Button | 顶部按钮2,通过I2C和IRQ读取 |
| Charge LED | 通过I2C读取或者控制 |
| ALDO1 | 默认1.8V，控制SD_MODE<br> > 1.4V: LEFT_MODE<br>0.77-1.4V: RIGHT_MODE<br>0.16-0.77V: LEFT/2+RIGHT/2<br>< 0.16V: Shutdown |
| ALDO2 | 默认关闭，控制Q10键盘背光 |
| ALDO3 | 默认3.3V，给PCM5102的DVDD和AVDD的LDO供电 |
| AW9523 | P0_0 - P0_4 分别接五向开关的 TOP, LEFT, RIGHT, DOWN, PRESSED |

## I2C Addresses


| Device | Address | 备注 |
| -- | -- | -- |
| AXP2101 | 52 (0x34)  | 电源 IC |
| SHT30   | 68 (0x44)  | 温湿度传感器 |
| PCF8563 | 81 (0x51)  | RTC 时钟 |
| AW9523  | 91 (0x5B)  | AW9523 IO扩展器 (0b1011011) |
| BMI270  | 104 (0x68) |  运动传感器 (BMI2_I2C_PRIM_ADDR) |
| BME280  | 119 (0x77) | (0x76) 气压传感器 (BMP280_I2C_ADDRESS_1) |

```C
#define BMP280_I2C_ADDRESS_0  0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1  0x77 //!< I2C address when SDO pin is high
```

## Keyboard


Pinout:
```
    GND   1 | 28  GND
MIC_VDD   2 | 27  ROW7
    MIC   3 | 26  LEDA 1 & 2
   AGND   4 | 25  LEDK 1 & 4
    GND   5 | 24  LEDA 3 & 4
   ROW1   6 | 23  LEDK 2 & 3
   COL1   7 | 22  ROW6
   ROW2   8 | 21  ROW5
   COL2   9 | 20  ROW4
   COL3  10 | 19  COL5
    GND  11 | 18  ROW3
    GND  12 | 17  COL4
    GND  13 | 16  GND
    GND  14 | 15  GND
```

Since the numbering can be confusing (is it the plug or the receptacle?), see footprint below for clarification.


### Column/Row to key matrix

Copied from [3]

Regular keys:

|          | COL1 | COL2 | COL3 | COL4 | COL5 | 
|----------|------|------|------|------|------| 
| **ROW1** | Q    | E    | R    | U    | O    |
| **ROW2** | W    | S    | G    | H    | L    |
| **ROW3** | sym  | D    | T    | Y    | I    |
| **ROW4** | A    | P    | R ⇧  | ↵    | ⌫   |
| **ROW5** | alt  | X    | V    | B    | $    |
| **ROW6** | spac | Z    | C    | N    | M    |
| **ROW7** | 🎤   | L ⇧  | F    | J    | K    |

Alternative keys:

|          | COL1 | COL2 | COL3 | COL4 | COL5 |
|----------|------|------|------|------|------|
| **ROW1** |   #  |   2  |   3  |   _  |   +  |
| **ROW2** |   1  |   4  |   /  |   :  |   "  |
| **ROW3** |      |   5  |   (  |   )  |   -  |
| **ROW4** |   *  |   @  |      |      |      |
| **ROW5** |      |   8  |   ?  |   !  |  🔊  |
| **ROW6** |      |   7  |   9  |   ,  |   .  |
| **ROW7** |   0  |      |   6  |   ;  |   '  |

### Blackberry UX

This part describes the way the Blackberry phone used the keyboard, might be a good reference and starting point for implementing a custom UX. Based on [7].

- Pressing the space twice adds a period and capitalizes the next letter
- Pressing and holding a letter key inserts capitalizes the letter
- Alt + R ⇧ turns on Caps Lock, pressing either shift turns it off
- Alt + L ⇧ turns on Num Lock, pressing either shift turns it off
- Pressing Alt will make the next key pressed insert a symbol from the alternative keys matrix
- Pressing Shift will make the next key pressed be capitalized


### Sources

[1] https://hackaday.io/project/27520-pimp-personal-information-manager-pager  
[2] https://www.eevblog.com/forum/beginners/how-to-connect-to-a-very-very-challanging-blackberry-q10-keyboard-connector/  
[3] https://forum.arduino.cc/index.php?topic=355709.0  
[4] https://grabcad.com/library/blackberry-q10-keyboard-shape-1  
[5] https://www.hirose.com/product/en/products/BM14/BM14B%280.8%29-24DS-0.4V%2853%29/  
[6] https://help.blackberry.com/en/blackberry-q10/10.3.1/help/mar1396885531697.html  
[7] https://help.blackberry.com/en/blackberry-q10/10.3.1/help/mba1343750483597.html   

### LLM
* https://www.bilibili.com/video/BV1rZ42187Ki/
* git@github.com:MetaWu2077/Esp32_VoiceChat_LLMs.git
* https://www.bilibili.com/video/BV1xz421S7d6/
* https://github.com/MetaWu2077/Esp32_VoiceChat_LLMs/tree/main
