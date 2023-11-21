#include "Buttons.h"
#include <Arduino.h>
#include "ButtonReader.h"
#include "Enums.h"
#include "HID-Project.h"



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

void Buttons::init(Config* config, Outputs* outputs) {
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: initializing Gamepad1\r\n"));}
  _config = config;
  _outputs = outputs;
  BootKeyboard.begin();
  SingleConsumer.begin();
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: initialized Gamepad1\r\n"));}
}

void Buttons::readInputs() {
  byte buttonPushed = 2;
  // read shift register values
  if(buttonReader.update()) {
    //if (DEBUG) {Serial.print(F("DEBUG"));}
    for(int i = 0; i < 24; i++) {
      bool currentButtonState = !buttonReader.state(i);
      //if (DEBUG) {Serial.print(currentButtonState);}
      if (currentButtonState != lastButtonState[i]) {
        _config->updateUSB = true;
        // mod 24 so that the button number is always between 0 and 23
        if (i == _config->nightModeButton % 24) {
          
          if (_config->nightModeButton < 24) {
            _config->nightMode = currentButtonState;
          } else {
            if (currentButtonState == 1) {
              //if (DEBUG) {Serial.print("DEBUG,setting night mode to ");Serial.print(_config->nightMode); Serial.print(F("\r\n"));}
              _config->nightMode = !_config->nightMode;
            }
          }
        }
        if (currentButtonState == 1) {
          buttonPushed = 1;
        } else if (currentButtonState == 0 && buttonPushed == 2) {
          buttonPushed = 0;
        }
        
        if (i > 3 && i < 8 && lastButtonState[_config->shiftButton] == 1) {
          buttonOffset = 20;
        } else {
          buttonOffset = 0;
        }
        if (_config->buttonKeyboard[i + buttonOffset] > 0) {
          if (currentButtonState == 1) {
            if (_config->buttonKeyboard[i + buttonOffset] == 127) {
              SingleConsumer.press(MEDIA_VOLUME_MUTE);
            } else if (_config->buttonKeyboard[i + buttonOffset] == 128) {
              SingleConsumer.press(MEDIA_VOLUME_DOWN);
            } else if (_config->buttonKeyboard[i + buttonOffset] == 129) {
              SingleConsumer.press(MEDIA_VOLUME_UP);
            } else {
              BootKeyboard.press(_config->buttonKeyboard[i + buttonOffset]);
            }
          } else {
            if (_config->buttonKeyboard[i + buttonOffset] == 127) {
              SingleConsumer.release(MEDIA_VOLUME_MUTE);
            } else if (_config->buttonKeyboard[i + buttonOffset] == 128) {
              SingleConsumer.release(MEDIA_VOLUME_DOWN);
            } else if (_config->buttonKeyboard[i + buttonOffset] == 129) {
              SingleConsumer.release(MEDIA_VOLUME_UP);
            } else {
              BootKeyboard.release(_config->buttonKeyboard[i + buttonOffset]);
            }
          }
        } else {
          if (currentButtonState == 1) {
            Gamepad1.press(i + buttonOffset);
          } else {
            Gamepad1.release(i + buttonOffset);
          }
        }

        
        
        lastButtonState[i] = currentButtonState;

        for (int j = 0; j < 4; j++) {
          if (currentButtonState == 1 && _config->solenoidButtonMap[j] > 0 && _config->solenoidButtonMap[j] - 1  == i) {
            if (_config->solenoidOutputMap[j] > 0) {
              _outputs->updateOutput(_config->solenoidOutputMap[j] - 1, 255);
            }
          } else if (currentButtonState == 0 && _config->solenoidButtonMap[j] > 0 && _config->solenoidButtonMap[j] - 1 == i) {
            if (_config->solenoidOutputMap[j] > 0) {
              _outputs->updateOutput(_config->solenoidOutputMap[j] - 1, 0);
            }
          }
        }
      }
    }
    //if (DEBUG) {Serial.print("\r\n");}
  }
  if (buttonPushed == 1) {
    if (_config->lightShowState == WAITING_INPUT || _config->lightShowState == IN_RANDOM_MODE_WAITING_INPUT) {
      _config->lightShowState = INPUT_RECEIVED_SET_LIGHTS_HIGH;
    }
  } else if (buttonPushed == 0) {
    if (_config->lightShowState == INPUT_RECEIVED_BUTTON_STILL_PRESSED) {
      _config->lightShowState = INPUT_RECEIVED_SET_LIGHTS_LOW;
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
