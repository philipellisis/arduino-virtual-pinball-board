#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>




Plunger::Plunger() {
  

  pinMode(23, INPUT_PULLUP); //plunger
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: pins initialized\r\n"));}
}

void Plunger::init(Joystick_* joystick, Config* config) {
  _config = config;
  _joystick = joystick;
  resetPlunger();
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: initialized joystick\r\n"));}
}

void Plunger::resetPlunger() {
  plungerScaleFactor = (float)_config->plungerMid / (float)_config->plungerMax;
  _joystick->setZAxisRange(_config->plungerMin, (_config->plungerMax - _config->plungerMid) * plungerScaleFactor + _config->plungerMid);
}

void Plunger::plungerRead() {
  unsigned long sensorValue = analogRead(23);
  for (int i = 0; i < _config->plungerAverageRead; i++) {
    sensorValue += analogRead(23);
  }
  // Serial.print("sensorraw: ");
  // Serial.print(sensorValue);
  // Serial.print("\r\n");
  sensorValue = sensorValue / (_config->plungerAverageRead + 1);
  // Serial.print("sensornorm: ");
  // Serial.print(sensorValue);
  // Serial.print("\r\n");

  if( (_config->plungerButtonPush == 1 || _config->plungerButtonPush == 3) && buttonState == 0 && sensorValue >= _config->plungerMax - 15) {
    _joystick->setButton(_config->plungerLaunchButton, 1);
    buttonState = 1;
  } else if ((_config->plungerButtonPush == 1 || _config->plungerButtonPush == 3)  && buttonState == 1 && sensorValue < _config->plungerMax - 15 ) {
    _joystick->setButton(_config->plungerLaunchButton, 0);
    buttonState = 0;
  }
  if( _config->plungerButtonPush >= 2 && buttonState2 == 0 && sensorValue <= _config->plungerMin + 10) {
    _joystick->setButton(_config->plungerLaunchButton, 1);
    buttonState2 = 1;
  } else if (_config->plungerButtonPush >= 2 && buttonState2 == 1 && sensorValue > _config->plungerMin + 10 ) {
    _joystick->setButton(_config->plungerLaunchButton, 0);
    buttonState2 = 0;
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
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print("\r\n");
}
