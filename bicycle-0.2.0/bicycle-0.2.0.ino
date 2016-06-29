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

const int startStopButtonPin = 10; // button for starting and stopping the stopwatch
const int unitButtonPin = 11; // button for toggling metric or imperial units
const int resetLapButtonPin = 12; // button for reset and lap on the stopwatch

bool watchOn = LOW;
bool unitButtonState = LOW;
bool startStopButtonState = LOW;
bool resetLapButtonState = LOW;
bool unitDepress = LOW;

bool startStopDepress = LOW;
bool resetLapDepress = LOW;
bool toggleOn = LOW;

unsigned long time_start;
unsigned long mseconds = 0;
unsigned long elapsedTime = 0;
float hseconds = 0;
float seconds = 0;
int minutes = 0;

byte isMetric;
byte address = 1; // address for EEPROM settings

void toggleUnits(bool initialize)
{
  String unitLabels[3][2] = {
    {"mi","km"},
    {"F","C"},
    {" mph","km/h"}
    };

  if (initialize == LOW)
  {
    switch (isMetric)
    {
      case 0:
        isMetric = 1;
        break;
      case 1:
        isMetric = 0;
        break;
    }
  }

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

  // Stores isMetric to EEPROM settings if value has changed
  if (EEPROM.read(address) != isMetric)
  {
    EEPROM.write(address, isMetric);
  }
  Serial.print("New EEPROM value: ");
  Serial.println(EEPROM.read(address));
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

void writeToLCD(float distance, float velocity, byte temp, bool toggleOn, bool watchOn)
{
  // Display distance travelled
  lcd.setCursor(0, 0);
  lcd.print(distance);

  // Display current speed
  lcd.setCursor(9 - isMetric, 0); // Shifts display to account for mph vs km/h
  lcd.print(velocity);

  // Display current temperature
  lcd.setCursor(0, 1);
  lcd.print(temp);

  // Display units only if changed
  if (toggleOn == HIGH)
  {
    toggleUnits(LOW);
  }

  // Display stopwatch if running
  if (watchOn == HIGH)
  {
    displayStopwatch();
  }
  // include iff for reset as well?
}

void setup()
{
  // for debugging in the serial monitor
  Serial.begin(9600);

  // Checks for metric value in EEPROM and sets isMetric, otherwise defaults
  // to imperial units.
  if ((EEPROM.read(address) == 1) | (EEPROM.read(address) == 0))
  {
    isMetric = EEPROM.read(address);
  }
  else
  {
    isMetric = 0; // Defaults the units to imperial if no value is found in
                  // memory
  }
  Serial.print("Initial value: ");
  Serial.println(EEPROM.read(address));
  // initialize lcd
  lcd.begin(16,2);
  toggleUnits(HIGH);

  // set pinMode for buttons
  pinMode(startStopButtonPin, INPUT);
  pinMode(unitButtonPin, INPUT);
  pinMode(resetLapButtonPin, INPUT);


}

void loop()
{

  // Determimes if the Metric/Imperial toggle button has been pressed and calls
  // toggleUnits() to rewrite the lcd screen.

  unitButtonState = digitalRead(unitButtonPin);
  if (unitDepress == HIGH && unitButtonState == HIGH)
  {
    toggleOn = LOW;
  }
  if (unitDepress == HIGH && unitButtonState == LOW)
  {
    unitDepress = LOW;
  }
  if (unitDepress == LOW && unitButtonState == HIGH)
  {
    unitDepress = HIGH;
    toggleOn = HIGH;
  }

  // Starts and Stops stopwatch
  startStopButtonState = digitalRead(startStopButtonPin);
  if (startStopDepress == 1 && startStopButtonState == LOW)
  {
    startStopDepress = 0;
  }
  if (startStopDepress == 0 && startStopButtonState == HIGH)
  {
    switch (watchOn)
    {
      case 0:
        watchOn = HIGH;
        time_start = millis();
        break;
      case 1:
        watchOn = LOW;
        elapsedTime = mseconds;
        break;
    }
    startStopDepress = 1;
  }

  // Resets stopwatch time
  resetLapButtonState = digitalRead(resetLapButtonPin);
  if (resetLapDepress == HIGH && resetLapButtonState == LOW)
  {
    resetLapDepress = 0;
  }
  if (resetLapDepress == LOW && resetLapButtonState == HIGH)
  {
    mseconds = 0;
    elapsedTime = 0;
    time_start = millis();
    lcd.setCursor(8,1);
    lcd.print("00:00.00");
    resetLapDepress = 1;
  }

  // calls writeToLCD, contains placeholder values for the time being
  writeToLCD(0.0, 0.0, 76, toggleOn, watchOn);
}
