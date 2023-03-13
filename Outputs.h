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
    void updateOutput(int outputId, int outputValue);
    void sendOutputState();
    void checkResetOutputs();
    
  private:
    Config* _config;
    bool DEBUG = true;
    byte numberOutputs = 7;
    byte outputList[7] = {5,6,7,8,9,10,12};
    byte outputValues[48] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    void updateOutputActual(int outputId, int outputValueStart, int outputValueFinish);
};

#endif
