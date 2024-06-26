#include <Wire.h>
#include <Arduino.h>

void setup_i2c()
{
  Serial.begin(115200);
  Wire.begin(2, 3);
  Wire1.begin(43, 44);
}

void loop_i2c(TwoWire &wire)
{
  byte error, address;
  int nDevices = 0;

  Serial.printf("%s: Scanning for I2C devices ...\n", &wire == &Wire ? "Wire0" : "Wire1");
  for (address = 0x01; address < 0x7f; address++)
  {
    wire.beginTransmission(address);
    error = wire.endTransmission();
    if (error == 0)
    {
      Serial.printf("I2C device found at address 0x%02X\n", address);
      nDevices++;
    }
    else if (error != 2)
    {
      Serial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0)
  {
    Serial.println("No I2C devices found");
  }
}