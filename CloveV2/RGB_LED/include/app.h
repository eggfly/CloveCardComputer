#ifndef APP_H
#define APP_H

#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI
#include <OpenFontRender.h>
#include <Arduino_GFX_Library.h>

#include <esp_partition.h>

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"

#define WIDTH 536
#define HEIGHT 240

extern TFT_eSprite spr;
extern OpenFontRender render;
extern const void *font_map_ptr;

extern XPowersAXP2101 PMU;

extern bool keypad_states[5];

void setup_keypad();
void loop_keypad();

void setup_pmu();
void loop_pmu();

void setup_amoled();
void loop_amoled();

void setup_ina219(bool isVbusOrVbat);
void loop_ina219(bool isVbusOrVbat);

void swapBytes(uint8_t *input, uint8_t *output, int size);

void setup_ws2812();
void loop_ws2812();


void flush_screen();

enum
{
    GAMEPAD_UP = 0,
    GAMEPAD_LEFT,
    GAMEPAD_RIGHT,
    GAMEPAD_DOWN,
    GAMEPAD_PRESSED,

    GAMEPAD_MAX,
};

#endif // APP_H
