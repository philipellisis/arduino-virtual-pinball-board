#include "Outputs.h"
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Outputs::Outputs() {
  Serial.println("Communication: About to initialize serial");
   for (int index = 0; index < numberOutputs; index++) {
    pinMode(outputList[index], OUTPUT);
   }
  
  Serial.println("Communication: pins initialized");
}

void Outputs::init() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  Wire.setClock(400000);
}

void Outputs::updateOutput(int outputId, int outputValue) {
  
}
