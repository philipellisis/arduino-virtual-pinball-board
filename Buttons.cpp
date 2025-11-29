#include "Buttons.h"
#include <Arduino.h>
#include "ButtonReader.h"
#include "Enums.h"
#include "MinimalHID.h"
#include "Globals.h"
#include "IRTransmit.h"

ButtonReader buttonReader;
IRTransmit irTransmit;

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
  Gamepad1.begin();
  BootKeyboard.begin();
  SingleConsumer.begin();
}

bool Buttons::checkChanged()
{
  if (config.flags.buttonPressed)
  {
    return true;
  }
  if (buttonReader.update())
  {
    config.flags.buttonPressed = true;
    config.flags.updateUSB = true;
    return true;
  }
  else
  {
    return false;
  }
}

void Buttons::readInputs()
{
  uint8_t buttonPushed = 2; // this is set to notify light show that a button press has happened
  // read shift register values
  if (config.flags.buttonPressed || buttons.numberButtonsPressed > 0)
  {
    for (uint8_t i = 0; i < 24; i++)
    {
      bool currentButtonState = !buttonReader.state(i);
      if (currentButtonState != config.lastButtonState(i))
      {

        if (currentButtonState == 1)
        {
          buttonPushed = 1;
        }
        else if (currentButtonState == 0 && buttonPushed == 2)
        {
          buttonPushed = 0;
        }

        config.setLastButtonState(i, sendButtonPush(i, currentButtonState));
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

  config.flags.updateUSB = true;


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

  // Handle shifted buttons (optimize common conditions)
  if (config.shiftButton < 24 && i > 3 && i < 12) {
    bool shiftPressed = config.lastButtonState(config.shiftButton) == 1;
    
    if (!currentButtonState && config.lastButtonState(i + 20) == 1 && !shiftPressed) {
      // Logic for turning the shifted button off if the shift button is released
      sendActualButtonPress(i + 20, currentButtonState);
      config.setLastButtonState(i + 20, currentButtonState);
    }

    if (shiftPressed) {
      // Logic for turning the shifted button on if the shift button is depressed
      buttonOffset = i + 20;
      config.setLastButtonState(buttonOffset, currentButtonState);
      sendActualButtonPress(buttonOffset, currentButtonState);
      
      if (!currentButtonState) {
        sendActualButtonPress(i, currentButtonState);
        sendActualButtonPress(buttonOffset, currentButtonState);
      }
    } else {
      buttonOffset = i;
      sendActualButtonPress(buttonOffset, currentButtonState);
    }
  } else {
    buttonOffset = i;
    sendActualButtonPress(buttonOffset, currentButtonState);
  }

  // trigger solenoid or IR command if needed
  for (uint8_t j = 0; j < 4; j++)
  {
    if (config.solenoidButtonMap[j] > 0 && config.solenoidButtonMap[j] - 1 == i)
    {
      if (config.solenoidOutputMap[j] > 0)
      {
        outputs.updateOutput(config.solenoidOutputMap[j] - 1, currentButtonState ? 255 : 0);
      }
    }
  }

  // Trigger IR command on button press (rising edge only)
  if (currentButtonState && config.irOutputPin < 15 && config.irButton == i)
  {
    irTransmit.sendCommand(outputs.outputList[config.irOutputPin]);
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
      config.flags.nightMode = currentButtonState;
    }
    else
    {
      if (currentButtonState == 1)
      {
        config.flags.nightMode = !config.flags.nightMode;
      }
    }
    // Don't process this button as a joystick/keyboard press
    return;
  }

  if (config.buttonKeyboard[config.buttonRemap[buttonOffset] - 1] > 0)
  {
    processKeyboardAction(config.buttonKeyboard[config.buttonRemap[buttonOffset] - 1], currentButtonState == 1);
  }
  if (config.buttonKeyboard[config.buttonRemap[buttonOffset] - 1] == 0 || config.flags.disableButtonPressWhenKeyboardEnabled == 0)
  {
    if (currentButtonState == 1)
    {
      Gamepad1.press(config.buttonRemap[buttonOffset]);
    }
    else
    {
      Gamepad1.release(config.buttonRemap[buttonOffset]);
    }
  }
  
  // Update processed button state array for SPI/Bluetooth controller
  // This represents the final button state after all shift logic processing
  config.setProcessedButtonState(config.buttonRemap[buttonOffset] - 1, currentButtonState);
}

void Buttons::sendButtonState()
{
  Serial.print(F("B,"));
  for (uint8_t i = 0; i < 32; i++)
  {
    Serial.print((uint8_t)config.lastButtonState(i));
    if (i < 31) Serial.print(F(","));
  }
  Serial.print(F("\r\n"));
}
void Buttons::processKeyboardAction(unsigned char keyCode, bool pressed) {
  if (keyCode > 251) {
    handleMediaKey(keyCode, pressed);
  } else {
    if (pressed) {
      BootKeyboard.press(KeyboardKeycode(keyCode));
    } else {
      BootKeyboard.release(KeyboardKeycode(keyCode));
    }
  }
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
