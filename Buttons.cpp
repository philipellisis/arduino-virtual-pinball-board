#include "Buttons.h"
#include <Arduino.h>
#include "ButtonReader.h"
#include "Enums.h"
#include "HID-Project.h"
#include "Globals.h"

ButtonReader buttonReader;

// Pin 18, A0 = UP
// Pin 19, A1 = RIGHT
// Pin 20, A2 = DOWN
// Pin 21, A3 = LEFT

Buttons::Buttons()
{
  // Initialize Button Pins
  buttonReader.init();
}

void Buttons::init()
{
  BootKeyboard.begin();
  SingleConsumer.begin();
}

bool Buttons::checkChanged()
{
  if (config.buttonPressed)
  {
    return true;
  }
  if (buttonReader.update())
  {
    config.buttonPressed = true;
    config.updateUSB = true;
    return true;
  }
  else
  {
    return false;
  }
}

void Buttons::readInputs()
{
  unsigned char buttonPushed = 2; // this is set to notify light show that a button press has happened
  // read shift register values
  if (config.buttonPressed || buttons.numberButtonsPressed > 0)
  {
    for (unsigned char i = 0; i < 24; i++)
    {
      bool currentButtonState = !buttonReader.state(i);
      if (currentButtonState != config.lastButtonState[i])
      {

        if (currentButtonState == 1)
        {
          buttonPushed = 1;
        }
        else if (currentButtonState == 0 && buttonPushed == 2)
        {
          buttonPushed = 0;
        }

        config.lastButtonState[i] = sendButtonPush(i, currentButtonState);
      }

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
}

bool Buttons::sendButtonPush(unsigned char i, bool currentButtonState)
{

  config.updateUSB = true;


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
    }
  }
  else if (config.buttonKeyDebounce[i] > 0 && currentButtonState == 1)
  {
    numberButtonsPressed++;
  }

  //logic for turning the shifted button off if the shift button is released
  if (config.shiftButton < 24 && i > 3 && i < 12 && currentButtonState == 0 && config.lastButtonState[i + 20] == 1 && config.lastButtonState[config.shiftButton] == 0)
  {
    sendActualButtonPress(i + 20, currentButtonState);
    config.lastButtonState[i + 20] = currentButtonState;
  }

  // logic for turning the shifted button on if the shift button is depressed
  if (config.shiftButton < 24 &&  i > 3 && i < 12 && config.lastButtonState[config.shiftButton] == 1)
  {
    buttonOffset = i + 20;
    if (config.lastButtonState[config.shiftButton] == 1)
    {
      config.lastButtonState[buttonOffset] = currentButtonState;
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
    if (config.solenoidButtonMap[j] > 0 && config.solenoidButtonMap[j] - 1 == i && config.solenoidOutputMap[j] > 0)
    {
      outputs.updateOutput(config.solenoidOutputMap[j] - 1, currentButtonState ? 255 : 0);
    }
  }
  return currentButtonState;
}

void Buttons::sendActualButtonPress(unsigned char buttonOffset, bool currentButtonState)
{
  // night mode
  // mod 32 so that the button number is always between 0 and 31
  if (config.nightModeButton < 64 && buttonOffset == config.nightModeButton % 32)
  {

    if (config.nightModeButton < 32)
    {
      config.nightMode = currentButtonState;
    }
    else
    {
      if (currentButtonState == 1)
      {
        config.nightMode = !config.nightMode;
      }
    }
  }

  if (config.buttonKeyboard[buttonOffset] > 0)
  {
    if (currentButtonState == 1)
    {
      if (config.buttonKeyboard[buttonOffset] > 251)
      {
        handleMediaKey(config.buttonKeyboard[buttonOffset], true);
      }
      else
      {
        BootKeyboard.press(KeyboardKeycode(config.buttonKeyboard[buttonOffset]));
      }
    }
    else
    {
      if (config.buttonKeyboard[buttonOffset] > 251)
      {
        handleMediaKey(config.buttonKeyboard[buttonOffset], false);
      }
      else
      {
        BootKeyboard.release(KeyboardKeycode(config.buttonKeyboard[buttonOffset]));
      }
    }
  }
  if (config.buttonKeyboard[buttonOffset] == 0 || config.disableButtonPressWhenKeyboardEnabled == 0)
  {
    if (currentButtonState == 1)
    {
      Gamepad1.press(buttonOffset + 1);
    }
    else
    {
      Gamepad1.release(buttonOffset + 1);
    }
  }
}

void Buttons::sendButtonState()
{
  Serial.print(F("B,"));
  for (int i = 0; i < 31; i++)
  {
    Serial.print(config.lastButtonState[i]);
    Serial.print(F(","));
  }
  Serial.print(config.lastButtonState[31]);
  Serial.print(F("\r\n"));
}
void Buttons::handleMediaKey(unsigned char keyCode, bool pressed) {
  if (keyCode == 252) {
    if (pressed) SingleConsumer.press(MEDIA_VOLUME_MUTE);
    else SingleConsumer.release(MEDIA_VOLUME_MUTE);
  } else if (keyCode == 253) {
    if (pressed) SingleConsumer.press(MEDIA_VOLUME_DOWN);
    else SingleConsumer.release(MEDIA_VOLUME_DOWN);
  } else if (keyCode == 254) {
    if (pressed) SingleConsumer.press(MEDIA_VOLUME_UP);
    else SingleConsumer.release(MEDIA_VOLUME_UP);
  }
}
