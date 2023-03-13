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
  if (DEBUG) {Serial.println(F("buttons: About to initialize pins"));}
  shift.begin(4, 1, 0);
  if (DEBUG) {Serial.println(F("buttons: pins initialized"));}
  
}

void Buttons::init(Joystick_* joystick, Config* config, Outputs* outputs) {
  if (DEBUG) {Serial.println(F("buttons: initializing joystick"));}
  _joystick = joystick;
  _config = config;
  _outputs = outputs;
  if (DEBUG) {Serial.println(F("buttons: initialized joystick"));}
}

void Buttons::readInputs() {

  // read shift register values
  if(shift.update()) {
    for(int i = 0; i < shift.getDataWidth(); i++) {
      int currentButtonState = shift.state(i);
      if (DEBUG) {Serial.print(currentButtonState);}
      if (currentButtonState != lastButtonState[i]) {
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
    if (DEBUG) {Serial.println("");}
  }
}

void Buttons::sendButtonState() {
  for (int i : lastButtonState) {
    Serial.print(i);
  }
  Serial.println("");
  
}
