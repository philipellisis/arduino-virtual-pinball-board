#ifndef ACCEL_H
#define ACCEL_H
#include <Arduino.h>
#include <Joystick.h>
#include "Config.h"


class Accelerometer {
  
  public:
    Accelerometer();
    void init(Joystick_* joystick, Config* config);
    void accelerometerRead();
    void sendAccelerometerState();
    void resetAccelerometer();
    void centerAccelerometer();
    
  private:
    bool DEBUG = false;
    Config* _config;
    float xValueOffset = 0;
    float yValueOffset = 0;
    Joystick_* _joystick;
    int xValue;
    int yValue;
    int priorXValue;
    int priorYValue;
    byte buttonState;
    bool recentered = false;
};

#endif
