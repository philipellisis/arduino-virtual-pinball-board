#include "Buttons.h"
#include <Arduino.h>
#include <Joystick.h>
#include "ShiftIn.h"

ShiftIn<1> shift;

// Pin 18, A0 = UP
// Pin 19, A1 = RIGHT
// Pin 20, A2 = DOWN
// Pin 21, A3 = LEFT


Buttons::Buttons() {
  // Initialize Button Pins
  Serial.println("buttons: About to initialize pins");
  shift.begin(4, 1, 0, 1);
  Serial.println("buttons: pins initialized");
  
}

void Buttons::init(Joystick_* joystick) {
  Serial.println("buttons: initializing joystick");
  _joystick = joystick;

  Serial.println("buttons: initialized joystick");
}

void Buttons::readInputs() {

  // read shift register values
  if(shift.update()) {
    for(int i = 0; i < shift.getDataWidth(); i++) {
      Serial.print(shift.state(i));
      int currentButtonState = shift.state(i);
      if (currentButtonState != lastButtonState[i]) {
        _joystick->setButton(i, currentButtonState);
        lastButtonState[i] = currentButtonState;
      }
    }
  }
}
