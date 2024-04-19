#include "Adafruit_Keypad.h"

const byte ROWS = 8; // rows
const byte COLS = 5; // columns
// define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
    {'1', '2', '3', '4', '5'},
    {'Q', 'W', 'E', 'R', 'T'},
    {'A', 'S', 'D', 'F', 'G'},
    {'Z', 'X', 'C', 'V', 'B'},

    {'6', '7', '8', '9', '0'},
    {'Y', 'U', 'I', 'O', 'P'},
    {'H', 'J', 'K', 'L', ';'},
    {'N', 'M', ',', '.', '/'},
};

byte rowPins[ROWS] = {1, 2, 3, 4, 5, 6, 7, 8}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13, 14};     // connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup_key()
{
  Serial.begin(115200);
  customKeypad.begin();
}

void loop_key()
{
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    Serial.print((char)e.bit.KEY);
    if (e.bit.EVENT == KEY_JUST_PRESSED)
    {
      Serial.print(" pressed");
      uint8_t row = e.bit.ROW;
      uint8_t col = e.bit.COL;
      Serial.print(" row: ");
      Serial.print(row);
      Serial.print(" col: ");
      Serial.print(col);
      Serial.println();
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED)
    {
      Serial.println(" released");
    }
  }

  delay(10);
}