#include "Plunger.h"
#include <Arduino.h>
#include "MinimalHID.h"
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
  if (!config.flags.enablePlunger) {
    Gamepad1.zAxis(0);
    return;
  }

  int16_t sensorValue = 0;
  int16_t newReading;

  // Remove any sensor value that does not agree with the prior value
  uint8_t goodReadings = 0;
  for (uint8_t i = 0; i < 5; i++) {
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

  if (config.flags.plungerMoving || config.flags.disablePlungerWhenNotInUse == 0) {
    for (uint8_t i = 0; i < config.plungerAverageRead; i++) {
      sensorValue += analogRead(23);
    }
    sensorValue = sensorValue / (config.plungerAverageRead + 1);
  }
  truePriorValue = sensorValue;

  // this checks that the plunger is sitting stationary. If so, it will enable the accelerometer. It also checks if there is nothing connected. to ensure the accelerometer still works even if the plunger is disconnected
  uint32_t currentTime = millis();

  if ((sensorValue < config.plungerMid + 50 && sensorValue > config.plungerMid - 50) || sensorValue > 990) {
    if (currentTime - restingStartTime >= config.restingStateMax) {
      // Plunger is in the resting state when the timer exceeds restingStateMax
      config.flags.plungerMoving = false;
    }
  } else {
    // Reset the timer if the plunger is moving
    restingStartTime = currentTime;
    config.flags.plungerMoving = true;
  }

  // Handle plunger button push logic for max position
  if (config.plungerButtonPush == 1 || config.plungerButtonPush == 3) {
    updateButtonState(buttonState, sensorValue >= config.plungerMax - 20, true);
    updateButtonState(buttonState, sensorValue < config.plungerMax - 20, false);
  }
  
  // Handle plunger button push logic for min position
  if (config.plungerButtonPush >= 2) {
    updateButtonState(buttonState2, sensorValue <= config.plungerMin + 10, true);
    updateButtonState(buttonState2, sensorValue > config.plungerMin + 10, false);
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


  int8_t currentDelayedValue = getDelayedPlungerValue(adjustedValue, currentTime);

  if (priorValue != currentDelayedValue) {
    if (config.flags.plungerMoving) {
      if (adjustedValue > 0 && !plungerReleased) {
        currentPlungerMax = currentDelayedValue;
      }
      Gamepad1.zAxis(currentDelayedValue);
      config.flags.updateUSB = true;
    } else {
      currentPlungerMax = 0;
      plungerReleased = false;
      if (config.flags.disablePlungerWhenNotInUse == 1) {
        Gamepad1.zAxis(0);
      } else {
        Gamepad1.zAxis(currentDelayedValue);
        config.flags.updateUSB = true;
      }
    }
    priorValue = currentDelayedValue;
  }
  
  plungerData[plungerDataCounter] = adjustedValue;
  plungerDataTime[plungerDataCounter] = (uint8_t)(currentTime - priorTime);
  plungerDataCounter = (plungerDataCounter + 1) % 35;
  priorTime = currentTime;
}

int8_t Plunger::getDelayedPlungerValue(int8_t sensorValue, uint32_t currentTime) {

  if (config.enablePlungerQuickRelease == 0) {
    return sensorValue;
  }
  if (!config.flags.plungerMoving && plungerReleased) {
    plungerReleased = false;
    config.flags.updateUSB = true;
    return 0;
  }
  if ((sensorValue < 0 && config.flags.plungerMoving && currentPlungerMax > 0 && truePriorValue > 50) || plungerReleased) {
    if (!plungerReleased) {
      config.setLastButtonState(config.plungerLaunchButton, buttons.sendButtonPush(config.plungerLaunchButton, 1));
    } else {
      config.setLastButtonState(config.plungerLaunchButton, buttons.sendButtonPush(config.plungerLaunchButton, 0));
    }
    plungerReleased = true;
    return 0;
  }

  // 100ms delay
  uint16_t accumulatedTime = 0;
  int8_t index = (plungerDataCounter - 1 + 35) % 35;

  while (accumulatedTime < config.enablePlungerQuickRelease && index != plungerDataCounter) {
      accumulatedTime += plungerDataTime[index];
      index = (index - 1 + 35) % 35;
  }


  return plungerData[index];
}

void Plunger::updateButtonState(uint8_t& buttonState, bool condition, bool pressed) {
  if (condition && buttonState != pressed) {
    buttonState = buttons.sendButtonPush(config.plungerLaunchButton, pressed);
    config.setLastButtonState(config.plungerLaunchButton, buttonState);
  }
}

void Plunger::sendPlungerState() {
  Serial.print(F("P,"));
  Serial.print(analogRead(23));
  Serial.print(F("\r\n"));
}
