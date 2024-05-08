
#include "rm67162.h"
#include <TFT_eSPI.h>   //https://github.com/Bodmer/TFT_eSPI
#include "true_color.h"


#include <Arduino.h>
#include <app.h>

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(100);
  Serial.println("Serial.begin() called.");
  const int freq = 5000;
  const int ledChannel = 0;
  const int resolution = 8;

  setup_pmu();
  setup_amoled();

  // setup_keypad();
  // setup_graphic();
}

void loop(void)
{
  // loop_keypad();
  // Serial.println("loop...");
  loop_amoled();
  delay(1);
}
