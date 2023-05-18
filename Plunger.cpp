#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>




Plunger::Plunger() {
  

  pinMode(23, INPUT_PULLUP); //plunger
  if (DEBUG) {Serial.print(F("DEBUG,plunger: pins initialized\r\n"));}
}

void Plunger::init(Joystick_* joystick, Config* config) {
  _config = config;
  plungerScaleFactor = (float)_config->plungerMid / (float)_config->plungerMax;
  _joystick = joystick;
  _joystick->setZAxisRange(_config->plungerMin, _config->plungerMax);
  if (DEBUG) {Serial.print(F("DEBUG,plunger: initialized joystick\r\n"));}
}

void Plunger::plungerRead() {
  int sensorValue = analogRead(23);
  if (sensorValue <= _config->plungerMid) {
    adjustedValue = sensorValue;
  } else {
    adjustedValue = (sensorValue-_config->plungerMid) * plungerScaleFactor + _config->plungerMid;
  }
  if (DEBUG) {Serial.print(F("DEBUG,plunger: scale factor ")); Serial.print(plungerScaleFactor); Serial.print(F("DEBUG,plunger: value ")); Serial.print(adjustedValue); Serial.print("\r\n");}
  _joystick->setZAxis(adjustedValue);
}

void Plunger::sendPlungerState() {
  Serial.print(F("PLUNGER,"));
  Serial.print(analogRead(23));
  Serial.print("\r\n");
}
