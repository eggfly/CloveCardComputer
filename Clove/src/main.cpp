#include <Arduino.h>
#include <app.h>

void setup()
{
  Serial.begin(115200);

  const int freq = 5000;
  const int ledChannel = 0;
  const int resolution = 8;
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(45, ledChannel);
  ledcWrite(ledChannel, 230);

  setup_i2s();
}

void loop(void)
{
  loop_i2s();
  Serial.println("loop...");
  // delay(1000);
}
