#include "Outputs.h"
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "Enums.h"
#include "Globals.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);
Outputs::Outputs() {
   for (uint8_t index = 0; index < numberOutputs; index++) {
    pinMode(outputList[index], OUTPUT);
   }
   
  
}

void Outputs::init() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(90);  // This is the maximum PWM frequency
  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(90);  // This is the maximum PWM frequency
  pwm2.begin();
  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(90);  // This is the maximum PWM frequency
  Wire.setClock(400000);
  Wire.setWireTimeout(3000, true);

  turnOff();

}

void Outputs::turnOff() {
  for (uint8_t i = 0; i < 62; i++) {
    updateOutput(i, 0);
  }
}

void Outputs::updateOutput(unsigned char outputId, unsigned char outputValue) {
  if (outputValue > config.maxOutputState[outputId]) {
    outputValue = config.maxOutputState[outputId];
  }
  if (config.flags.nightMode && (config.toySpecialOption[outputId] == NOISY || config.toySpecialOption[outputId] == SHARED)) {
    outputValue = 0;
  }
  if (config.toySpecialOption[outputId] == SHARED && outputValue > 0 && outputValues[outputId] > 0) {
    for (uint8_t i = 0; i < 9; i++) {
      if(config.toySpecialOption[i] == SHARED && outputValues[i] == 0) {
        outputId = i;
        SET_BIT(virtualOutputOnPacked, i, 1);
        break;
      }
    }
  }
  if (config.toySpecialOption[outputId] == SHARED && outputValue == 0 && outputId < 10) {
    if (GET_BIT(virtualOutputOnPacked, outputId) == 0) {
      for (uint8_t i = 0; i < 9; i++) {
        if(config.toySpecialOption[i] == SHARED && outputValues[i] > 0 && GET_BIT(virtualOutputOnPacked, i) > 0) {
          updateOutputInternal(i, 0);
          SET_BIT(virtualOutputOnPacked, i, 0);
        }
      }
    }
    updateOutputInternal(outputId, outputValue);
  } else {
    updateOutputInternal(outputId, outputValue);
  }
}

void Outputs::updateOutputInternal(unsigned char outputId, unsigned char outputValue) {
  outputValues[outputId] = outputValue;
  if (outputValue != 0) {
    timeTurnedOn[outputId] = millis();
  }
  if (outputId < 15) {
    if (outputId < 5) {
      analogWrite(outputList[outputId], outputValue);
    } else {
      if (config.flags.bluetoothEnable && outputId == 10) {
        return; // do not update the bluetooth output if bluetooth is enabled, as this pin is being used by SS
      }
      if (outputValue > 127) {
        digitalWrite(outputList[outputId], HIGH);
      } else {
        digitalWrite(outputList[outputId], LOW);
      }
    }
  } else {
    // if the output is the button board output, then invert the value
    if (outputId < 31 && config.flags.reverseButtonOutputPolarity) {
      outputValue = 255 - outputValue;
    }
    if (outputValue == 255) {
      updateOutputActual(outputId - 15, 4096, 0);
    } else if (outputValue == 0) {
      updateOutputActual(outputId - 15, 0, 4096);
    } else {
      updateOutputActual(outputId - 15, 1, outputValue * 16);
    }
  }
  

}

void Outputs::checkResetOutputs() {
  long int t1 = millis();
  for(uint8_t i = 0; i < 5; i++) {
    resetOutputNumber = (resetOutputNumber + 1) % 63;
    if (outputValues[resetOutputNumber] > config.turnOffState[resetOutputNumber] && config.maxOutputTime[resetOutputNumber] > 0) {
      if (t1 - timeTurnedOn[resetOutputNumber] >= config.maxOutputTime[resetOutputNumber] * 100) {
        updateOutput(resetOutputNumber, config.turnOffState[resetOutputNumber]);
      }
    }
  }
}

void Outputs::updateOutputActual(unsigned char outputId, int outputValueStart, int outputValueFinish) {
  if (outputId < 16) {
    pwm.setPWM(outputId, outputValueStart, outputValueFinish);
  }
  else if (outputId >= 16 && outputId < 32) {
    pwm1.setPWM(outputId - 16, outputValueStart, outputValueFinish);
  }
  else if (outputId >= 32 && outputId < 48) {
    pwm2.setPWM(outputId - 32, outputValueStart, outputValueFinish);
  }
}

void Outputs::sendOutputState() {
  Serial.print(F("O,"));
  for (uint8_t i = 0; i < 62; i++) {
    Serial.print(outputValues[i]);
    Serial.print(F(","));
  }
  Serial.print(outputValues[62]);
  Serial.print(F("\r\n"));
}
