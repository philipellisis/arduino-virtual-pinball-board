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
  localMax = static_cast<float>((_config->plungerMax - _config->plungerMid) * plungerScaleFactor + _config->plungerMid);
  localMin = static_cast<float>(_config->plungerMin);
}

void Plunger::plungerRead() {
  unsigned long sensorValue = analogRead(23);
  int currentValue = 0;
  int minValue = 1024;

  if (_config->restingStateCounter < 200) {
    for (int i = 0; i < _config->plungerAverageRead; i++) {
      currentValue = analogRead(23);
      if (currentValue < minValue) {
        minValue = currentValue;
      }
      sensorValue += currentValue;
    }
    if (minValue < globalMinValue) {
      globalMinValue = minValue;
    }
    // Serial.print("global min: ");
    // Serial.print(globalMinValue);
    // Serial.print("\r\n");
    // Serial.print("local min: ");
    // Serial.print(minValue);
    // Serial.print("\r\n");
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

  if( (_config->plungerButtonPush == 1 || _config->plungerButtonPush == 3) && buttonState == 0 && sensorValue >= _config->plungerMax - 15) {
    Gamepad1.press(_config->plungerLaunchButton);
    buttonState = 1;
  } else if ((_config->plungerButtonPush == 1 || _config->plungerButtonPush == 3)  && buttonState == 1 && sensorValue < _config->plungerMax - 15 ) {
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

  if (minValue <= _config->plungerMid - 40) {
    plungerMinSendCount = 0;
  }
  
  if (plungerMinSendCount < 50) {
      plungerMinSendCount++;
      adjustedValue = globalMinValue;
      // Serial.print(F("plunger at min: "));
      // Serial.print(adjustedValue);
      // Serial.print(F("\r\n"));
      // if (plungerMinSendCount == 62) {
      //   Serial.print("incrementor: ");
      //   Serial.print(incrementor);
      //   Serial.print(F("\r\n"));
      //   for (int i = 0; i < 63; i++) {
      //     Serial.print(i);
      //     Serial.print(F("-"));
      //     Serial.print(plungerData[i]);
      //     Serial.print(F("\r\n"));
      //   }
      // }
  } else if (sensorValue <= _config->plungerMid) {
    adjustedValue = sensorValue; 
  } else {
    adjustedValue = (sensorValue-_config->plungerMid) * plungerScaleFactor + _config->plungerMid;
    globalMinValue = 1024;
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
  if (priorValue != adjustedValue && _config->restingStateCounter < 200 && (priorValue - adjustedValue < 10 || plungerMinSendCount < 50)) {
    if (sensorValue > _config->plungerMid) {
      // Serial.print(static_cast<int8_t>(adjustedValue / localMax * 128));
      // Serial.print(F("\r\n"));
      Gamepad1.zAxis(static_cast<int8_t>(adjustedValue / localMax * 128));
    } else {
      // Serial.print(static_cast<int8_t>(-adjustedValue / localMin * 128));
      // Serial.print(F("\r\n"));
      Gamepad1.zAxis(static_cast<int8_t>(-adjustedValue / localMin * 128));
    }
    priorValue = adjustedValue;
  }
}

void Plunger::sendPlungerState() {
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print(F("\r\n"));
}
