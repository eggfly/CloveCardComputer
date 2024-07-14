

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
  auto targetVbat = PMU.getChargeTargetVoltage();

  /*     XPOWERS_AXP2101_CHG_VOL_4V = 1,
    XPOWERS_AXP2101_CHG_VOL_4V1,
    XPOWERS_AXP2101_CHG_VOL_4V2,
    XPOWERS_AXP2101_CHG_VOL_4V35,
    XPOWERS_AXP2101_CHG_VOL_4V4,
     */
  switch (targetVbat)
  {
  case XPOWERS_AXP2101_CHG_VOL_4V1:
    Serial.printf("AXP2101 Target VBAT Voltage = 4.1V\n");
    break;

  case XPOWERS_AXP2101_CHG_VOL_4V2:
    Serial.printf("AXP2101 Target VBAT Voltage = 4.2V\n");
    break;

  default:
    Serial.printf("AXP2101 Target VBAT Voltage enum = %d\n", targetVbat);
    break;
  }
  if (targetVbat != XPOWERS_AXP2101_CHG_VOL_4V2) {
    PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V2);
  }
  printf("AXP2101 Power Initialized.\n");
}

int prevBatteryPercent;
uint16_t prevBatteryVoltage;
long batteryPercentUpdateTime;

void loop_pmu()
{
  auto time = millis();
  if (time - batteryPercentUpdateTime < 1000)
  {
    return;
  }
  batteryPercentUpdateTime = time;
  auto percent = PMU.getBatteryPercent();
  auto vbat = PMU.getBattVoltage();
  if (percent != prevBatteryPercent)
  {
    Serial.printf("BatteryPercent: %d %%\n", percent);
  }
  prevBatteryPercent = percent;

  if (vbat != prevBatteryVoltage)
  {
    Serial.printf("BatteryVoltage: %d mV\n", vbat);
  }
  prevBatteryVoltage = vbat;
}
