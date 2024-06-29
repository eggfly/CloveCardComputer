
// IIC
#define IIC_SDA 2
#define IIC_SCL 3

// TOUCH
#define TP_INT 10
#define TP_RST 8

#include "Arduino_DriveBus_Library.h"

static size_t CycleTime = 0;

std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

void Arduino_IIC_Touch_Interrupt(void);

std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));

void Arduino_IIC_Touch_Interrupt(void)
{
    FT3168->IIC_Interrupt_Flag = true;
}

void setup_ft3168()
{
    while (FT3168->begin() == false)
    {
        Serial.println("FT3168 initialization fail");
        delay(2000);
    }
    Serial.println("FT3168 initialization successfully");

    // 触摸芯片功耗模式选择
    // 激活模式
    // FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
    //                                FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_ACTIVE);
    // 监听触发模式
    FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
                                   FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);
    // 待机模式
    // FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
    //                                FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_STANDBY);
    // 休眠模式
    // FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
    //                                FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_HIBERNATE);

    // 开启特殊手势模式
    // FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_GESTUREID_MODE,
    //                                FT3168->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);
    // 开启近距离感应模式
    // FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_PROXIMITY_SENSING_MODE,
    //                                FT3168->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);

    // 触摸芯片是否自动进入Monitor模式
    // FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_AUTOMATICALLY_MONITOR_MODE,
    //                                FT3168->Arduino_IIC_Touch::Device_State::TOUCH_DEVICE_ON);
    // 设定触摸芯片自动进入Monitor模式的时间为10秒
    // FT3168->IIC_Write_Device_Value(FT3168->Arduino_IIC_Touch::Device_Value::TOUCH_AUTOMATICALLY_MONITOR_TIME,
    //                                10);

    Serial.printf("ID: %#X \n\n", (int32_t)FT3168->IIC_Read_Device_ID());
    delay(1000);
}

void loop_ft3168()
{
    // Serial.printf("System running time: %d\n\n", (uint32_t)millis() / 1000);

    if (FT3168->IIC_Interrupt_Flag)
    {
        FT3168->IIC_Interrupt_Flag = false;

        Serial.printf("Device ID: %#X \n", (int32_t)FT3168->IIC_Read_Device_ID());

        Serial.printf("\nGesture:%s\n",
                      (FT3168->IIC_Read_Device_State(FT3168->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID)).c_str());
        auto fingers_count = (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);
        Serial.printf("Fingers count:%d\n", fingers_count);

        Serial.printf("Touch X:%d Y:%d\n",
                      (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X),
                      (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y));
        auto x1 = (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH1_COORDINATE_X);
        auto y1 = (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH1_COORDINATE_Y);
        auto x2 = (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH2_COORDINATE_X);
        auto y2 = (int32_t)FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH2_COORDINATE_Y);
        Serial.printf("\nTouch X1:%d Y1:%d\n", x1, y1);
        Serial.printf("Touch X2:%d Y2:%d\n", x2, y2);
    }

    delay(20);

    if (millis() > CycleTime)
    {
        CycleTime = millis() + 5000;
    }
}
