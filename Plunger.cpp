#include "Plunger.h"
#include <Arduino.h>
#include "HID-Project.h"
#include "Globals.h"


Plunger::Plunger() {
  

  pinMode(23, INPUT_PULLUP); //plunger
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: pins initialized\r\n"));}
}

void Plunger::init() {
  resetPlunger();
  //if (DEBUG) {Serial.print(F("DEBUG,plunger: initialized Gamepad1\r\n"));}
}

void Plunger::resetPlunger() {
  plungerScaleFactor = (float)(config.plungerMid - config.plungerMin) / (float)(config.plungerMax - config.plungerMid);
}

void Plunger::plungerRead() {
  if (config.enablePlunger == false) {
    Gamepad1.zAxis(0);
    return;
  }

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
  
  


  if (config.restingStateCounter < config.restingStateMax || config.disablePlungerWhenNotInUse == 0) {
    for (int i = 0; i < config.plungerAverageRead; i++) {
      sensorValue += analogRead(23);
    }
    sensorValue = sensorValue / (config.plungerAverageRead + 1);
  }
  truePriorValue = sensorValue;

  // this checks that the plunger is sitting stationary. If so, it will enable the accelerometer. It also checks if there is nothing connected. to ensure the accelerometer still works even if the plunger is disconnected
  if ((sensorValue < config.plungerMid + 50 && sensorValue > config.plungerMid - 50) || sensorValue > 990) {
    // plunger is in resting state when counter is > 200. Is moving or almost done moving when counter is > 0 and <= 200
    if (config.restingStateCounter < config.restingStateMax) {
      config.restingStateCounter++;
    }
  } else {
      //plunger is actively moving when counter is = 0
      config.restingStateCounter = 0;
  }
  // Serial.print("sensornorm: ");
  // Serial.print(sensorValue);
  // Serial.print("\r\n");

  if( (config.plungerButtonPush == 1 || config.plungerButtonPush == 3) && buttonState == 0 && sensorValue >= config.plungerMax - 20) {
    buttonState = buttons.sendButtonPush(config.plungerLaunchButton, 1);
  } else if ((config.plungerButtonPush == 1 || config.plungerButtonPush == 3)  && buttonState == 1 && sensorValue < config.plungerMax - 20 ) {
    buttonState = buttons.sendButtonPush(config.plungerLaunchButton, 0);
  }
  if( config.plungerButtonPush >= 2 && buttonState2 == 0 && sensorValue <= config.plungerMin + 10) {
    buttonState2 = buttons.sendButtonPush(config.plungerLaunchButton, 1);
  } else if (config.plungerButtonPush >= 2 && buttonState2 == 1 && sensorValue > config.plungerMin + 10 ) {
    buttonState2 = buttons.sendButtonPush(config.plungerLaunchButton, 0);
  }

  if (sensorValue <= config.plungerMid) {
    adjustedValue = static_cast<int8_t>((1 - (float)(sensorValue - config.plungerMin) / (config.plungerMid - config.plungerMin)) * -128);
    if (adjustedValue > 100) {
      adjustedValue = -127;
    }
  } else {
    adjustedValue = static_cast<int8_t>((float)(sensorValue - config.plungerMid) / (config.plungerMax - config.plungerMid) * 128);
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
  signed char currentDelayedValue = getDelayedPlungerValue(adjustedValue);

  //if (DEBUG) {Serial.print(F("DEBUG,plunger: scale factor ")); Serial.print(plungerScaleFactor); Serial.print(F("DEBUG,plunger: value ")); Serial.print(adjustedValue); Serial.print("\r\n");}
  if (priorValue != currentDelayedValue && config.restingStateCounter < config.restingStateMax ) {
    if (adjustedValue > 0 && plungerReleased == false) {
      currentPlungerMax = currentDelayedValue;
    }
    // Serial.print(adjustedValue);
    // Serial.print(F("\r\n"));
    config.updateUSB = true;
    Gamepad1.zAxis(currentDelayedValue);
    Serial.print("plunger in motion: ");
    Serial.print(currentDelayedValue);
    Serial.print(F("\r\n"));
    priorValue = currentDelayedValue;

  //is not moving
  } else if (priorValue != currentDelayedValue && config.restingStateCounter == config.restingStateMax  ) {
    currentPlungerMax = 0;
    plungerReleased = false;
    if (config.disablePlungerWhenNotInUse == 1) {
      Gamepad1.zAxis(0);
      // Serial.print("plunger not in motion, sending 0: ");
      // Serial.print(adjustedValue);
      // Serial.print(F("\r\n"));
      priorValue = currentDelayedValue;
    } else {
      Serial.print("plunger not in motion: ");
      Serial.print(currentDelayedValue);
      Serial.print(F("\r\n"));
      Gamepad1.zAxis(currentDelayedValue);
      config.updateUSB = true;
      priorValue = currentDelayedValue;
    }
  }
  
  plungerData[plungerDataCounter] = adjustedValue;
  if (plungerDataCounter < 49) {
    plungerDataCounter++;
  } else {
    plungerDataCounter = 0;
  }
  
}

signed char Plunger::getDelayedPlungerValue(signed char sensorValue) {
  if (config.restingStateCounter == config.restingStateMax && plungerReleased == true) {
    plungerReleased = false; 
    if (config.disablePlungerWhenNotInUse == 1) {
      config.updateUSB = true;
      return 0;
    }
  }
  if ((sensorValue < 0 && config.restingStateCounter < config.restingStateMax && currentPlungerMax > 0 && truePriorValue > 50) || plungerReleased == true) {
    plungerReleased = true;
    return -currentPlungerMax;
  }
  if (plungerDataCounter == 49) {
    return plungerData[0];
  } else {
    return plungerData[plungerDataCounter + 1];
  }
}

void Plunger::sendPlungerState() {
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print(F("\r\n"));
}