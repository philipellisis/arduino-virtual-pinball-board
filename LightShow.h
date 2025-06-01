#ifndef LIGHTSHOW_H
#define LIGHTSHOW_H
#include <Arduino.h>
#include "Config.h"
#include "Outputs.h"


class LightShow {
  
  public:
    LightShow();
    void init();
    void setLightsNormal();
    void incrementRandom();
    void setLightsRandom();
    void setLightsHigh();
    void checkSetLights();
    void setLightsOff();
    
  private:
    long int timeInState;
    unsigned char outputValues[63] = {0};
    unsigned char outputDirection[63] = {0};
    unsigned char startLight = 0;
    unsigned char finishLight = 0;
    unsigned char incrementor = 5;

    unsigned char currentStartLight = 0;
    unsigned char currentFinishLight = 0;
    bool doneSettingLights = true;
    void setStartFinishLoops();
    bool isLightShowOutput(int i);
    bool isMediumLight(int i);
    void transitionToState(unsigned char newState, long currentTime);
    long lightShowStartTime = 10000;
};

#endif
