
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
  Serial.begin(115200);
  // delay(1000);
  Serial.println("setup");
  setup_i2c();
  setup_pmu();
  setup_tca8418();
  setup_pcf8574();
}


void loop(void)
{
  // delay(5000);
  // loop_i2c(Wire);
  // loop_i2c(Wire1);
  loop_tca8418();
  loop_pcf8574();
  // Serial.println("\n\n");
}
