


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

#define VDD_FM_VOLTAGE 3300


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
  PMU.disableALDO1();
  // PMU.enableALDO1();

  // Because BLDO2 Input is DC3, Set The Voltage to 3.3V
  PMU.setDC3Voltage(3300);
  PMU.enableDC3();
  // PMU.disableDC3();

  printf("AXP2101 Power Initialized.\n");
}

void loop_pmu()
{
}
