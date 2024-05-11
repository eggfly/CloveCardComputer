#ifndef APP_H
#define APP_H

#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI
#include <OpenFontRender.h>
#include "rm67162.h"

#include <esp_partition.h>

#define WIDTH 536
#define HEIGHT 240

extern TFT_eSprite spr;
extern OpenFontRender render;
extern const void *font_map_ptr;


void setup_i2s();
void loop_i2s();

void setup_keypad();
void loop_keypad();

void setup_pmu();

void setup_amoled();
void loop_amoled();

void setup_aw9523();
void loop_aw9523();

const esp_partition_t* find_partition(esp_partition_type_t type, esp_partition_subtype_t subtype, const char* name);

#endif // APP_H
