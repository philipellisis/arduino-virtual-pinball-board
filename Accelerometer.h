#ifndef ACCEL_H
#define ACCEL_H
#include <Arduino.h>
#include "HID-Project.h"
#include "Config.h"



class Accelerometer {
  
  public:
    Accelerometer();
    void init();
    void accelerometerRead();
    void sendAccelerometerState();
    void resetAccelerometer();
    void centerAccelerometer();
    
  private:
    bool DEBUG = false;
    float xValueOffset = 0;
    float yValueOffset = 0;
    int xValue;
    int yValue;
    int priorXValue = 0;
    int priorYValue = 0;
    unsigned char buttonState;
    bool recentered = false;
    unsigned char orientation = 0;
    float localMax = 0;
    float localMaxY = 0;
    unsigned char tiltSuppressTime = 0;
};

#endif
