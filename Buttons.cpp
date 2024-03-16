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


Buttons::Buttons() {
  // Initialize Button Pins
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: About to initialize pins\r\n"));}
  buttonReader.init();
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: pins initialized\r\n"));}
  
}

void Buttons::init() {
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: initializing Gamepad1\r\n"));}
  BootKeyboard.begin();
  SingleConsumer.begin();
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: initialized Gamepad1\r\n"));}
}

void Buttons::readInputs() {
  unsigned char buttonPushed = 2; //this is set to notify light show that a button press has happened
  // read shift register values
  if(buttonReader.update() || numberButtonsPressed > 0) {
    //if (DEBUG) {Serial.print(F("DEBUG"));}
    for(unsigned char i = 0; i < 24; i++) {
      bool currentButtonState = !buttonReader.state(i);
      //if (DEBUG) {Serial.print(currentButtonState);}
      if (currentButtonState != lastButtonState[i]) {
        

        if (currentButtonState == 1) {
          buttonPushed = 1;
        } else if (currentButtonState == 0 && buttonPushed == 2) {
          buttonPushed = 0;
        }
        
        lastButtonState[i] = sendButtonPush(i, currentButtonState);


      }
    }
    //if (DEBUG) {Serial.print("\r\n");}
  }
  if (buttonPushed == 1) {
    if (config.lightShowState == WAITING_INPUT || config.lightShowState == IN_RANDOM_MODE_WAITING_INPUT) {
      config.lightShowState = INPUT_RECEIVED_SET_LIGHTS_HIGH;
    }
  } else if (buttonPushed == 0) {
    if (config.lightShowState == INPUT_RECEIVED_BUTTON_STILL_PRESSED) {
      config.lightShowState = INPUT_RECEIVED_SET_LIGHTS_LOW;
    }
  }
}

bool Buttons::sendButtonPush(unsigned char i, bool currentButtonState) {

  config.updateUSB = true;
  // mod 24 so that the button number is always between 0 and 23
  if (i == config.nightModeButton % 24) {
    
    if (config.nightModeButton < 24) {
      config.nightMode = currentButtonState;
    } else {
      if (currentButtonState == 1) {
        //if (DEBUG) {Serial.print("DEBUG,setting night mode to ");Serial.print(config.nightMode); Serial.print(F("\r\n"));}
        config.nightMode = !config.nightMode;
      }
    }
  }

  if (config.buttonKeyDebounce[i] > 0 && currentButtonState == 0) {
    if (config.buttonKeyDebounce[i] < config.buttonDebounceCounter) {
      config.buttonKeyDebounce[i]++;
      return 1;
    } else {
      config.buttonKeyDebounce[i] = 1;
      numberButtonsPressed--;
      // Serial.print(F("decrementing"));
      // Serial.print(numberButtonsPressed);
      // Serial.print(F("\r\n"));
    }
  } else if (config.buttonKeyDebounce[i] > 0 && currentButtonState == 1) {
    numberButtonsPressed++;
    // Serial.print(F("incrementing"));
    // Serial.print(numberButtonsPressed);
    // Serial.print(F("\r\n"));
  }

  if (i > 3 && i < 12 && currentButtonState == 0 && lastButtonState[i + 20] == 1 && lastButtonState[config.shiftButton] == 0) {
    sendActualButtonPress(i + 20, currentButtonState);
    lastButtonState[i + 20] = currentButtonState;
  }

  if (i > 3 && i < 12 && lastButtonState[config.shiftButton] == 1) {
    buttonOffset = i + 20;
    if (lastButtonState[config.shiftButton] == 1) {
      lastButtonState[buttonOffset] = currentButtonState;
      sendActualButtonPress(buttonOffset, currentButtonState);
    }
    if (currentButtonState == 0) {
      sendActualButtonPress(i, currentButtonState);
      sendActualButtonPress(buttonOffset, currentButtonState);
    }
    
  } else {
    buttonOffset = i;
    sendActualButtonPress(buttonOffset, currentButtonState);
  }
  


  // trigger solenoid if needed
  for (int j = 0; j < 4; j++) {
    if (currentButtonState == 1 && config.solenoidButtonMap[j] > 0 && config.solenoidButtonMap[j] - 1  == i) {
      if (config.solenoidOutputMap[j] > 0) {
        outputs.updateOutput(config.solenoidOutputMap[j] - 1, 255);
      }
    } else if (currentButtonState == 0 && config.solenoidButtonMap[j] > 0 && config.solenoidButtonMap[j] - 1 == i) {
      if (config.solenoidOutputMap[j] > 0) {
        outputs.updateOutput(config.solenoidOutputMap[j] - 1, 0);
      }
    }
  }
  return currentButtonState;
}

void Buttons::sendActualButtonPress(unsigned char buttonOffset, bool currentButtonState) {
  
  if (config.buttonKeyboard[buttonOffset] > 0) {
    if (currentButtonState == 1) {
      if (config.buttonKeyboard[buttonOffset] > 251) {
        if (config.buttonKeyboard[buttonOffset] == 252) {
          SingleConsumer.press(MEDIA_VOLUME_MUTE);
        } else if (config.buttonKeyboard[buttonOffset] == 253) {
          SingleConsumer.press(MEDIA_VOLUME_DOWN);
        } else if (config.buttonKeyboard[buttonOffset] == 254) {
          SingleConsumer.press(MEDIA_VOLUME_UP);
        }
      } else {
        BootKeyboard.press(KeyboardKeycode(config.buttonKeyboard[buttonOffset]));
      }

    } else {
      if (config.buttonKeyboard[buttonOffset] > 251) {
        if (config.buttonKeyboard[buttonOffset] == 252) {
          SingleConsumer.release(MEDIA_VOLUME_MUTE);
        } else if (config.buttonKeyboard[buttonOffset] == 253) {
          SingleConsumer.release(MEDIA_VOLUME_DOWN);
        } else if (config.buttonKeyboard[buttonOffset] == 254) {
          SingleConsumer.release(MEDIA_VOLUME_UP);
        }
      } else {
        BootKeyboard.release(KeyboardKeycode(config.buttonKeyboard[buttonOffset]));
      }
    }
  }
  if (config.buttonKeyboard[buttonOffset] == 0 || config.disableButtonPressWhenKeyboardEnabled == 0) {
    if (currentButtonState == 1) {
      Gamepad1.press(buttonOffset + 1);
    } else {
      Gamepad1.release(buttonOffset + 1);
    }
  }
}

void Buttons::sendButtonState() {
  Serial.print(F("B,"));
  for (int i = 0; i < 31; i++) {
    Serial.print(lastButtonState[i]);
    Serial.print(F(","));
  }
  Serial.print(lastButtonState[23]);
  Serial.print(F("\r\n"));
  
}