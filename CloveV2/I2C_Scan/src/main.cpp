
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
}

void loop(void)
{
  delay(5000);
  loop_i2c(Wire);
  loop_i2c(Wire1);
  Serial.println("\n\n");
}
