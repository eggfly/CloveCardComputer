#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219_vbus(0x40);
Adafruit_INA219 ina219_vbat(0x41);

void setup_ina219(bool isVbusOrVbat)
{
    Adafruit_INA219 &ina219 = isVbusOrVbat? ina219_vbus: ina219_vbat;
    // Initialize the INA219.
    // By default the initialization will use the largest range (32V, 2A).  However
    // you can call a setCalibration function to change this range (see comments).
    if (!ina219.begin())
    {
        Serial.println("Failed to find INA219 chip");
        while (1)
        {
            delay(10);
        }
    }
    // To use a slightly lower 32V, 1A range (higher precision on amps):
    // ina219.setCalibration_32V_1A();
    // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
    // ina219.setCalibration_16V_400mA();

    Serial.println("Measuring voltage and current with INA219 ...");
}

long update_time[2] = {0, 0};

void loop_ina219(bool isVbusOrVbat)
{
    Adafruit_INA219 &ina219 = isVbusOrVbat? ina219_vbus: ina219_vbat;
    uint8_t index = isVbusOrVbat? 0: 1;
    if (millis() < update_time[index])
    {
        return;
    }
    update_time[index] = millis() + 1000;
    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;

    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    power_mW = ina219.getPower_mW();
    loadvoltage = busvoltage + (shuntvoltage / 1000);

    Serial.print("Bus Voltage:   ");
    Serial.print(busvoltage);
    Serial.println(" V");
    Serial.print("Shunt Voltage: ");
    Serial.print(shuntvoltage);
    Serial.println(" mV");
    Serial.print("Load Voltage:  ");
    Serial.print(loadvoltage);
    Serial.println(" V");
    Serial.print("Current:       ");
    Serial.print(current_mA);
    Serial.println(" mA");
    Serial.print("Power:         ");
    Serial.print(power_mW);
    Serial.println(" mW");
    Serial.println("");
}
