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
    int getXValue() const { return xValue; }
    int getYValue() const { return yValue; }
    
  private:
    bool DEBUG = false;
    int xValueOffset = 0;
    int yValueOffset = 0;
    int xValue;
    int yValue;
    int priorXValue = 0;
    int priorYValue = 0;
    bool recentered = false;
    unsigned char orientation = 0;
    int localMax = 0;
    int localMaxY = 0;
    unsigned char tiltSuppressTime = 0;
};

#endif
