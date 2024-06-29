

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

  // Shutdown SPEAKER
  // PMU.disableALDO1();
  // ENABLE SPEAKER
  PMU.enableALDO1();

  PMU.enableALDO2();

  // Because BLDO2 Input is DC3, Set The Voltage to 3.3V
  PMU.setDC3Voltage(3300);
  // AUDIO_SEL as FM Input
  PMU.enableBLDO2();

  printf("AXP2101 Power Initialized.\n");

  // ALDO2 IMAX=300mA
  // 500~3500mV, 100mV/step,31steps
  PMU.setALDO4Voltage(VDD_FM_VOLTAGE);
  PMU.enableALDO4();
  delay(1);
  uint16_t targetVol = PMU.getALDO4Voltage();
  printf("ALDO4  :%s   Voltage:%u mV \n", PMU.isEnableALDO4() ? "ENABLE" : "DISABLE", targetVol);
  if (targetVol != VDD_FM_VOLTAGE)
  {
    Serial.println(">>> FAILED!");
  }

  PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);
  PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);
}

long updateTime = 0;

void loop_pmu()
{
  if (updateTime < millis())
  { // Update every 2s
    updateTime = millis() + 2000;

    auto target = PMU.getChargeTargetVoltage();
    auto curr = PMU.getChargerConstantCurr();
    auto isLimit = PMU.isChargerTerminationLimit();
    auto terminationCurr = PMU.getChargerTerminationCurr();
    Serial.printf("Charge Target Voltage: enum=%u\n", target);
    Serial.printf("Charge Constant Current: %u mA\n", curr);
    Serial.printf("Charge Termination Limit: %s\n", isLimit ? "ENABLE" : "DISABLE");
    Serial.printf("Charge Termination Current: %u mA\n", terminationCurr);
  }
}
