


#include "XPowersLib.h"

#include <Wire.h>
#include <Arduino.h>

XPowersAXP2101 PMU;

#ifndef CONFIG_PMU_SDA
#define CONFIG_PMU_SDA 2
#endif

#ifndef CONFIG_PMU_SCL
#define CONFIG_PMU_SCL 3
#endif

#ifndef CONFIG_PMU_IRQ
#define CONFIG_PMU_IRQ 38
#endif

const uint8_t i2c_sda = CONFIG_PMU_SDA;
const uint8_t i2c_scl = CONFIG_PMU_SCL;
const uint8_t pmu_irq_pin = CONFIG_PMU_IRQ;

#define BL_MIN_VOLTAGE 2500

uint16_t vol = BL_MIN_VOLTAGE;

void setup_pmu()
{
  bool result = PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);

  if (result == false)
  {
    printf("PMU is not online...\n");
    while (1)
    {
      delay(50);
    }
  }

  // Disable SPEAKER by default.
  // PMU.disableALDO1();
  PMU.enableALDO1();

  printf("AXP2101 Power Initialized.\n");
}

void loop_pmu()
{
  // ALDO2 IMAX=300mA
  // 500~3500mV, 100mV/step,31steps
  PMU.setALDO2Voltage(vol);
  delay(1);
  uint16_t targetVol = PMU.getALDO2Voltage();
  printf("ALDO2  :%s   Voltage:%u mV \n", PMU.isEnableALDO2() ? "ENABLE" : "DISABLE", targetVol);
  if (targetVol != vol)
  {
    Serial.println(">>> FAILED!");
  }
  vol += 100;
  if (vol >= 3300)
  {
    vol = BL_MIN_VOLTAGE;
  }
  delay(500);
}
