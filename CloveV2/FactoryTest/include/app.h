#ifndef APP_H
#define APP_H

#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI
#include <OpenFontRender.h>
#include "rm67162.h"
#include <Arduino_GFX_Library.h>
#include <Adafruit_PCF8574.h>
#include <esp_partition.h>
#include <ESP32Encoder.h>
#include <RDA5807M.h>

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"

#define WIDTH 536
#define HEIGHT 240

extern TFT_eSprite spr;
extern OpenFontRender render;
extern const void *font_map_ptr;

extern XPowersAXP2101 PMU;
extern Arduino_Canvas *gfx;

extern bool keypad_states[5];
extern Adafruit_PCF8574 pcf;

extern int32_t tp_fingers_count, tp_x1, tp_y1, tp_x2, tp_y2;


extern int prevBatteryPercent;
extern uint16_t prevBatteryVoltage;

void setup_pmu();
void loop_pmu();

void setup_amoled();
void loop_touch_amoled();
void test_amoled();

void setup_pcf8574();
void loop_pcf8574();

void setup_ina219(bool isVbusOrVbat);
void loop_ina219(bool isVbusOrVbat);

const esp_partition_t *find_partition(esp_partition_type_t type, esp_partition_subtype_t subtype, const char *name);

void swapBytes(uint8_t *input, uint8_t *output, int size);

void setup_spm1423();
void loop_spm1423();

void setup_music_player();
void loop_music_player();

void flush_screen();

void setup_ft3168();
void loop_ft3168();

extern RDA5807M radio;

void setup_rda5807m();
void loop_rda5807m();
void loop_radio_ui();

void setup_headphone_jack_detect();
void is_headphone_jack_inserted();

extern ESP32Encoder encoder;

void setup_encoder();

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
