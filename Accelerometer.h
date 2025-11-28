#ifndef ACCEL_H
#define ACCEL_H
#include <Arduino.h>
#include "MinimalHID.h"
#include "Config.h"



class Accelerometer {
  
  public:
    Accelerometer();
    void init();
    void accelerometerRead();
    void sendAccelerometerState();
    void resetAccelerometer();
    void centerAccelerometer();
    int16_t getXValue() const { return xValue; }
    int16_t getYValue() const { return yValue; }
    void applyOrientationTransform(int16_t& x, int16_t& y);
    int16_t getRawAccelValue();
    void processTiltButton();
    void updateXAxis();
    void updateYAxis();
    
  private:
    int16_t xValueOffset = 0;
    int16_t yValueOffset = 0;
    int16_t xValue;
    int16_t yValue;
    int16_t priorXValue = 0;
    int16_t priorYValue = 0;
    bool recentered = false;
    uint8_t orientation = 0;
    int16_t localMax = 0;
    int16_t localMaxY = 0;
    uint8_t tiltSuppressTime = 0;
};

#endif
