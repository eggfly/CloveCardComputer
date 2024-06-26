
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI

#include "esp_err.h"

#include "esp_partition.h"
#include "esp_log.h"

#include <Arduino.h>

static const char *TAG = "main";


void setup_i2c();
void loop_i2c();

void setup()
{
  setup_i2c();
}

void loop(void)
{
  loop_i2c();
}
