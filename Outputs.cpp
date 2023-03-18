#include "Outputs.h"
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);
Outputs::Outputs() {
   for (int index = 0; index < numberOutputs; index++) {
    pinMode(outputList[index], OUTPUT);
   }
  
  if (DEBUG) {Serial.println(F("Communication: pins initialized"));}
}

void Outputs::init(Config* config) {
  _config = config;
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(1600);  // This is the maximum PWM frequency
  pwm2.begin();
  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(1600);  // This is the maximum PWM frequency
  Wire.setClock(400000);

}

void Outputs::updateOutput(byte outputId, byte outputValue) {
  if (_config->maxOutputState[outputId] > 0 && outputValue > _config->maxOutputState[outputId]) {
    outputValue = _config->maxOutputState[outputId];
  }
  if (_config->nightMode == true && bitRead(_config->toySpecialOption[outputId],0) == 1) {
    outputValue = 0;
  }
  if (DEBUG) {Serial.print(F("output ")); Serial.print(outputId); Serial.print(F(" set to ")); Serial.println(outputValue);}
  outputValues[outputId] = outputValue;
  if (outputValue == 0) {
    timeTurnedOn[outputId] = 0;
  }
  if (outputId < 16) {
    if (outputId < 5) {
      analogWrite(outputList[outputId], outputValue);
    } else {
      if (outputValue > 127) {
        digitalWrite(outputList[outputId], HIGH);
      } else {
        digitalWrite(outputList[outputId], LOW);
      }
    }
  } else {
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
  for(int i = 0; i < 5; i++) {
    if (resetOutputNumber < 62) {
      resetOutputNumber++;
    } else {
      resetOutputNumber = 0;
    }
    if (outputValues[resetOutputNumber] > _config->turnOffState[resetOutputNumber] && _config->maxOutputTime[resetOutputNumber] > 0) {
      if (DEBUG) {Serial.print(F("Output is turned on, checking if it has been on for too long "));Serial.println(resetOutputNumber);}
      if (timeTurnedOn[resetOutputNumber] == _config->maxOutputTime[resetOutputNumber]) {
        if (DEBUG) {Serial.println(F("Output has reached max time, turning off"));}
        updateOutput(resetOutputNumber, _config->turnOffState[resetOutputNumber]);
      } else {
        timeTurnedOn[resetOutputNumber]++;
      }
      
    }
  }
}

void Outputs::updateOutputActual(byte outputId, int outputValueStart, int outputValueFinish) {
  if (outputId < 16) {
    pwm.setPWM(outputId, outputValueStart, outputValueFinish);
  }
  else if (outputId >= 16 && outputId < 32) {
    pwm1.setPWM(outputId, outputValueStart, outputValueFinish);
  }
  else if (outputId >= 32 && outputId < 48) {
    pwm2.setPWM(outputId, outputValueStart, outputValueFinish);
  }
}

void Outputs::sendOutputState() {
  for (int i : outputValues) {
    Serial.print(i);
  }
  Serial.println("");
}
