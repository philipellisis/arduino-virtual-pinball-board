#include "Buttons.h"
#include <Arduino.h>
#include <Joystick.h>
#include "ShiftIn.h"
#include "Enums.h"

ShiftIn<3> shift;

// Pin 18, A0 = UP
// Pin 19, A1 = RIGHT
// Pin 20, A2 = DOWN
// Pin 21, A3 = LEFT


Buttons::Buttons() {
  // Initialize Button Pins
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: About to initialize pins\r\n"));}
  shift.begin(4, 1, 0);
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: pins initialized\r\n"));}
  
}

void Buttons::init(Joystick_* joystick, Config* config, Outputs* outputs) {
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: initializing joystick\r\n"));}
  _joystick = joystick;
  _config = config;
  _outputs = outputs;
  //if (DEBUG) {Serial.print(F("DEBUG,buttons: initialized joystick\r\n"));}
}

void Buttons::readInputs() {
  byte buttonPushed = 2;
  // read shift register values
  if(shift.update()) {
    //if (DEBUG) {Serial.print(F("DEBUG"));}
    for(int i = 0; i < shift.getDataWidth(); i++) {
      bool currentButtonState = !shift.state(i);
      //if (DEBUG) {Serial.print(currentButtonState);}
      if (currentButtonState != lastButtonState[i]) {
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
        _joystick->setButton(i, currentButtonState);
        
        
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
