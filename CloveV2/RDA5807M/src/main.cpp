
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI

#include "esp_err.h"

#include "esp_partition.h"
#include "esp_log.h"
#include <app.h>
#include <Arduino.h>

static const char *TAG = "main";

// 93.90,
void setup()
{
  setup_pmu();
  setup_i2c();
  setup_rda5807m();
  setup_ina219(true);
  setup_ina219(false);
  setup_encoder();
}

static int32_t prev_encoder_count = 0;
void loop(void)
{
  // loop_pmu();
  loop_rda5807m();
  // loop_ina219(true);
  // loop_ina219(false);
  loop_encoder();
  int32_t count = static_cast<int32_t>(encoder.getCount() / 2);
  if (count != prev_encoder_count)
  {
    Serial.printf("count=%d\n", count);
    prev_encoder_count = count;
    auto fMin = radio.getMinFrequency();
    auto fMax = radio.getMaxFrequency();
    auto step = radio.getFrequencyStep();
    auto fCurr = radio.getFrequency();
    auto fMid = fMin + (fMax - fMin) / 2;
    auto fNew = fMid + count * step;
    fNew = constrain(fNew, fMin, fMax);
    radio.setFrequency(fNew);
    Serial.printf("count=%d, freq=%.2fMHz, new=%.2fMHz\n", count, fCurr / 100.0, fNew / 100.0);
  }
}
