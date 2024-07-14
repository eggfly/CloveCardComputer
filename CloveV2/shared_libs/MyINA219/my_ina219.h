#ifndef MY_INA219_H
#define MY_INA219_H

typedef struct
{
    float shuntVoltage;
    float busVoltage;
    float current_mA;
    float loadVoltage;
    float power_mW;
} INA219_Data;

extern INA219_Data ina219_data[2];


#endif
