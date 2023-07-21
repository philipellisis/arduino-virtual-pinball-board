#ifndef LIGHTSHOW_H
#define LIGHTSHOW_H
#include <Arduino.h>
#include "Config.h"
#include "Outputs.h"


class LightShow {
  
  public:
    LightShow();
    void init(Config* config, Outputs* outputs);
    void setLightsNormal();
    void incrementRandom();
    void setLightsRandom();
    void setLightsHigh();
    void checkSetLights();
    
  private:
    Config* _config;
    Outputs* _outputs;
    bool DEBUG = false;
    long int timeInState;
    byte outputValues[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte outputDirection[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte startLight = 0;
    byte finishLight = 0;
    byte incrementor = 5;

    byte currentStartLight = 0;
    byte currentFinishLight = 0;
    bool doneSettingLights = true;
    void setStartFinishLoops();
};

#endif
