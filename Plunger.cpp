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

  long sensorValue = 0;
  int newReading;


  //remove any sensor value that does not agree with the prior value
  int goodReadings = 0;
  for (int i = 0; i < 5; i++) {
    newReading = analogRead(23);
    if (newReading < truePriorValue + 10 && newReading > truePriorValue - 10) {
      goodReadings++;
      sensorValue += newReading;
    }
  }
  if (goodReadings > 0) {
    sensorValue = sensorValue / (goodReadings);
  } else {
    sensorValue = newReading;
  }
  
  


  if (_config->restingStateCounter < _config->restingStateMax || _config->disablePlungerWhenNotInUse == 0) {
    for (int i = 0; i < _config->plungerAverageRead; i++) {
      sensorValue += analogRead(23);
    }
    sensorValue = sensorValue / (_config->plungerAverageRead + 1);
  }


  // this checks that the plunger is sitting stationary. If so, it will enable the accelerometer. It also checks if there is nothing connected. to ensure the accelerometer still works even if the plunger is disconnected
  if ((sensorValue < _config->plungerMid + 50 && sensorValue > _config->plungerMid - 50) || sensorValue > 990) {
    // plunger is in resting state when counter is > 200. Is moving or almost done moving when counter is > 0 and <= 200
    if (_config->restingStateCounter < _config->restingStateMax) {
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
    if (adjustedValue > 100) {
      adjustedValue = -127;
    }
  } else {
    adjustedValue = static_cast<int8_t>((float)(sensorValue - _config->plungerMid) / (_config->plungerMax - _config->plungerMid) * 128);
    if (adjustedValue < -100) {
      adjustedValue = 127;
    }
  }


        // Serial.print("not resting: ");
        // Serial.print(adjustedValue);
        // Serial.print("\r\n");
  // if (plungerMinSendCount == 62) {
  //   plungerData[incrementor] = minValue;
  //   if (incrementor < 62) {
  //     incrementor++;
  //   } else {
  //     incrementor = 0;
  //   }
  // }
  
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: scale factor ")); Serial.print(plungerScaleFactor); Serial.print(F("DEBUG,plunger: value ")); Serial.print(adjustedValue); Serial.print("\r\n");}
  if (priorValue != sensorValue && _config->restingStateCounter < _config->restingStateMax ) {
    // Serial.print(adjustedValue);
    // Serial.print(F("\r\n"));
    if ((priorValue - sensorValue < 20 || adjustedValue < 0 || _config->enablePlungerQuickRelease == 0) && (priorValue - sensorValue > -30 || adjustedValue < 10 || _config->enablePlungerQuickRelease == 0)) {
      _config->updateUSB = true;
      Gamepad1.zAxis(adjustedValue);
      // Serial.print("plunger in motion: ");
      // Serial.print(adjustedValue);
      // Serial.print(F("\r\n"));
      priorValue = sensorValue;
    } else {
      // Serial.print("plunger in motion not sending value: ");
      // Serial.print(adjustedValue);
      // Serial.print(F("\r\n"));
    }

  } else if (priorValue != sensorValue && _config->restingStateCounter == _config->restingStateMax  ) {
    if (_config->disablePlungerWhenNotInUse == 1) {
      Gamepad1.zAxis(0);
      // Serial.print("plunger not in motion, sending 0: ");
      // Serial.print(adjustedValue);
      // Serial.print(F("\r\n"));
      if (priorValue != 0) {
        _config->updateUSB = true;
      }
      priorValue = 0;
    } else {
      // Serial.print("plunger not in motion: ");
      // Serial.print(adjustedValue);
      // Serial.print(F("\r\n"));
      Gamepad1.zAxis(adjustedValue);
      _config->updateUSB = true;
      priorValue = sensorValue;
    }
    truePriorValue = sensorValue;
    
  }
  
}

void Plunger::sendPlungerState() {
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print(F("\r\n"));
}
