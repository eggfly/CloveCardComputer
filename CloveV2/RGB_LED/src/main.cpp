
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI

#include "esp_err.h"

#include "esp_partition.h"
#include "esp_log.h"
#include <app.h>
#include <Arduino.h>

static const char *TAG = "main";


void setup_i2c();
void loop_i2c(TwoWire &wire);

void setup()
{
  setup_pmu();
  setup_i2c();
  setup_ina219(true);
  setup_ina219(false);
  setup_ws2812();
}

void loop(void)
{
  loop_ina219(true);
  loop_ina219(false);
  loop_ws2812();
}
