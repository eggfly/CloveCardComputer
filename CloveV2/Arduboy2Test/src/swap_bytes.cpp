#include <Arduino.h>
void swapBytes(uint8_t *input, uint8_t *output, int size) {
    for (int i = 0; i < size; i += 2) {
        // 读取LSB和MSB
        uint8_t lsb = input[i];
        uint8_t msb = input[i + 1];

        // 对调LSB和MSB
        output[i] = msb;
        output[i + 1] = lsb;
    }
}