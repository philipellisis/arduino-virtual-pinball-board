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
    void resetPlunger();
    
  private:
    bool DEBUG = false;
    Joystick_* _joystick;
    float plungerScaleFactor;
    int adjustedValue;
    Config* _config;
    byte buttonState;
    byte buttonState2;
    int priorValue = 0;
};

#endif
