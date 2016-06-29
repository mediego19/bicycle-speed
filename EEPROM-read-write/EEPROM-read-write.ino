

void setup() {
  // put your setup code here, to run once:

}

void loop()
{
 
}

// writeOn = 0x0 for read only, 0x1 for unit toggling
uint8_t unitSettingsToggle(bool writeOn)
{
  uint8_t metAddr = 1; // address for EEPROM metric setting
  uint8_t metVal = EEPROM.read(metAddr);
  uint8_t DEFAULT_VAL = 0; // Change to 1 for imperial units
  
  // Only true if readVal is neither a 1 or 0, used for error
  // handling, will simply return a 0 for imperial units
  if !((metVal == 1) || (metVal == 0))
  {
    EEPROM.write(metAddr, DEFAULT_VAL);
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
  }
  return metVal;
}

