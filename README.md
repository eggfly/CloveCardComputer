# SBC
## Single Board Computer

ESP32-S3R8 with 8MB internal Octal SPI PSRAM, 32MB external QIO Flash.

## IO Pins


| Function | IO Pin | 备注 |
| -- | -- | -- |
| Boot_Button | IO0 | 左边按钮 |
| ROW1-ROW8 | IO1-IO8 | 键盘按键4行，左边部分的4行是ROW1-ROW4，右边是ROW5-ROW8 |
| I2S_DIN | IO9 | I2S DIN 数据 Pin |
| COL1-COL5 | IO10-IO14 | 键盘按键5列 |
| LCD_RST | IO15 | 屏幕 Reset |
| LCD_DC | IO16 | 屏幕 Data/Command |
| LCD_MOSI | IO17 | 屏幕 SPI: MOSI |
| LCD_SCLK | IO18 | 屏幕 SPI: 时钟 |
| USB D- | IO19 | 已连到 USB Type-C |
| USB D+ | IO20 | 已连到 USB Type-C |
| LCD_CS | IO21 | 屏幕 SPI: 片选 |
| N/A | IO33-37 | 内部已占用 |
| PMIC_IRQ | IO38 | PMIC 中断 |
| SD_MISO | IO39 | SD MISO |
| SD_SCLK | IO40 | SD SCLK |
| SD_MOSI | IO41 | SD MOSI |
| SD_CS   | IO42 | SD 片选 |
| I2C_SDA | IO43 | 原来是 U0TXD，现在是 SDA，外部上拉 |
| I2C_SCL | IO44 | 原来是 U0RXD，现在是 SCL，外部上拉 |
| LCD_BL  | IO45 | 屏幕背光，默认开机下拉，常亮，使用封装内 PSRAM，不再起到 SPI_VDD 控制作用 |
| SD_MODE  | IO46 | MAX98357 SD_MODE，默认开机下拉，控制左右声道和静音，详情见 MAX98357 手册 |
| I2S_BCK | IO47 | I2S BCK |
| I2S_LRCK | IO48 | I2S LRCK |

## I2C Addresses


| Device | Address | 备注 |
| -- | -- | -- |
| PCF8563 | 81 (0x51) | RTC 时钟 |
| AXP2101 | 52 (0x34) | 电源 IC |

## Key map
| Key | COL1 | COL2 | COL3 | COL4 | COL5 | 左/右 | COL1 | COL2 | COL3 | COL4 | COL5 |
| --  |  --  |  --  |  --  |  --  |  --  | -- |  --  |  --  |  --  |  --  |  --  |
| ROW1 | 1 | 2 | 3 | 4 | 5 | ROW5 | 6 | 7 | 8 | 9 | 0 |
| ROW2 | Q | W | E | R | T | ROW6 | Y | U | I | O | P |
| ROW3 | A | S | D | F | G | ROW7 | H | J | K | L | 上 |
| ROW4 | Z | X | C | V | B | ROW8 | N | M | 左 | 下 | 右 |
