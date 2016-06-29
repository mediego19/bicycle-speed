// Button debounce and polling test functions for bicycle

struct buttonInfo
{
  uint8_t pin;
  bool lastState;
  bool toggle;
};

struct buttonInfo unitButton = {12, 0x0, 0x0}; // initializing button object for the main loop
struct buttonInfo startButton = {11, 0x0, 0x0};
struct buttonInfo resetButton = {10, 0x0, 0x0};

uint16_t unitTotal = 0; // initializing total for main loop
uint16_t startTotal = 0;
uint16_t resetTotal = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(unitButton.pin, 0x0);
  pinMode(startButton.pin, 0x0);
  pinMode(resetButton.pin, 0x0);  // also starts incrementing infinitely when this is uncommented
}


struct buttonInfo checkButton(uint8_t pin, bool last) // maybe it's struct return messing up, it's not supposed to work
{
  struct buttonInfo button;
  button.pin = pin;
 
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

uint16_t increment(bool toggle, uint16_t x)
{
  if (toggle == 0x1)
  {
    x += 1;
  }
  return x;
}

void loop()
{
  unitButton = checkButton(unitButton.pin, unitButton.lastState);
  unitTotal = increment(unitButton.toggle, unitTotal);  // it works if one is commented out, but will increment infinitley
                                                        // if both are allowed, OR if there's a delay, even delay(0);                                                                   

  startButton = checkButton(startButton.pin, startButton.lastState);
  startTotal = increment(startButton.toggle, startTotal);

  resetButton = checkButton(resetButton.pin, resetButton.lastState);
  resetTotal = increment(resetButton.toggle, resetTotal);

  Serial.print(unitTotal);
  Serial.print("      ");
  Serial.print(startTotal);
  Serial.print("      ");
  Serial.println(resetTotal);
}
