#ifndef OUTPUTS_H
#define OUTPUTS_H
#include <Arduino.h>
#include <Joystick.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "Config.h"


class Outputs {
  
  public:
    Outputs();
    void init(Config* config);
    void updateOutput(byte outputId, byte outputValue);
    void sendOutputState();
    void checkResetOutputs();
    
  private:
    Config* _config;
    bool DEBUG = true;
    byte numberOutputs = 15;
    byte outputList[15] = {5,6,10,9,8,7,12,18,19,20,21,22,14,15,16};
    byte outputValues[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    void updateOutputActual(byte outputId, int outputValueStart, int outputValueFinish);
    byte resetOutputNumber = 0;
};

#endif
