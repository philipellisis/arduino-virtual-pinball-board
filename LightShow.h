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
    void flashLights();
    
  private:
    uint32_t timeInState;
    uint8_t outputValues[63] = {0};
    uint8_t outputDirection[63] = {0};
    uint8_t startLight = 0;
    uint8_t finishLight = 0;
    uint8_t incrementor = 5;

    uint8_t currentStartLight = 0;
    uint8_t currentFinishLight = 0;
    bool doneSettingLights = true;
    void setStartFinishLoops();
    bool isLightShowOutput(int i);
    bool isMediumLight(int i);
    void transitionToState(uint8_t newState, uint32_t currentTime);
    void updateLightValue(uint8_t i, uint8_t value);
    uint32_t lightShowStartTime = 10000;
};

#endif
