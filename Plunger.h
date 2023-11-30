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
    unsigned char buttonState;
    unsigned char buttonState2;
    long priorValue = 0;
    long truePriorValue = 0;
    //unsigned char plungerMinSendCount = 0;

    // int plungerData[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // unsigned char incrementor = 0;
};

#endif
