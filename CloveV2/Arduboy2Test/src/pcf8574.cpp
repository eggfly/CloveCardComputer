#include <Adafruit_PCF8574.h>
#include <clove_pin_config.h>
#include <app.h>
/* Example for 8 input buttons that are connected from the GPIO expander pins to ground.
 * Note the buttons must be connected with the other side of the switch to GROUND. There is
 * a built in pull-up 'resistor' on each input, but no pull-down resistor capability.
 */

bool keypad_states[5];

Adafruit_PCF8574 pcf;

// P0: BTN_UP
// P1: BTN_RIGHT
// P2: BTN_LEFT
// P3: BTN_DOWN

// P4: ENCODER_KEY
// P5: LCD_RESET
// P6: 4V6_EN
// P7: TP_RST

#define PCF8574_TP_RST 7
#define PCF8574_LCD_RST 5

void setup_pcf8574()
{
    Serial.println("Adafruit PCF8574 button read test");

    if (!pcf.begin(0x20, &Wire))
    {
        Serial.println("Couldn't find PCF8574");
        while (1)
        {
        }
    }
    for (uint8_t p = 0; p < 4; p++)
    {
        pcf.pinMode(p, INPUT_PULLUP); // INPUT or INPUT_PULLUP are same.
    }
    pcf.pinMode(PCF8574_LCD_RST, OUTPUT);
    pcf.digitalWrite(PCF8574_LCD_RST, LOW);
    delay(300);
    pcf.digitalWrite(PCF8574_LCD_RST, HIGH);
    delay(200);
}

void loop_pcf8574()
{
    for (uint8_t p = 0; p < 4; p++)
    {
        auto state = pcf.digitalRead(p);
        if (!state)
        {
            // Serial.print("Button on GPIO #");
            // Serial.print(p);
            // Serial.println(" pressed!");
        }
        keypad_states[p] = !state;
    }
}
