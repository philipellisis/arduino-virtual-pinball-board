#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>




Plunger::Plunger() {
  

  pinMode(23, INPUT_PULLUP); //plunger
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: pins initialized\r\n"));}
}

void Plunger::init(Joystick_* joystick, Config* config) {
  _config = config;
  plungerScaleFactor = (float)_config->plungerMid / (float)_config->plungerMax;
  _joystick = joystick;
  _joystick->setZAxisRange(_config->plungerMin, _config->plungerMax);
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: initialized joystick\r\n"));}
}

void Plunger::plungerRead() {
  int sensorValue = analogRead(23);
  if( _config->plungerButtonPush == 1 && buttonState == 0 && sensorValue >= _config->plungerMax - 5) {
    _joystick->setButton(23, 1);
    buttonState = 1;
  } else if (_config->plungerButtonPush == 1 && buttonState == 1 && sensorValue < _config->plungerMax - 5 ) {
    _joystick->setButton(23, 0);
    buttonState = 0;
  }
  if( _config->plungerButtonPush == 2 && buttonState == 0 && sensorValue <= _config->plungerMin + 5) {
    _joystick->setButton(23, 1);
    buttonState = 1;
  } else if (_config->plungerButtonPush == 2 && buttonState == 1 && sensorValue > _config->plungerMin + 5 ) {
    _joystick->setButton(23, 0);
    buttonState = 0;
  }
  
  if (sensorValue <= _config->plungerMid) {
    adjustedValue = sensorValue;
  } else {
    adjustedValue = (sensorValue-_config->plungerMid) * plungerScaleFactor + _config->plungerMid;
  }
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: scale factor ")); Serial.print(plungerScaleFactor); Serial.print(F("DEBUG,plunger: value ")); Serial.print(adjustedValue); Serial.print("\r\n");}
  _joystick->setZAxis(adjustedValue);
}

void Plunger::sendPlungerState() {
  Serial.print(F("PLUNGER,"));
  Serial.print(analogRead(23));
  Serial.print("\r\n");
}
