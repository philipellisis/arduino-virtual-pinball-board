#ifndef PLUNGER_H
#define PLUNGER_H
#include <Arduino.h>
#include "Config.h"

class Plunger {
  
  public:
    Plunger();
    void init();
    void plungerRead();
    void sendPlungerState();
    void resetPlunger();
    
  private:
    bool plungerReleased = false;
    bool DEBUG = false;
    float plungerScaleFactor;
    int adjustedValue;
    unsigned char buttonState;
    unsigned char buttonState2;
    long priorValue = 0;
    long truePriorValue = 0;
    long restingStartTime;
    unsigned long priorTime = 0;
    //unsigned char plungerMinSendCount = 0;

    int plungerData[35] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char plungerDataTime[35] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // unsigned char incrementor = 0;
    signed char plungerDataCounter = 0;
    signed char getDelayedPlungerValue(signed char sensorValue, unsigned long currentTime);
    signed char currentPlungerMax = 0;

};

#endif
