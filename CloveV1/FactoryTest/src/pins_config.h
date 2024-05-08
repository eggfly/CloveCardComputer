#pragma once

/***********************config*************************/
#define LCD_USB_QSPI_DREVER   1

#define SPI_FREQUENCY         80000000
#define TFT_SPI_MODE          SPI_MODE0
#define TFT_SPI_HOST          SPI2_HOST

#define EXAMPLE_LCD_H_RES     536
#define EXAMPLE_LCD_V_RES     240
#define LVGL_LCD_BUF_SIZE     (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)

/***********************config*************************/

#define SEND_BUF_SIZE         (0x4000) //(LCD_WIDTH * LCD_HEIGHT + 8) / 10

#define TFT_DC                7
#define TFT_RES               9
#define TFT_CS                21
#define TFT_SCK               47

#define TFT_QSPI_SCK          45
#define TFT_QSPI_D0           15
#define TFT_QSPI_D1           16
#define TFT_QSPI_D2           17
#define TFT_QSPI_D3           18
