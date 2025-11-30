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
    uint32_t timeInState;
    uint8_t outputValues[63] = {0};
    uint64_t outputDirectionPacked = 0;  // Bit-packed (63 bits = 8 bytes instead of 63)
    uint8_t startLight = 0;
    uint8_t finishLight = 0;
    uint8_t incrementor = 5;

    uint8_t currentStartLight = 0;
    uint8_t currentFinishLight = 0;
    bool doneSettingLights = true;
    void setStartFinishLoops();
    inline void transitionToState(uint8_t newState, uint32_t currentTime);
    uint32_t lightShowStartTime = 10000;
};

#endif
