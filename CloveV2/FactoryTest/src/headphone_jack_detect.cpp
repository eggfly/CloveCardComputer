#include <Arduino.h>
#include <clove_pin_config.h>

void setup_headphone_jack_detect() {
    pinMode(PJ_DETECT, INPUT);
}

bool is_headphone_jack_inserted() {
    auto voltage = analogRead(PJ_DETECT);
    Serial.printf("Headphone jack voltage: %d\n", voltage);
    return voltage > 1024;
}
