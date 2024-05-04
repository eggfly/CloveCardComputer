#ifndef APP_H
#define APP_H

void setup_i2s();
void loop_i2s();
void setup_graphic();
void loop_graphic();

void printnice(int32_t v);
uint32_t testFilledRoundRects();

uint32_t testRoundRects();
uint32_t testFilledRects(uint16_t color1, uint16_t color2);
uint32_t testFilledTriangles();
uint32_t testFilledCircles(uint8_t radius, uint16_t color);
uint32_t testTriangles();
uint32_t testText();
uint32_t testFillScreen();
uint32_t testPixels();
uint32_t testCircles(uint8_t radius, uint16_t color);
uint32_t testRects(uint16_t color);
uint32_t testLines(uint16_t color);
uint32_t testFastLines(uint16_t color1, uint16_t color2);


#endif // APP_H
