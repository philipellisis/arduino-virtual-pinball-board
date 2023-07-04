#include "Buttons.h"
#include <Arduino.h>
#include <Joystick.h>
#include "ShiftIn.h"

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

  // read shift register values
  if(shift.update()) {
    //if (DEBUG) {Serial.print(F("DEBUG"));}
    for(int i = 0; i < shift.getDataWidth(); i++) {
      bool currentButtonState = !shift.state(i);
      //if (DEBUG) {Serial.print(currentButtonState);}
      if (currentButtonState != lastButtonState[i]) {
        if (i == 21) {
          //if (DEBUG) {Serial.print("DEBUG,setting night mode to ");Serial.print(currentButtonState); Serial.print(F("\r\n"));}
          _config->nightMode = currentButtonState;
        }
        if (_config->lightShowState == 3 || _config->lightShowState == 6) {
          _config->lightShowState = 4;
        }
        _joystick->setButton(i, currentButtonState);
        
        
        lastButtonState[i] = currentButtonState;
        for (int j = 0; j < 4; j++) {
          if (currentButtonState == 1 && _config->solenoidButtonMap[j] == i) {
            _outputs->updateOutput(_config->solenoidOutputMap[j], 255);
          } else if (currentButtonState == 0 && _config->solenoidButtonMap[j] == i) {
            _outputs->updateOutput(_config->solenoidOutputMap[j], 0);
          }
        }
      }
    }
    //if (DEBUG) {Serial.print("\r\n");}
  }
}

void Buttons::sendButtonState() {
  Serial.print(F("BUTTONS,"));
  for (int i = 0; i < 23; i++) {
    Serial.print(lastButtonState[i]);
    Serial.print(F(","));
  }
  Serial.print(lastButtonState[23]);
  Serial.print(F("\r\n"));
  
}
