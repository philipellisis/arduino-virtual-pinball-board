#ifndef OUTPUTS_H
#define OUTPUTS_H
#include <Arduino.h>
#include <Joystick.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>


class Outputs {
  
  public:
    Outputs();
    void init();
    void updateOutput(int outputId, int outputValue);
    
  private:
    int numberOutputs = 7;
    int outputList[7] = {5,6,7,8,9,10,12};
};

#endif