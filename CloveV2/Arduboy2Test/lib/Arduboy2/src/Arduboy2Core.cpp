/**
 * @file Arduboy2Core.cpp
 * \brief
 * The Arduboy2Core class for Arduboy hardware initilization and control.
 */

#include "Arduboy2Core.h"
#include "CloveExternalAPI.h"

#ifdef ONEBUTTON
#include "Button2.h"
#endif

#ifdef PS3GAMEPAD
#include <Ps3Controller.h>
#endif
#include <Wire.h>

#ifdef ESP32
static uint64_t inputlastTime = 0;
static uint64_t inputcurrentTime = 0;
static uint64_t inputframeTime = 0;
extern uint64_t inputfps;

#endif

#ifdef ADAFRUIT
Adafruit_MCP23017 mcp;
Adafruit_MCP4725 dac;
#endif

#ifdef ESP8266
ESPboyLED myled;
#endif

uint8_t Arduboy2Core::sBuffer[];

Arduboy2Core::Arduboy2Core() {}

void Arduboy2Core::boot()
{
  Serial.begin(115200);
#ifdef ESP32
  esp_timer_init();
#endif
  delay(100);
  Wire.begin(2, 3);
  Wire1.begin(43, 44);
  setup_pcf8574();
  // setup_pmu();
  mmap_font_partition();
  setup_tca8418();
  delay(100);
  // WiFi.mode(WIFI_OFF);
  // LED init

  Serial.println("Screen Init");
  // delay(100);
  setup_amoled();
  Serial.println("Boot Done!");
}

void Arduboy2Core::setCPUSpeed8MHz() {};
void Arduboy2Core::bootPins() {};
void Arduboy2Core::bootOLED() {};
void Arduboy2Core::LCDDataMode() {};
void Arduboy2Core::LCDCommandMode() {};
void Arduboy2Core::bootSPI() {}
void Arduboy2Core::SPItransfer(uint8_t data) {};
void Arduboy2Core::safeMode() {}
void Arduboy2Core::bootPowerSaving() {};
void Arduboy2Core::sendLCDCommand(uint8_t command) {};
void Arduboy2Core::exitToBootloader() {};
void Arduboy2Core::mainNoUSB() {};
// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void Arduboy2Core::allPixelsOn(bool on) {};
void Arduboy2Core::blank() {};
// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void Arduboy2Core::invert(bool inverse) {};
// flip the display vertically or set to normal
void Arduboy2Core::flipVertical(bool flipped) {};
// flip the display horizontally or set to normal
void Arduboy2Core::flipHorizontal(bool flipped) {};
void Arduboy2Core::paint8Pixels(uint8_t pixels) {}
void Arduboy2Core::freeRGBled() {};

/* Power Management */
void Arduboy2Core::idle()
{
  delay(1);
};

// Shut down the display
void Arduboy2Core::displayOff()
{
#ifdef ADAFRUIT
  dac.setVoltage(0, false);
#endif
};

// Restart the display after a displayOff()
void Arduboy2Core::displayOn()
{
#ifdef ADAFRUIT
  dac.setVoltage(4095, false);
#endif
};

uint8_t Arduboy2Core::width() { return ARDUBOY2_WIDTH; }

uint8_t Arduboy2Core::height() { return ARDUBOY2_HEIGHT; }

/* Drawing */

void Arduboy2Core::paintScreen(const uint8_t *image) {
  // memcpy(sBuffer, image, HEIGHT*WIDTH/8);
};

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
//
// The following assembly code runs "open loop". It relies on instruction
// execution times to allow time for each byte of data to be clocked out.
// It is specifically tuned for a 16MHz CPU clock and SPI clocking at 8MHz.
void Arduboy2Core::paintScreen(uint8_t image[], bool clear) {
  //  memcpy(sBuffer, image, HEIGHT*WIDTH/8);
  //  if (clear) memset(sBuffer, 0, HEIGHT*WIDTH/8);
};

/* RGB LED */
void Arduboy2Core::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ESP8266
  myled.setRGB(red, green, blue);
#endif
};

void Arduboy2Core::setRGBled(uint8_t color, uint8_t val)
{
#ifdef ESP8266
  if (color == RED_LED)
    myled.setR(val);
  else if (color == GREEN_LED)
    myled.setG(val);
  else if (color == BLUE_LED)
    myled.setB(val);
#endif
};

void Arduboy2Core::digitalWriteRGB(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ESP8266
  if (red)
    myled.setR(200);
  else
    myled.setR(0);
  if (green)
    myled.setG(200);
  else
    myled.setG(0);
  if (blue)
    myled.setB(200);
  else
    myled.setB(0);
#endif
};

void Arduboy2Core::digitalWriteRGB(uint8_t color, uint8_t val)
{
#ifdef ESP8266
  if (color == 0)
    if (val)
      myled.setR(200);
    else
      myled.setR(0);
  if (color == 1)
    if (val)
      myled.setG(200);
    else
      myled.setG(0);
  if (color == 2)
    if (val)
      myled.setB(200);
    else
      myled.setB(0);
#endif
}

#ifdef ESP32
// Button definitions
static const int P1_Left = 0;
static const int P1_Top = 1;
static const int P1_Right = 2;
static const int P1_Bottom = 3;

static const int P2_Right = 4;
static const int P2_Bottom = 5;
static const int P2_Left = 6;
static const int P2_Top = 7;

static const int BIT_P1_Left = 1;
static const int BIT_P1_Top = 2;
static const int BIT_P1_Right = 4;
static const int BIT_P1_Bottom = 8;

static const int BIT_P2_Right = 16;
static const int BIT_P2_Bottom = 32;
static const int BIT_P2_Left = 64;
static const int BIT_P2_Top = 128;
static bool inputSetup = false;

#ifdef ONEBUTTON
static bool buttonRun = false;
static Button2 pBtns = Button2(39);
static byte ePaperButtons = 0;

void button_callback(Button2 &b)
{
  bool aButton = false;
  bool bButton = false;
  bool cButton = false;

  unsigned int time = b.wasPressedFor();
  if (time > 3000)
  {
  }
  else if (time > 1500)
  {
    cButton = true;
  }
  else if (time > 500)
  {
    bButton = true;
  }
  else
  {
    aButton = true;
  }

  // Serial.write(printf("a %d : b %d : c %d \r\n", aButton, bButton, cButton));

  byte buttonVals = 0;

  buttonVals = buttonVals | (false << P1_Left);
  buttonVals = buttonVals | (false << P1_Top);
  buttonVals = buttonVals | (aButton << P1_Right);
  buttonVals = buttonVals | (false << P1_Bottom);

  buttonVals = buttonVals | (bButton << P2_Right);
  buttonVals = buttonVals | (false << P2_Bottom);
  buttonVals = buttonVals | (cButton << P2_Left);
  buttonVals = buttonVals | (false << P2_Top);

  ePaperButtons = buttonVals;
}

void button_init()
{
  pBtns.setPressedHandler(button_callback);
}

void button_loop()
{
  pBtns.loop();
}

static byte buttonCheck()
{
  if (!buttonRun)
  {
    button_init();
    buttonRun = true;
  }
  else
  {
    button_loop();
  }

  return ePaperButtons;
}
#elif defined(GAMEPAD)
static const byte TOUCH_SENSE_MAX = 50;
static const byte TOUCH_SENSE_MIN = 20;
static int inputVal = 0;
static int TOUCH_SENSE[8];
static int touch_values[8];

static bool readAnalogSensor(int pin, int touch_sense)
{
  inputVal = touchRead(pin);
  return inputVal < touch_sense && inputVal > 0;
}

static int readAnalogSensorRaw(int pin)
{
  inputVal = touchRead(pin);
  return inputVal;
}

static byte getReadShiftAnalog()
{
  byte buttonVals = 0;
  buttonVals = buttonVals | (readAnalogSensor(32, TOUCH_SENSE[0]) << P1_Left);
  buttonVals = buttonVals | (readAnalogSensor(14, TOUCH_SENSE[1]) << P1_Top);
  buttonVals = buttonVals | (readAnalogSensor(27, TOUCH_SENSE[2]) << P1_Right);
  buttonVals = buttonVals | (readAnalogSensor(33, TOUCH_SENSE[3]) << P1_Bottom);

  buttonVals = buttonVals | (readAnalogSensor(15, TOUCH_SENSE[4]) << P2_Right);
  buttonVals = buttonVals | (readAnalogSensor(13, TOUCH_SENSE[5]) << P2_Bottom);
  buttonVals = buttonVals | (readAnalogSensor(2, TOUCH_SENSE[6]) << P2_Left);
  buttonVals = buttonVals | (readAnalogSensor(12, TOUCH_SENSE[7]) << P2_Top);

  return buttonVals;
}

static void getRawInput()
{
  for (int i = 0; i < 8; i++)
  {
    touch_values[i] = 0;
  }

  int i = 0;
  touch_values[i++] = readAnalogSensorRaw(32); // Left
  touch_values[i++] = readAnalogSensorRaw(14); // Up
  touch_values[i++] = readAnalogSensorRaw(27); // Right
  touch_values[i++] = readAnalogSensorRaw(33); // Down
  touch_values[i++] = readAnalogSensorRaw(15); // A
  touch_values[i++] = readAnalogSensorRaw(13); // Start
  touch_values[i++] = readAnalogSensorRaw(2);  // B
  touch_values[i++] = readAnalogSensorRaw(12); // Select
}
#elif defined(PS3GAMEPAD)
static bool buttonRun = false;
int player = 0;
int battery = 0;

void onConnect()
{
  Serial.println("PS3 GamePad Connected.");
  Serial.print("Setting LEDs to player ");
  Serial.println(player, DEC);
  Ps3.setPlayer(player);
  player += 1;
  if (player > 10)
    player = 0;
}

bool ps3left = 0;
bool ps3right = 0;
bool ps3up = 0;
bool ps3down = 0;
bool ps3cross = 0;
bool ps3square = 0;
bool ps3circle = 0;
bool ps3triangle = 0;

void notify()
{
  if (Ps3.event.button_down.up)
    ps3up = 1;
  if (Ps3.event.button_up.up)
    ps3up = 0;

  if (Ps3.event.button_down.down)
    ps3down = 1;
  if (Ps3.event.button_up.down)
    ps3down = 0;

  if (Ps3.event.button_down.left)
    ps3left = 1;
  if (Ps3.event.button_up.left)
    ps3left = 0;

  if (Ps3.event.button_down.right)
    ps3right = 1;
  if (Ps3.event.button_up.right)
    ps3right = 0;

  if (Ps3.event.button_down.cross)
    ps3cross = 1;
  if (Ps3.event.button_up.cross)
    ps3cross = 0;

  if (Ps3.event.button_down.square)
    ps3square = 1;
  if (Ps3.event.button_up.square)
    ps3square = 0;

  if (Ps3.event.button_down.triangle)
    ps3triangle = 1;
  if (Ps3.event.button_up.triangle)
    ps3triangle = 0;

  if (Ps3.event.button_down.circle)
    ps3circle = 1;
  if (Ps3.event.button_up.circle)
    ps3circle = 0;
}

void ps3gamepad_init()
{
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("01:02:03:04:05:06");
}

byte ps3gamepad_loop()
{
  byte buttonVals = 0;

  if (!Ps3.isConnected())
    return 0;

  buttonVals = buttonVals | (ps3left << P1_Left);
  buttonVals = buttonVals | (ps3right << P1_Right);
  buttonVals = buttonVals | (ps3up << P1_Top);
  buttonVals = buttonVals | (ps3down << P1_Bottom);

  buttonVals = buttonVals | (ps3circle << P2_Right);
  buttonVals = buttonVals | (ps3cross << P2_Bottom);
  buttonVals = buttonVals | (ps3square << P2_Left);
  buttonVals = buttonVals | (ps3triangle << P2_Top);

  return buttonVals;
}

static byte getReadPS3GamePad()
{
  if (!buttonRun)
  {
    ps3gamepad_init();
    buttonRun = true;
  }
  else
  {
    return ps3gamepad_loop();
  }

  return 0;
}

#endif

static byte getReadShift()
{
#ifdef ONEBUTTON
  return buttonCheck();
#elif defined(GAMEPAD)
  return getReadShiftAnalog();
#elif defined(PS3GAMEPAD)
  return getReadPS3GamePad();
#endif
}

static uint16_t keyStateThread;
static SemaphoreHandle_t xSemaphoreInput;
static void inputThread(void *empty)
{
  for (;;)
  {
    if (xSemaphoreTake(xSemaphoreInput, (TickType_t)100) == pdTRUE)
    {
      inputlastTime = inputcurrentTime;
      inputcurrentTime = esp_timer_get_time();
      inputframeTime = inputcurrentTime - inputlastTime;
      inputfps = 1000000 / inputframeTime;

      keyStateThread = getReadShift();
      xSemaphoreGive(xSemaphoreInput);
      vTaskDelay(10);
    }
  }
}

#endif

/* Buttons */
static uint16_t keystate;
uint8_t buttons = 0;

uint8_t Arduboy2Core::buttonsState()
{
  buttons = 0;
  loop_tca8418();
  loop_pcf8574();
  auto up = keyboard_states[1][0];
  auto down = keyboard_states[1][2];
  auto left = keyboard_states[0][1];
  auto right = keyboard_states[2][1];
  auto keyA = keyboard_states[6][5];
  auto keyB = keyboard_states[6][6];
  if (keyA)
  {
    buttons |= A_BUTTON;
  }
  if (keyB)
  {
    buttons |= B_BUTTON;
  }
  if (up || keypad_states[0])
  {
    buttons |= UP_BUTTON;
  }
  if (down || keypad_states[1])
  {
    buttons |= DOWN_BUTTON;
  }
  if (left || keypad_states[2])
  {
    buttons |= LEFT_BUTTON;
  }
  if (right || keypad_states[3])
  {
    buttons |= RIGHT_BUTTON;
  }
  return buttons;
}

// delay in ms with 16 bit duration
void Arduboy2Core::delayShort(uint16_t ms)
{
  delay((unsigned long)ms);
};
