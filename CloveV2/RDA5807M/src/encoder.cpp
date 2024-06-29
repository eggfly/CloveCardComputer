#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder.git
#include <Arduino.h>

#define CLK 12 // CLK ENCODER
#define DT 13  // DT ENCODER

ESP32Encoder encoder;

void setup_encoder()
{
    encoder.attachHalfQuad(DT, CLK);
    encoder.setCount(0);
}

static unsigned long update_time = 0;
static int64_t prev_position = 0;

void loop_encoder()
{
    if (update_time + 10 < millis())
    {
        update_time = millis();
        auto pos = encoder.getCount() / 2;
        if (prev_position != pos) {
            Serial.printf("Encoder position=%d\n", pos);
            prev_position = pos;
        }
    }
}
