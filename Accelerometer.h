#ifndef ACCEL_H
#define ACCEL_H
#include <Arduino.h>
#include <Joystick.h>


class Accelerometer {
  
  public:
    Accelerometer();
    void init(Joystick_* joystick);
    void accelerometerRead();
    
  private:
    Joystick_* _joystick;
    float xValue;
    float yValue;
};

#endif
