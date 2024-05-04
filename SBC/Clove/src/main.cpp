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
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(45, ledChannel);
  ledcWrite(ledChannel, 230);

  setup_graphic();
}

void loop(void)
{
  loop_graphic();
  // Serial.println("loop...");
}
