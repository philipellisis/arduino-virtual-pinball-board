#include "Plunger.h"
#include <Arduino.h>
#include "HID-Project.h"
#include "Globals.h"


Plunger::Plunger() {
  pinMode(23, INPUT_PULLUP); // plunger

  restingStartTime = millis(); // Initialize the timestamp
}

void Plunger::init() {
  resetPlunger();
  priorTime = millis();
}

void Plunger::resetPlunger() {
  plungerScaleFactor = (float)(config.plungerMid - config.plungerMin) / (float)(config.plungerMax - config.plungerMid);
  restingStartTime = millis(); // Reset the resting start time
}

void Plunger::plungerRead() {
  if (config.enablePlunger == false) {
    Gamepad1.zAxis(0);
    return;
  }

  long sensorValue = 0;
  int newReading;

  // Remove any sensor value that does not agree with the prior value
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

  if (config.plungerMoving == true || config.disablePlungerWhenNotInUse == 0) {
    for (int i = 0; i < config.plungerAverageRead; i++) {
      sensorValue += analogRead(23);
    }
    sensorValue = sensorValue / (config.plungerAverageRead + 1);
  }
  truePriorValue = sensorValue;

  // this checks that the plunger is sitting stationary. If so, it will enable the accelerometer. It also checks if there is nothing connected. to ensure the accelerometer still works even if the plunger is disconnected
  unsigned long currentTime = millis();

  if ((sensorValue < config.plungerMid + 50 && sensorValue > config.plungerMid - 50) || sensorValue > 990) {
    if (currentTime - restingStartTime >= config.restingStateMax) {
      // Plunger is in the resting state when the timer exceeds restingStateMax
      config.plungerMoving = false;
    }
  } else {
    // Reset the timer if the plunger is moving
    restingStartTime = currentTime;
    config.plungerMoving = true;
  }

  if ((config.plungerButtonPush == 1 || config.plungerButtonPush == 3) && buttonState == 0 && sensorValue >= config.plungerMax - 20) {
    buttonState = buttons.sendButtonPush(config.plungerLaunchButton, 1);
    config.lastButtonState[config.plungerLaunchButton] = buttonState;
  } else if ((config.plungerButtonPush == 1 || config.plungerButtonPush == 3) && buttonState == 1 && sensorValue < config.plungerMax - 20) {
    buttonState = buttons.sendButtonPush(config.plungerLaunchButton, 0);
    config.lastButtonState[config.plungerLaunchButton] = buttonState;
  }
  if (config.plungerButtonPush >= 2 && buttonState2 == 0 && sensorValue <= config.plungerMin + 10) {
    buttonState2 = buttons.sendButtonPush(config.plungerLaunchButton, 1);
    config.lastButtonState[config.plungerLaunchButton] = buttonState2;
  } else if (config.plungerButtonPush >= 2 && buttonState2 == 1 && sensorValue > config.plungerMin + 10) {
    buttonState2 = buttons.sendButtonPush(config.plungerLaunchButton, 0);
    config.lastButtonState[config.plungerLaunchButton] = buttonState2;
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


  signed char currentDelayedValue = getDelayedPlungerValue(adjustedValue, currentTime);

  if (priorValue != currentDelayedValue && config.plungerMoving == true ) {
    if (adjustedValue > 0 && plungerReleased == false) {
      currentPlungerMax = currentDelayedValue;
    }
    config.updateUSB = true;
    Gamepad1.zAxis(currentDelayedValue);
    // Serial.print("plunger in motion: ");
    // Serial.print(currentDelayedValue);
    // Serial.print(F("\r\n"));
    priorValue = currentDelayedValue;
  } else if (priorValue != currentDelayedValue && config.plungerMoving == false) {
    currentPlungerMax = 0;
    plungerReleased = false;
    if (config.disablePlungerWhenNotInUse == 1) {
      Gamepad1.zAxis(0);
      priorValue = currentDelayedValue;
    } else {
      Gamepad1.zAxis(currentDelayedValue);
      config.updateUSB = true;
      priorValue = currentDelayedValue;
    }
  }
  
  plungerData[plungerDataCounter] = adjustedValue;
  plungerDataTime[plungerDataCounter] = (unsigned char)(currentTime - priorTime);
  if (plungerDataCounter < 34) {
    plungerDataCounter++;
  } else {
    plungerDataCounter = 0;
  }
  priorTime = currentTime;
}

signed char Plunger::getDelayedPlungerValue(signed char sensorValue, unsigned long currentTime) {

  if (config.enablePlungerQuickRelease == 0) {
    return sensorValue;
  }
  if (config.plungerMoving == false && plungerReleased == true) {
    plungerReleased = false;
    config.updateUSB = true;
    return 0;
  }
  if ((sensorValue < 0 && config.plungerMoving == true && currentPlungerMax > 0 && truePriorValue > 50) || plungerReleased == true) {
    if (plungerReleased == false) {
      config.lastButtonState[config.plungerLaunchButton] = buttons.sendButtonPush(config.plungerLaunchButton, 1);
    } else {
      config.lastButtonState[config.plungerLaunchButton] = buttons.sendButtonPush(config.plungerLaunchButton, 0);
    }
    plungerReleased = true;
    return 0;
  }

  // 100ms delay
  unsigned short accumulatedTime = 0;
  int index = plungerDataCounter == 0 ? 34 : plungerDataCounter - 1;

  while (accumulatedTime < config.enablePlungerQuickRelease && index != plungerDataCounter) {
      accumulatedTime += plungerDataTime[index];
      index = (index - 1 + 35) % 35;
  }


  // Serial.print("plunger delay timer: ");
  // Serial.print(index);
  // Serial.print(", ");
  // Serial.print(plungerDataCounter);
  // Serial.print(F("\r\n"));
  return plungerData[index];
  // if (plungerDataCounter == 25) {
  //   return plungerData[0];
  // } else {
  //   return plungerData[plungerDataCounter + 1];
  // }
}

void Plunger::sendPlungerState() {
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print(F("\r\n"));
}
