#include "Outputs.h"
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Outputs::Outputs() {
  if (DEBUG) {Serial.println("Communication: About to initialize serial");}
   for (int index = 0; index < numberOutputs; index++) {
    pinMode(outputList[index], OUTPUT);
   }
  
  if (DEBUG) {Serial.println("Communication: pins initialized");}
}

void Outputs::init() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  Wire.setClock(400000);
}

void Outputs::updateOutput(int outputId, int outputValue) {
  if (DEBUG) {Serial.print("output "); Serial.print(outputId); Serial.print(" set to "); Serial.println(outputValue);}
  if (outputValue == 255) {
    pwm.setPWM(outputId, 4096, 0);
    outputValues[outputId] = 255;
  } else if (outputValue == 0) {
    pwm.setPWM(outputId, 0, 4096);
    outputValues[outputId] = 0;
  } else {
    outputValues[outputId] = outputValue;
    pwm.setPWM(outputId, 1, outputValue * 16);
  }
}

void Outputs::sendOutputState() {
  for (int i : outputValues) {
    Serial.print(i);
  }
  Serial.println("");
}
