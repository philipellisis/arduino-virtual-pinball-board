#ifndef PLUNGER_H
#define PLUNGER_H
#include <Arduino.h>
#include <Joystick.h>
#include "Config.h"

class Plunger {
  
  public:
    Plunger();
    void init(Joystick_* joystick, Config* config);
    void plungerRead();
    void sendPlungerState();
    
  private:
    bool DEBUG = false;
    Joystick_* _joystick;
    float plungerScaleFactor;
    int adjustedValue;
    Config* _config;
};

#endif
