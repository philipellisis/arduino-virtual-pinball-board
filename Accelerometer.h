#ifndef ACCEL_H
#define ACCEL_H
#include <Arduino.h>
#include "HID-Project.h"
#include "Config.h"



class Accelerometer {
  
  public:
    Accelerometer();
    void init(Config* config);
    void accelerometerRead();
    void sendAccelerometerState();
    void resetAccelerometer();
    void centerAccelerometer();
    
  private:
    bool DEBUG = false;
    Config* _config;
    float xValueOffset = 0;
    float yValueOffset = 0;
    int xValue;
    int yValue;
    int priorXValue = 0;
    int priorYValue = 0;
    byte buttonState;
    bool recentered = false;
    byte orientation = 0;
    float localMax = 0;
};

#endif
