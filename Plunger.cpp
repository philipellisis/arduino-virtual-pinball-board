#include "Plunger.h"
#include <Arduino.h>
#include "HID-Project.h"



Plunger::Plunger() {
  

  pinMode(23, INPUT_PULLUP); //plunger
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: pins initialized\r\n"));}
}

void Plunger::init(Config* config) {
  _config = config;
  resetPlunger();
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: initialized Gamepad1\r\n"));}
}

void Plunger::resetPlunger() {
  plungerScaleFactor = (float)(_config->plungerMid - _config->plungerMin) / (float)(_config->plungerMax - _config->plungerMid);
}

void Plunger::plungerRead() {

  long sensorValue = analogRead(23);
  
  //remove any sensor value that does not agree with the others


  if (_config->restingStateCounter < 200 && _config->disablePlungerWhenNotInUse == 0) {
    for (int i = 0; i < _config->plungerAverageRead; i++) {
      sensorValue += analogRead(23);
    }
    // Serial.print("sensorraw: ");
    // Serial.print(sensorValue);
    // Serial.print("\r\n");
    sensorValue = sensorValue / (_config->plungerAverageRead + 1);
  }


  // this checks that the plunger is sitting stationary. If so, it will enable the accelerometer. It also checks if there is nothing connected. to ensure the accelerometer still works even if the plunger is disconnected
  if ((sensorValue < _config->plungerMid + 50 && sensorValue > _config->plungerMid - 50) || sensorValue > 990) {
    // plunger is in resting state when counter is > 200. Is moving or almost done moving when counter is > 0 and <= 200
    if (_config->restingStateCounter < 200) {
      _config->restingStateCounter++;
    }
  } else {
      //plunger is actively moving when counter is = 0
      _config->restingStateCounter = 0;
  }
  // Serial.print("sensornorm: ");
  // Serial.print(sensorValue);
  // Serial.print("\r\n");

  if( (_config->plungerButtonPush == 1 || _config->plungerButtonPush == 3) && buttonState == 0 && sensorValue >= _config->plungerMax - 20) {
    Gamepad1.press(_config->plungerLaunchButton);
    buttonState = 1;
  } else if ((_config->plungerButtonPush == 1 || _config->plungerButtonPush == 3)  && buttonState == 1 && sensorValue < _config->plungerMax - 20 ) {
    Gamepad1.release(_config->plungerLaunchButton);
    buttonState = 0;
  }
  if( _config->plungerButtonPush >= 2 && buttonState2 == 0 && sensorValue <= _config->plungerMin + 10) {
    Gamepad1.press(_config->plungerLaunchButton);
    buttonState2 = 1;
  } else if (_config->plungerButtonPush >= 2 && buttonState2 == 1 && sensorValue > _config->plungerMin + 10 ) {
    Gamepad1.release(_config->plungerLaunchButton);
    buttonState2 = 0;
  }

  if (sensorValue <= _config->plungerMid) {
    adjustedValue = static_cast<int8_t>((1 - (float)(sensorValue - _config->plungerMin) / (_config->plungerMid - _config->plungerMin)) * -128);
  } else {
    adjustedValue = static_cast<int8_t>((float)(sensorValue - _config->plungerMid) / (_config->plungerMax - _config->plungerMid) * 128);
    if (adjustedValue < 0) {
      adjustedValue = 128;
    }
  }
  // if (plungerMinSendCount == 62) {
  //   plungerData[incrementor] = minValue;
  //   if (incrementor < 62) {
  //     incrementor++;
  //   } else {
  //     incrementor = 0;
  //   }
  // }
  
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: scale factor ")); Serial.print(plungerScaleFactor); Serial.print(F("DEBUG,plunger: value ")); Serial.print(adjustedValue); Serial.print("\r\n");}
  if (priorValue != sensorValue && _config->restingStateCounter < 200 ) {
    // Serial.print(adjustedValue);
    // Serial.print(F("\r\n"));
    if (priorValue - sensorValue < 10 || adjustedValue < 0 || _config->enablePlungerQuickRelease == 0) {
      _config->updateUSB = true;
      Gamepad1.zAxis(adjustedValue);
    }

  } else if (priorValue != sensorValue && _config->restingStateCounter == 200  ) {
    if (_config->disablePlungerWhenNotInUse == 1) {
      Gamepad1.zAxis(0);
    } else {
      Gamepad1.zAxis(adjustedValue);
    }
    _config->updateUSB = true;
  }
  priorValue = sensorValue;
}

void Plunger::sendPlungerState() {
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print(F("\r\n"));
}
