/*
  74HC595 + Serial Control
  L = Left -> Right
  R = Right -> Left
  B = Balance Ball
  X = Clear
*/

int latchPin = 5;
int clockPin = 6;
int dataPin = 4;

byte leds = 0;

unsigned long previousMillis = 0;
const unsigned long interval = 500; // ms

enum Mode {
  IDLE,
  LEFT_RIGHT,
  RIGHT_LEFT,
  BALANCE
};

Mode currentMode = IDLE;

int position = 0;
int balanceStep = 0;

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  Serial.begin(9600);

  updateShiftRegister();

  Serial.println("L = Left -> Right");
  Serial.println("R = Right -> Left");
  Serial.println("B = Balance Ball");
  Serial.println("X = Clear");
}

void loop()
{
  readSerial();
  runAnimation();
}

void readSerial()
{
  while (Serial.available())
  {
    char ch = toupper(Serial.read());

    if (ch >= '0' && ch <= '7')
    {
      currentMode = IDLE;

      leds = 0;
      bitSet(leds, ch - '0');

      updateShiftRegister();

      Serial.print("LED ");
      Serial.println(ch - '0');
    }

    else if (ch == 'L')
    {
      currentMode = LEFT_RIGHT;
      position = 0;

      leds = 0;
      updateShiftRegister();

      Serial.println("L");
    }

    else if (ch == 'R')
    {
      currentMode = RIGHT_LEFT;
      position = 7;

      leds = 0;
      updateShiftRegister();

      Serial.println("R");
    }

    else if (ch == 'B')
    {
      currentMode = BALANCE;
      balanceStep = 0;

      Serial.println("B");
    }

    else if (ch == 'X')
    {
      currentMode = IDLE;
      leds = 0;

      updateShiftRegister();

      Serial.println("CLEAR");
    }
  }
}

void runAnimation()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < interval)
    return;

  previousMillis = currentMillis;

  switch (currentMode)
  {
    case LEFT_RIGHT:

      bitSet(leds, position);
      updateShiftRegister();

      position++;

      if (position > 7)
      {
        currentMode = IDLE;
      }

      break;

    case RIGHT_LEFT:

      bitSet(leds, position);
      updateShiftRegister();

      position--;

      if (position < 0)
      {
        currentMode = IDLE;
      }

      break;

    case BALANCE:

      switch (balanceStep)
      {
        case 0: leds = B00011000; break;
        case 1: leds = B00100100; break;
        case 2: leds = B01000010; break;
        case 3: leds = B10000001; break;
        case 4: leds = B01000010; break;
        case 5: leds = B00100100; break;
        case 6: leds = B00011000; break;
      }

      updateShiftRegister();

      balanceStep++;

      if (balanceStep > 6)
      {
        currentMode = IDLE;
      }

      break;

    case IDLE:
      break;
  }
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}