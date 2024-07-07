

#ifndef CLOVE_EXTERNAL_API_H
#define CLOVE_EXTERNAL_API_H


#define AMOLED_WIDTH 536
#define AMOLED_HEIGHT 240

extern bool keypad_states[5];
extern uint8_t keyboard_states[7][10];

// eggfly
void mmap_font_partition();
void setup_amoled();

void setup_pmu();

void setup_pcf8574();
void loop_pcf8574();
void setup_tca8418();
void loop_tca8418();


#endif

