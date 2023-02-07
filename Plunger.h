#ifndef PLUNGER_H
#define PLUNGER_H
#include <Arduino.h>
#include <Joystick.h>


class Plunger {
  
  public:
    Plunger();
    void init(Joystick_* joystick);
    void plungerRead();
    int getMid();
    int getMax();
    int getMin();
    
  private:
    Joystick_* _joystick;
    float plungerScaleFactor;
    int plungerMid;
    int plungerMax;
    int plungerMin;
};

#endif
