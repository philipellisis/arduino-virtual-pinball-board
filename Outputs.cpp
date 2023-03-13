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

void Outputs::updateOutput(int outputId, int outputValue) {
  if (DEBUG) {Serial.print(F("output ")); Serial.print(outputId); Serial.print(F(" set to ")); Serial.println(outputValue);}
  if (outputValue == 255) {
    updateOutputActual(outputId, 4096, 0);
    outputValues[outputId] = 1;
  } else if (outputValue == 0) {
    updateOutputActual(outputId, 0, 4096);
    outputValues[outputId] = 0;
  } else {
    outputValues[outputId] = 1;
    updateOutputActual(outputId, 1, outputValue * 16);
  }
}

void Outputs::checkResetOutputs() {
  for (int i = 0; i < 48; i++) {
    if (outputValues[i] > 0 && _config->maxOutputTime[i] != 0) {
      outputValues[i]++;
      if (outputValues[i] == _config->maxOutputTime[i]) {
        if (DEBUG) {Serial.println(F("Output has reached max time, turning off"));}
        updateOutput(i, 0);
      }
    }
  }
}

void Outputs::updateOutputActual(int outputId, int outputValueStart, int outputValueFinish) {
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
