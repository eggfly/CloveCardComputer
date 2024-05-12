#include <Adafruit_AW9523.h>

Adafruit_AW9523 aw;

void setup_aw9523()
{
  printf("Adafruit AW9523 test!\n");

  if (!aw.begin(0x5B))
  {
    printf("AW9523 not found? Check wiring!\n");
    while (1)
      delay(10); // halt forever
  }

  printf("AW9523 found!\n");
  aw.pinMode(0, INPUT);
  aw.pinMode(1, INPUT);
  aw.pinMode(2, INPUT);
  aw.pinMode(3, INPUT);
  aw.pinMode(4, INPUT);
  // aw.enableInterrupt(ButtonPin, true);
}

unsigned long last_update_time = 0;
bool keypad_states[5];

void loop_aw9523()
{
  if (millis() - last_update_time < 50)
  {
    return;
  }
  last_update_time = millis();
  for (uint8_t i = 0; i < 5; i++)
  {
    keypad_states[i] = !aw.digitalRead(i);
  }
  bool hasKey = true;
  if (keypad_states[0])
  {
    printf("top");
  }
  else if (keypad_states[1])
  {
    printf("left");
  }
  else if (keypad_states[2])
  {
    printf("right");
  }
  else if (keypad_states[3])
  {
    printf("down");
  }
  else if (keypad_states[4])
  {
    printf("press");
  }
  else
  {
    hasKey = false;
  }
  if (hasKey)
  {
    printf("\n");
  }
}
