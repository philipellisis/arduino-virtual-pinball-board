#ifndef PLUNGER_H
#define PLUNGER_H
#include <Arduino.h>
#include "HID-Project.h"
#include "Config.h"

class Plunger {
  
  public:
    Plunger();
    void init(Config* config);
    void plungerRead();
    void sendPlungerState();
    void resetPlunger();
    
  private:
    bool DEBUG = false;
    float plungerScaleFactor;
    int adjustedValue;
    Config* _config;
    byte buttonState;
    byte buttonState2;
    int priorValue = 0;
    int globalMinValue = 1024;
    byte plungerMinSendCount = 0;
    float localMax = 0;
    float localMin = 0;
    // int plungerData[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // byte incrementor = 0;
};

#endif
