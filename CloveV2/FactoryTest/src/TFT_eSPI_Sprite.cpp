/**
   @file      TFT_eSPI_Sprite.ino
   @author    Lewis He (lewishe@outlook.com)
   @license   MIT
   @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
   @date      2023-06-14

*/
#include <Arduino.h>
#include "rm67162.h"
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI
#include "true_color.h"
#include <app.h>
#include "res.h"

#include "music_player_536_240.h"

#include <Arduino_GFX_Library.h>

// #include "binaryttf_small.h"
// #include "GenSenRounded-R-binaryttf.h"
// #include "AlibabaPuHuiTi-3-65-Medium-binaryttf.h"
// #include "Regular-Hei-binaryttf.h"
// #include "FangZhengZhuYuan-binaryttf.h"
#include <OpenFontRender.h>

// #define RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#if ARDUINO_USB_CDC_ON_BOOT != 1
#warning "If you need to monitor printed data, be sure to set USB CDC On boot to ENABLE, otherwise you will not see any data in the serial monitor"
#endif

#ifndef BOARD_HAS_PSRAM
#error "Detected that PSRAM is not turned on. Please set PSRAM to OPI PSRAM in ArduinoIDE"
#endif

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

Arduino_Canvas *gfx = new Arduino_Canvas(WIDTH /* width */, HEIGHT /* height */, nullptr);

uint8_t *swappedBuffer;

OpenFontRender render;

unsigned long targetTime = 0;
byte red = 31;
byte green = 0;
byte blue = 0;
byte state = 0;
unsigned int colour = red << 11;

void drawRainbow();
unsigned int rainbow(uint8_t value);

uint8_t brightness = 0xC0;

const void *font_map_ptr;

const size_t font1_size = 8429388;
const size_t font2_size = 4625768;

void flush_screen()
{
  swapBytes((uint8_t *)gfx->getFramebuffer(), swappedBuffer, WIDTH * HEIGHT * 2);
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)swappedBuffer);
  // lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
}

void setup_amoled()
{
  /*
    Compatible with touch version
    Touch version, IO38 is the screen power enable
    Non-touch version, IO38 is an onboard LED light
  * * */
  // pinMode(PIN_LED, OUTPUT);
  // digitalWrite(PIN_LED, HIGH);
  // pinMode(0, INPUT);
  rm67162_init();
  lcd_setRotation(1);
  spr.createSprite(WIDTH, HEIGHT);
  spr.setSwapBytes(1);
  gfx->begin();
  gfx->setRotation(0);
  swappedBuffer = (uint8_t *)ps_malloc(WIDTH * HEIGHT * 2);
  if (swappedBuffer)
  {
    printf("swappedBuffer=%p\n", swappedBuffer);
  }
  else
  {
    printf("buffer malloc failed.\n");
  }
  // delay(5000);
  lcd_setBrightness(brightness);
  render.set_printFunc([&](const char *msg)
                       { printf(msg); });
  // render.setSerial(Serial);
  render.showFreeTypeVersion();
  render.showCredit();

  if (font_map_ptr && !render.loadFont((const unsigned char *)font_map_ptr, font2_size))
  {
    printf("Font initialized font(%dKB) from partition OK!\n", font2_size / 1024);
  }
  else
  {
    printf("Font initialize error.\n");
    return;
  }

  // Arduino 专案始于2003年，作为意大利伊夫雷亚地区伊夫雷亚互动设计研究所的学生专案，目的是为新手和专业人员提供一种低成本且简单的方法，以建立使用感测器与环境相互作用的装置执行器。适用于初学者爱好者的此类装置的常见范例包括感测器、简单机械人、恒温器和运动检测器。
  // Arduino 是一個開源嵌入式硬體平台，用來供用戶製作可互動式的嵌入式專案。此外 Arduino 作為一個開源硬體和開源軟件的公司，同時兼有專案和用戶社群。該公司負責設計和製造Arduino電路板及相關附件。這些產品按照GNU寬通用公共許可證（LGPL）或GNU通用公共許可證（GPL）[1]許可的開源硬體和軟件分發的，Arduino 允許任何人製造 Arduino 板和軟件分發。 Arduino 板可以以預裝的形式商業銷售，也可以作為DIY套件購買。
  render.setDrawer(spr); // Set drawer object
  // 设置startWrite和endWrite是空操作，否则背景变绿，有点问题
  render.set_startWrite([&](void) {});
  render.set_endWrite([&](void) {});
  unsigned long t_start = millis();
  printf("space ratio = %lf\n", render.getLineSpaceRatio());
  render.setLineSpaceRatio(0.80);
  printf("new space ratio = %lf\n", render.getLineSpaceRatio());
  render.setFontSize(12);
  render.setFontColor(TFT_WHITE, TFT_BLACK);
  render.printf("Hello World\n");
  render.seekCursor(0, -3);
  render.setFontSize(20);
  render.setFontColor(TFT_GREEN, TFT_BLACK);
  render.printf("完全なUnicodeサポート\n");
  render.seekCursor(0, -3);

  render.setFontSize(25);
  render.setFontColor(TFT_ORANGE, TFT_BLACK);
  render.printf("こんにちは世界\n");
  render.seekCursor(0, -3);

  render.setFontSize(19);
  render.setFontColor(TFT_MAGENTA, TFT_BLACK);
  // render.printf("Clove 电脑 - 音乐播放器\n（支持 mp3、flac、aac）\n");
  // render.calculateBoundingBox()
  render.printf(
      "Arduino 专案始于2003年，作为意大利伊夫雷亚地区伊夫雷亚\n"
      "互动设计研究所的学生专案，目的是为新手和专业人员提供一种\n"
      "低成本且简单的方法，以建立使用感测器与环境相互作用的装置\n");
  render.printf(
      "执行器。适用于初学者爱好者的此类装置的常见范例包括感测器\n"
      "、简单机械人、恒温器和运动检测器。\n");
  render.setFontSize(12);
  render.setFontColor(TFT_RED);
  render.printf(
      "Arduino 是一個開源嵌入式硬體平台，用來供用戶製作可互動式的嵌入式專案。此外 Arduino 作為一個開源硬體和開源軟件\n");
  render.printf(
      "的公司，同時兼有專案和用戶社群。該公司負責設計和製造 Arduino 電路板及相關附件。這些產品按照GNU寬通用公共許可\n");
  render.printf(
      "證（LGPL）或GNU通用公共許可證（GPL）[1]許可的開源硬體和軟件分發的，Arduino 允許任何人製造 Arduino 板和軟件\n");
  render.printf(
      "分發。Arduino 板可以以預裝的形式商業銷售，也可以作為DIY套件購買。\n");
  // render.printf(
  //     "\n"
  //     " \n"
  //     "\n");
  // render.setCursor(WIDTH/2, HEIGHT/6);
  // render.setFontSize(160);
  // render.setFontColor(TFT_WHITE);
  // render.cprintf("牛逼！\n");
  unsigned long t_end = millis();
  printf("Time: %ld ms\n", t_end - t_start);
  // finally flush
  // gfx->fillScreen(TFT_GREEN);
  gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)music_player_536_240, 536, 240);
  unsigned long start = millis();
  swapBytes((uint8_t *)gfx->getFramebuffer(), swappedBuffer, WIDTH * HEIGHT * 2);
  printf("swapBuffer cost %lu\n", millis() - start);
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)swappedBuffer);
  // lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
  // delay(1000);
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());

  printf("setup() done\n");
}

void loop_touch_amoled()
{
  if (tp_fingers_count >= 1)
  {
    gfx->fillCircle(tp_x1, tp_y1, 5, TFT_MAGENTA);
  }
  if (tp_fingers_count >= 2)
  {
    gfx->fillCircle(tp_x2, tp_y2, 5, TFT_ORANGE);
  }

  flush_screen();
}

void test_amoled()
{
  spr.pushImage(0, 0, WIDTH, HEIGHT, (uint16_t *)gImage_true_color);
  // spr.pushImage((536 - 480) / 2, 0, 480, 240, (uint16_t *)nc1020);

  // spr.pushImage(0, 0, 256, 240, (uint16_t *)mario);
  // spr.pushImage(0 + 536 / 2, 0, 256, 240, (uint16_t *)mario);
  // Finally send buffer to screen
  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
  // if (!digitalRead(0)) {
  //   Serial.println("press");
  //   lcd_setBrightness(brightness);
  //   // lcd_setHBMMode(true);
  //   if (brightness == 0) {
  //     brightness = 0x60;
  //   } else {
  //     brightness = 0;
  //   }
  //   printf("brightness done\n");
  //   delay(100);
  // }
  // delay(10);
  // return;
  delay(3000);

  spr.fillSprite(TFT_BLACK);

  spr.fillRect(0, 0, 67, 120, TFT_RED);
  spr.fillRect(67 * 1, 0, 67, 120, TFT_GREEN);
  spr.fillRect(67 * 2, 0, 67, 120, TFT_BLUE);
  spr.fillRect(67 * 3, 0, 67, 120, TFT_RED);
  spr.fillRect(67 * 4, 0, 67, 120, TFT_GREEN);
  spr.fillRect(67 * 5, 0, 67, 120, TFT_BLUE);
  spr.fillRect(67 * 6, 0, 67, 120, TFT_RED);
  spr.fillRect(67 * 7, 0, 67, 120, TFT_GREEN);

  spr.fillRect(0, 120, 67, 120, TFT_BLUE);
  spr.fillRect(67 * 1, 120, 67, 120, TFT_RED);
  spr.fillRect(67 * 2, 120, 67, 120, TFT_GREEN);
  spr.fillRect(67 * 3, 120, 67, 120, TFT_BLUE);
  spr.fillRect(67 * 4, 120, 67, 120, TFT_RED);
  spr.fillRect(67 * 5, 120, 67, 120, TFT_GREEN);
  spr.fillRect(67 * 6, 120, 67, 120, TFT_BLUE);
  spr.fillRect(67 * 7, 120, 67, 120, TFT_RED);

  lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
  delay(2000);

  uint16_t colors[6] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};
  for (int i = 0; i < 6; ++i)
  {
    spr.fillSprite(TFT_BLACK);
    spr.setTextColor(colors[i], TFT_BLACK);
    spr.drawString("LilyGo.cc", WIDTH / 2 - 30, 85, 4);
    spr.drawString("T-Display AMOLED", WIDTH / 2 - 70, 110, 4);
    lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
    delay(200);
  }
  delay(2000);

  for (int pos = WIDTH; pos > 0; pos--)
  {
    int h = HEIGHT;
    while (h--)
      spr.drawFastHLine(0, h, WIDTH, rainbow(h * 4));
    spr.setTextSize(1);
    spr.setTextFont(4);
    spr.setTextColor(TFT_WHITE);
    spr.setTextWrap(false);
    spr.setCursor(pos, 100);
    spr.print("LilyGo AMOLED");
    lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
  }
  delay(2000);

  targetTime = millis() + 1000;
  uint32_t runTime = millis() + 6000;
  spr.fillSprite(TFT_BLACK);
  while (runTime > millis())
  {
    drawRainbow();
  }
  delay(2000);
}

void drawRainbow()
{
  if (targetTime < millis())
  {
    targetTime = millis() + 500;
    Serial.println(targetTime);
    // Colour changing state machine
    for (int i = 0; i < WIDTH; i++)
    {
      spr.drawFastVLine(i, 0, spr.height(), colour);
      switch (state)
      {
      case 0:
        green += 2;
        if (green == 64)
        {
          green = 63;
          state = 1;
        }
        break;
      case 1:
        red--;
        if (red == 255)
        {
          red = 0;
          state = 2;
        }
        break;
      case 2:
        blue++;
        if (blue == 32)
        {
          blue = 31;
          state = 3;
        }
        break;
      case 3:
        green -= 2;
        if (green == 255)
        {
          green = 0;
          state = 4;
        }
        break;
      case 4:
        red++;
        if (red == 32)
        {
          red = 31;
          state = 5;
        }
        break;
      case 5:
        blue--;
        if (blue == 255)
        {
          blue = 0;
          state = 0;
        }
        break;
      }
      colour = red << 11 | green << 5 | blue;
    }

    // The standard ADAFruit font still works as before
    spr.setTextColor(TFT_BLACK);
    spr.setCursor(12, 5);
    spr.print("Original ADAfruit font!");

    // The new larger fonts do not use the .setCursor call, coords are embedded
    spr.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour

    // Overlay the black text on top of the rainbow plot (the advantage of not drawing the background colour!)
    spr.drawCentreString("Font size 2", 80, 14, 2); // Draw text centre at position 80, 12 using font 2

    // spr.drawCentreString("Font size 2",81,12,2); // Draw text centre at position 80, 12 using font 2

    spr.drawCentreString("Font size 4", 80, 30, 4); // Draw text centre at position 80, 24 using font 4

    spr.drawCentreString("12.34", 80, 54, 6); // Draw text centre at position 80, 24 using font 6

    spr.drawCentreString("12.34 is in font size 6", 80, 92, 2); // Draw text centre at position 80, 90 using font 2

    // Note the x position is the top left of the font!

    // draw a floating point number
    float pi = 3.14159;                                     // Value to print
    int precision = 3;                                      // Number of digits after decimal point
    int xpos = 50;                                          // x position
    int ypos = 110;                                         // y position
    int font = 2;                                           // font number only 2,4,6,7 valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : a p m
    xpos += spr.drawFloat(pi, precision, xpos, ypos, font); // Draw rounded number and return new xpos delta for next print position
    spr.drawString(" is pi", xpos, ypos, font);             // Continue printing from new x position
    lcd_PushColors(0, 0, WIDTH, HEIGHT, (uint16_t *)spr.getPointer());
  }
}

// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(uint8_t value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = red through to 127 = blue

  uint8_t red = 0;   // Red is the top 5 bits of a 16 bit colour value
  uint8_t green = 0; // Green is the middle 6 bits
  uint8_t blue = 0;  // Blue is the bottom 5 bits

  uint8_t sector = value >> 5;
  uint8_t amplit = value & 0x1F;

  switch (sector)
  {
  case 0:
    red = 0x1F;
    green = amplit;
    blue = 0;
    break;
  case 1:
    red = 0x1F - amplit;
    green = 0x1F;
    blue = 0;
    break;
  case 2:
    red = 0;
    green = 0x1F;
    blue = amplit;
    break;
  case 3:
    red = 0;
    green = 0x1F - amplit;
    blue = 0x1F;
    break;
  }

  return red << 11 | green << 6 | blue;
}
