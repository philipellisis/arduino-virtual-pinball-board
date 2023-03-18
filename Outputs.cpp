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
  if (DEBUG) {Serial.print(F("output ")); Serial.print(outputId); Serial.print(F(" set to ")); Serial.println(outputValue);}
  if (outputId < 16) {
    if (outputId < 5) {
      if (outputValue > 0) {
        outputValues[outputId] = 1;
      } else {
        outputValues[outputId] = 0;
      }
      analogWrite(outputList[outputId], outputValue);
    } else {
      if (outputValue > 127) {
        digitalWrite(outputList[outputId], HIGH);
        outputValues[outputId] = 1;
      } else {
        digitalWrite(outputList[outputId], LOW);
        outputValues[outputId] = 0;
      }
    }
  } else {
    if (outputValue == 255) {
      updateOutputActual(outputId - 15, 4096, 0);
      outputValues[outputId] = 1;
    } else if (outputValue == 0) {
      updateOutputActual(outputId - 15, 0, 4096);
      outputValues[outputId] = 0;
    } else {
      outputValues[outputId] = 1;
      updateOutputActual(outputId - 15, 1, outputValue * 16);
    }
  }
  

}

void Outputs::checkResetOutputs() {
  if (resetOutputNumber < 62) {
    resetOutputNumber++;
  } else {
    resetOutputNumber = 0;
  }
  if (outputValues[resetOutputNumber] > 0 && _config->maxOutputTime[resetOutputNumber] > 0) {
    if (DEBUG) {Serial.print(F("Output is turned on, checking if it has been on for too long "));Serial.println(resetOutputNumber);}
    if (outputValues[resetOutputNumber] == _config->maxOutputTime[resetOutputNumber]) {
      if (DEBUG) {Serial.println(F("Output has reached max time, turning off"));}
      updateOutput(resetOutputNumber, 0);
    }
    outputValues[resetOutputNumber]++;
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
