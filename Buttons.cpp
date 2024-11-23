#include "Buttons.h"
#include <Arduino.h>
#include "ButtonReader.h"
#include "Enums.h"
#include "Globals.h"
#include <XInput.h>

ButtonReader buttonReader;

// Pin 18, A0 = UP
// Pin 19, A1 = RIGHT
// Pin 20, A2 = DOWN
// Pin 21, A3 = LEFT

Buttons::Buttons()
{
  // Initialize Button Pins
  // if (DEBUG) {Serial.print(F("DEBUG,buttons: About to initialize pins\r\n"));}
  buttonReader.init();
  // if (DEBUG) {Serial.print(F("DEBUG,buttons: pins initialized\r\n"));}
}

void Buttons::init()
{
  // if (DEBUG) {Serial.print(F("DEBUG,buttons: initializing Gamepad1\r\n"));}
  // if (DEBUG) {Serial.print(F("DEBUG,buttons: initialized Gamepad1\r\n"));}
}

bool Buttons::readInputs()
{
  unsigned char buttonPushed = 2; // this is set to notify light show that a button press has happened
  bool buttonChangedState = false;
  // read shift register values
  if (buttonReader.update() || numberButtonsPressed > 0)
  {
    for (unsigned char i = 0; i < 24; i++)
    {
      bool currentButtonState = !buttonReader.state(i);
      if (currentButtonState != lastButtonState[i])
      {
        buttonChangedState = true;

        if (currentButtonState == 1)
        {
          buttonPushed = 1;
        }
        else if (currentButtonState == 0 && buttonPushed == 2)
        {
          buttonPushed = 0;
        }

        lastButtonState[i] = sendButtonPush(i, currentButtonState);
      }
    }

    // Check if button state starts with 001111111
    if (lastButtonState[0] == 0 &&
        lastButtonState[1] == 0 &&
        lastButtonState[2] == 1 &&
        lastButtonState[3] == 1 &&
        lastButtonState[4] == 1 &&
        lastButtonState[5] == 1 &&
        lastButtonState[6] == 1 &&
        lastButtonState[7] == 1 &&
        lastButtonState[8] == 1)
    {
      // Issue the command
      pinMode(11, OUTPUT);
      digitalWrite(11, LOW);
    }
  }

  if (buttonPushed == 1)
  {
    if (config.lightShowState == WAITING_INPUT || config.lightShowState == IN_RANDOM_MODE_WAITING_INPUT)
    {
      config.lightShowState = INPUT_RECEIVED_SET_LIGHTS_HIGH;
    }
  }
  else if (buttonPushed == 0)
  {
    if (config.lightShowState == INPUT_RECEIVED_BUTTON_STILL_PRESSED)
    {
      config.lightShowState = INPUT_RECEIVED_SET_LIGHTS_LOW;
    }
  }
  return buttonChangedState;
}

bool Buttons::sendButtonPush(unsigned char i, bool currentButtonState)
{

  config.updateUSB = true;
  // mod 24 so that the button number is always between 0 and 23
  if (i == config.nightModeButton % 24)
  {

    if (config.nightModeButton < 24)
    {
      config.nightMode = currentButtonState;
    }
    else
    {
      if (currentButtonState == 1)
      {
        // if (DEBUG) {Serial.print("DEBUG,setting night mode to ");Serial.print(config.nightMode); Serial.print(F("\r\n"));}
        config.nightMode = !config.nightMode;
      }
    }
  }

  if (config.buttonKeyDebounce[i] > 0 && currentButtonState == 0)
  {
    if (config.buttonKeyDebounce[i] < config.buttonDebounceCounter)
    {
      config.buttonKeyDebounce[i]++;
      return 1;
    }
    else
    {
      config.buttonKeyDebounce[i] = 1;
      numberButtonsPressed--;
      // Serial.print(F("decrementing"));
      // Serial.print(numberButtonsPressed);
      // Serial.print(F("\r\n"));
    }
  }
  else if (config.buttonKeyDebounce[i] > 0 && currentButtonState == 1)
  {
    numberButtonsPressed++;
    // Serial.print(F("incrementing"));
    // Serial.print(numberButtonsPressed);
    // Serial.print(F("\r\n"));
  }

  if (i > 3 && i < 12 && currentButtonState == 0 && lastButtonState[i + 20] == 1 && lastButtonState[config.shiftButton] == 0)
  {
    sendActualButtonPress(i + 20, currentButtonState);
    lastButtonState[i + 20] = currentButtonState;
  }

  if (i > 3 && i < 12 && lastButtonState[config.shiftButton] == 1)
  {
    buttonOffset = i + 20;
    if (lastButtonState[config.shiftButton] == 1)
    {
      lastButtonState[buttonOffset] = currentButtonState;
      sendActualButtonPress(buttonOffset, currentButtonState);
    }
    if (currentButtonState == 0)
    {
      sendActualButtonPress(i, currentButtonState);
      sendActualButtonPress(buttonOffset, currentButtonState);
    }
  }
  else
  {
    buttonOffset = i;
    sendActualButtonPress(buttonOffset, currentButtonState);
  }

  // trigger solenoid if needed
  for (int j = 0; j < 4; j++)
  {
    if (currentButtonState == 1 && config.solenoidButtonMap[j] > 0 && config.solenoidButtonMap[j] - 1 == i)
    {
      if (config.solenoidOutputMap[j] > 0)
      {
        outputs.updateOutput(config.solenoidOutputMap[j] - 1, 255);
      }
    }
    else if (currentButtonState == 0 && config.solenoidButtonMap[j] > 0 && config.solenoidButtonMap[j] - 1 == i)
    {
      if (config.solenoidOutputMap[j] > 0)
      {
        outputs.updateOutput(config.solenoidOutputMap[j] - 1, 0);
      }
    }
  }
  return currentButtonState;
}

void Buttons::sendActualButtonPress(unsigned char buttonOffset, bool currentButtonState)
{

  if (currentButtonState == 1)
  {
    XInput.press(buttonMapper(buttonOffset));
  }
  else
  {
    XInput.release(buttonMapper(buttonOffset));
  }
}

unsigned char Buttons::buttonMapper(unsigned char buttonNumber)
{
  //	BUTTON_LOGO = 0,
  //  BUTTON_A = 1,
  //  BUTTON_B = 2,
  //  BUTTON_X = 3,
  //  BUTTON_Y = 4,
  //  BUTTON_LB = 5,
  //  BUTTON_RB = 6,
  //  BUTTON_BACK = 7,
  //  BUTTON_START = 8,
  //  BUTTON_L3 = 9,
  //  BUTTON_R3 = 10,
  //  DPAD_UP = 11,
  //  DPAD_DOWN = 12,
  //  DPAD_LEFT = 13,
  //  DPAD_RIGHT = 14,
  //  TRIGGER_LEFT = 15,
  //  TRIGGER_RIGHT = 16,
  //  JOY_LEFT,
  //  JOY_RIGHT,
  switch (buttonNumber)
  {
  case 0:
    return 6;
  case 1:
    return 16;
  case 2:
    return 5;
  case 3:
    return 15;
  case 4:
    return 4;
  case 5:
    return 1;
  case 6:
    return 3;
  case 7:
    return 2;
  case 8:
    return 8;
  case 9:
    return 9;
  case 10:
    return 10;
  case 11:
    return 0;
  case 12:
    return 13;
  case 13:
    return 14;
  case 14:
    return 15;
  case 15:
    return 16;
  case 16:
    return 17;
  case 17:
    return 18;
  case 18:
    return 19;
  case 19:
    return 20;
  case 20:
    return 21;
  case 21:
    return 22;
  case 22:
    return 23;
  case 23:
    return 24;
  case 24:
    return 11;
  case 25:
    return 12;
  case 26:
    return 13;
  case 27:
    return 14;
  case 28:
    return 7;
  case 29:
    return 24;
  case 30:
    return 24;
  case 31:
    return 24;
  case 32:
    return 24;
  default:
    return 0;
  }
}

void Buttons::sendButtonState()
{
  Serial.print(F("B,"));
  for (int i = 0; i < 31; i++)
  {
    Serial.print(lastButtonState[i]);
    Serial.print(F(","));
  }
  Serial.print(lastButtonState[23]);
  Serial.print(F("\r\n"));
}