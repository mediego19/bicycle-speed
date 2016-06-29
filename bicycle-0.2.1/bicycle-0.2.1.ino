/*  Bicycle speedometer for Arduino. Displays speed, distance travelled,
    temperature and includes a stopwatch.

    Copyright (C) <2016> <Nathaniel Ivan>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

bool watchOn = 0x0;
bool toggleOn = 0x0;

uint64_t time_start;
uint64_t mseconds = 0;
uint64_t elapsedTime = 0;
float hseconds = 0;
float seconds = 0;
uint32_t minutes = 0;

uint8_t isMetric;



// Defines the buttonInfo class for use in stuff (HAHA PLACEHOLDER)
struct buttonInfo
{
  uint8_t pin;
  bool lastState;
  bool toggle;
};

// Initializes button objects, with pin settings and initial states
struct buttonInfo unitButton = {11, 0x0, 0x0};
struct buttonInfo startButton = {10, 0x0, 0x0};
struct buttonInfo resetButton = {12, 0x0, 0x0};

struct buttonInfo checkButton(uint8_t pin, bool last) // maybe it's struct return messing up, it's not supposed to work
{
  struct buttonInfo button;
  button.pin = pin;

  // Debouncing code to stabalize input signal
  bool bounceNew = 0x0;
  bool bounceOld = last;
  const uint16_t DEBOUNCE_THRESHHOLD = 200;
  uint16_t x = 0;

  while (x < DEBOUNCE_THRESHHOLD)
  {
    bounceNew = digitalRead(pin);
    if (bounceNew == bounceOld)
    {
      x++;
    }
    else
    {
      x = 0;
    }
    bounceOld = bounceNew;
  }
  bool current = bounceNew;

  // Schmitt trigger
  if (current == 0x1)
  {
    if (last == 0x1)
    {
      button.toggle = 0x0;
    }
    else if (last == 0x0)
    {
      button.toggle = 0x1;
    }
  }

  button.lastState = current;
  return button;
}

void displayStopwatch()
{
  // rewrite so that I can pass a function two variables predeclared for
  // setCursor locations store in array?

  mseconds = millis() - time_start + elapsedTime;
  hseconds = mseconds % 1000;
  seconds = mseconds / 1000 % 60;
  minutes = mseconds / 60000;

  if (minutes < 10)
  {
    lcd.setCursor(8,1);
    lcd.print("0");
    lcd.setCursor(9,1);
  }
  else
  {
    lcd.setCursor(8,1);
  }
  lcd.print(minutes);

  if (seconds < 10)
  {
    lcd.setCursor(11,1);
    lcd.print("0");
    lcd.setCursor(12,1);
  }
  else
  {
    lcd.setCursor(11,1);
  }
  lcd.print(seconds);

  if (hseconds <= 9)
  {
    lcd.setCursor(14,1);
    lcd.print("0");
    lcd.setCursor(15,1);
  }
  else
  {
    lcd.setCursor(14,1);
  }
  lcd.print((int) hseconds);
}

void writeToLCD(float distance, float velocity, uint8_t temp, bool toggleOn, bool watchOn)
{
  uint8_t isMetric = unitSettingsToggle(toggleOn);

  String unitLabels[3][2] = {
    {"mi","km"},
    {"F","C"},
    {" mph","km/h"}
    };

  lcd.setCursor(4,0);
  lcd.print(unitLabels[0][0 + isMetric]);
  lcd.setCursor(3,1);
  lcd.print(unitLabels[1][0 + isMetric]);
  lcd.setCursor(12,0);
  lcd.print(unitLabels[2][0 + isMetric]);

  lcd.setCursor(10,1);
  lcd.print(":");
  lcd.setCursor(13,1);
  lcd.print(".");

  // Display distance travelled
  lcd.setCursor(0, 0);
  lcd.print(distance);

  // Display current speed
  lcd.setCursor(9 - isMetric, 0); // Shifts display to account for mph vs km/h
  lcd.print("0.0");

  // Display current temperature
  lcd.setCursor(0, 1);
  lcd.print(temp);

  // Display stopwatch if running
  if (watchOn == 0x1)
  {
    displayStopwatch();
  }
  // include iff for reset as well?
}

// writeOn = 0x0 for read only, 0x1 for unit toggling
uint8_t unitSettingsToggle(bool writeOn)
{
  uint8_t metAddr = 1; // address for EEPROM metric setting
  uint8_t metVal = EEPROM.read(metAddr);
  uint8_t DEFAULT_VAL = 0; // Change to 1 for imperial units

  // Only true if readVal is neither a 1 or 0, used for error
  // handling, will simply return a 0 for imperial units
  if (!((metVal == 1) || (metVal == 0)))
  {
    EEPROM.write(metAddr, DEFAULT_VAL);
    Serial.println("Changed EEPROM in error check.");
    return DEFAULT_VAL;
  }

  // Only change value and write if asked to do so
  if (writeOn)
  {
    switch (metVal)
    {
      case 0:
        metVal = 1;
        break;
      case 1:
        metVal = 0;
        break;
    }
    EEPROM.write(metAddr, metVal);
    Serial.println("Changed EEPROM in regular way.");
  }
  return metVal;
}

// Setup loop
void setup()
{
  // for debugging in the serial monitor
  Serial.begin(9600);

  // set pinMode for buttons
  pinMode(unitButton.pin, 0x0);
  pinMode(startButton.pin, 0x0);
  pinMode(resetButton.pin, 0x0);

  // initialize lcd
  lcd.begin(16,2);

  // Loads saved units from EEPROM without changing the value
  unitSettingsToggle(0x0);
}

void loop()
{
  // Returns flag for changing the units on next display update
  unitButton = checkButton(unitButton.pin, unitButton.lastState);

  // Returns flag for starting(continuing) or stopping the stopwatch
  // on next display update
  startButton = checkButton(startButton.pin, startButton.lastState);

  // Returns flag for reseting the stopwatch on next display update
  resetButton = checkButton(resetButton.pin, resetButton.lastState);


    // Toggles stopwatch between on and off depending on startButton.toggle
  if (startButton.toggle)
  {
    switch (watchOn)
    {
      case 0:
        watchOn = 0x1;
        time_start = millis();
        break;
      case 1:
        watchOn = 0x0;
        elapsedTime = mseconds;
        break;
    }
  }

  if (resetButton.toggle)
  {
    mseconds = 0;
    elapsedTime = 0;
    time_start = millis();
    lcd.setCursor(8,1);
    lcd.print("00:00.00");
  }

  // calls writeToLCD, contains placeholder values for the time being
  writeToLCD(0.0, 0.0, 76, unitButton.toggle, watchOn);
}
