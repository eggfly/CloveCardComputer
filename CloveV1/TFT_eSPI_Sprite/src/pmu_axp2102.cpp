

#define XPOWERS_CHIP_AXP2101

#include <Wire.h>
#include <Arduino.h>
#include "XPowersLib.h"


XPowersPMU PMU;

#ifndef CONFIG_PMU_SDA
#define CONFIG_PMU_SDA 43
#endif

#ifndef CONFIG_PMU_SCL
#define CONFIG_PMU_SCL 44
#endif

#ifndef CONFIG_PMU_IRQ
#define CONFIG_PMU_IRQ 38
#endif

const uint8_t i2c_sda = CONFIG_PMU_SDA;
const uint8_t i2c_scl = CONFIG_PMU_SCL;
const uint8_t pmu_irq_pin = CONFIG_PMU_IRQ;

uint16_t targetVol;
uint16_t vol = 0;


void setup_pmu()
{
  bool result = PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);

  if (result == false) {
    Serial.println("PMU is not online...");
    while (1) {
      delay(50);
    }
  }

  // Disable SPEAKER by default.
  PMU.disableALDO1();

  // PMU.enableALDO2();

  //ALDO2 IMAX=300mA
  //500~3500mV, 100mV/step,31steps
  // uint16_t vol = 500;
  // for (int i = 0; i < 31; ++i) {
  //   PMU.setALDO2Voltage(vol);
  //   delay(1);
  //   targetVol = PMU.getALDO2Voltage();
  //   Serial.printf("[%u]ALDO2  :%s   Voltage:%u mV \n", i,  PMU.isEnableALDO2()  ? "ENABLE" : "DISABLE", targetVol );
  //   if (targetVol != vol)Serial.println(">>> FAILED!");
  //   vol += 100;
  // }
  Serial.println("AXP2101 Power Initialized.");
}

void loop_pmu()
{
}
