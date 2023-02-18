#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>



Plunger::Plunger() {
  //Serial.println("plunger: About to initialize pins");
  int plungerMax = 634;
  int plungerMin = 10;
  plungerMid = 100;
  plungerScaleFactor = float(plungerMid) / float(plungerMax);

  pinMode(23, INPUT_PULLUP); //plunger
  //Serial.println("plunger: pins initialized");
}

void Plunger::init(Joystick_* joystick) {
  //Serial.println("plunger: initializing joystick");
  _joystick = joystick;
  _joystick->setZAxisRange(plungerMin, plungerMax);
  //Serial.println("plunger: initialized joystick");
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
  //Serial.println("plunger: value " + String(adjustedValue));
  _joystick->setZAxis(adjustedValue);
}

void Plunger::sendPlungerState() {
  Serial.println(adjustedValue);
}
