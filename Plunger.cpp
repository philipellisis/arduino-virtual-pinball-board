#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>




Plunger::Plunger() {
  if (DEBUG) {Serial.println("plunger: About to initialize pins");}
  int plungerMax = 634;
  int plungerMin = 10;
  plungerMid = 100;
  plungerScaleFactor = float(plungerMid) / float(plungerMax);

  pinMode(23, INPUT_PULLUP); //plunger
  if (DEBUG) {Serial.println("plunger: pins initialized");}
}

void Plunger::init(Joystick_* joystick) {
  if (DEBUG) {Serial.println("plunger: initializing joystick");}
  _joystick = joystick;
  _joystick->setZAxisRange(plungerMin, plungerMax);
  if (DEBUG) {Serial.println("plunger: initialized joystick");}
}

int Plunger::getMid() {
  return plungerMid;
}
int Plunger::getMax() {
  return plungerMax;
}
int Plunger::getMin() {
  return plungerMin;
}
void Plunger::plungerRead() {
  int sensorValue = analogRead(22);
  if (sensorValue <= plungerMid) {
    adjustedValue = sensorValue;
  } else {
    adjustedValue = (sensorValue-plungerMid) * plungerScaleFactor + plungerMid;
  }
  if (DEBUG) {Serial.print("plunger: value "); Serial.println(adjustedValue);}
  _joystick->setZAxis(adjustedValue);
}

void Plunger::sendPlungerState() {
  Serial.println(adjustedValue);
}
