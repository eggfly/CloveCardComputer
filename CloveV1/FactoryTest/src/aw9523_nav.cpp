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

void loop_aw9523()
{
  bool values[5] = {
      aw.digitalRead(0),
      aw.digitalRead(1),
      aw.digitalRead(2),
      aw.digitalRead(3),
      aw.digitalRead(4),
  };
  bool hasKey = true;
  if (!values[0])
  {
    printf("top");
  }
  else if (!values[1])
  {
    printf("left");
  }
  else if (!values[2])
  {
    printf("right");
  }
  else if (!values[3])
  {
    printf("down");
  }
  else if (!values[4])
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

  delay(50);
}