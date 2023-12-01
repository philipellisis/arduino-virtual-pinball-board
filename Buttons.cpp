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
  unsigned char buttonPushed = 2;
  // read shift register values
  if(buttonReader.update()) {
    //if (DEBUG) {Serial.print(F("DEBUG"));}
    for(int i = 0; i < 24; i++) {
      bool currentButtonState = !buttonReader.state(i);
      //if (DEBUG) {Serial.print(currentButtonState);}
      if (currentButtonState != lastButtonState[i]) {
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
        if (currentButtonState == 1) {
          buttonPushed = 1;
        } else if (currentButtonState == 0 && buttonPushed == 2) {
          buttonPushed = 0;
        }
        
        if (i > 3 && i < 8 && lastButtonState[config.shiftButton] == 1) {
          buttonOffset = 20;
        } else {
          buttonOffset = 0;
        }
        if (config.buttonKeyboard[i + buttonOffset] > 0) {
          if (currentButtonState == 1) {
            if (config.buttonKeyboard[i + buttonOffset] > 251) {
              if (config.buttonKeyboard[i + buttonOffset] == 252) {
                SingleConsumer.press(MEDIA_VOLUME_MUTE);
              } else if (config.buttonKeyboard[i + buttonOffset] == 253) {
                SingleConsumer.press(MEDIA_VOLUME_DOWN);
              } else if (config.buttonKeyboard[i + buttonOffset] == 254) {
                SingleConsumer.press(MEDIA_VOLUME_UP);
              }
            } else {
              BootKeyboard.press(config.buttonKeyboard[i + buttonOffset]);
            }

          } else {
            if (config.buttonKeyboard[i + buttonOffset] > 251) {
              if (config.buttonKeyboard[i + buttonOffset] == 252) {
                SingleConsumer.release(MEDIA_VOLUME_MUTE);
              } else if (config.buttonKeyboard[i + buttonOffset] == 253) {
                SingleConsumer.release(MEDIA_VOLUME_DOWN);
              } else if (config.buttonKeyboard[i + buttonOffset] == 254) {
                SingleConsumer.release(MEDIA_VOLUME_UP);
              }
            } else {
              BootKeyboard.release(config.buttonKeyboard[i + buttonOffset]);
            }
          }
        } else if (config.buttonKeyboard[i + buttonOffset] == 0 || config.disableButtonPressWhenKeyboardEnabled == 0) {
          if (currentButtonState == 1) {
            Gamepad1.press(i + buttonOffset + 1);
          } else {
            Gamepad1.release(i + buttonOffset + 1);
          }
        }

        
        
        lastButtonState[i] = currentButtonState;

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

void Buttons::sendButtonState() {
  Serial.print(F("B,"));
  for (int i = 0; i < 23; i++) {
    Serial.print(lastButtonState[i]);
    Serial.print(F(","));
  }
  Serial.print(lastButtonState[23]);
  Serial.print(F("\r\n"));
  
}
