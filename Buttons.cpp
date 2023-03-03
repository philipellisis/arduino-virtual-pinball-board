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
  if (DEBUG) {Serial.println("buttons: About to initialize pins");}
  shift.begin(4, 1, 0, 1);
  if (DEBUG) {Serial.println("buttons: pins initialized");}
  
}

void Buttons::init(Joystick_* joystick) {
  if (DEBUG) {Serial.println("buttons: initializing joystick");}
  _joystick = joystick;

  if (DEBUG) {Serial.println("buttons: initialized joystick");}
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
