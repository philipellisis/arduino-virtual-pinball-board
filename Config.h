#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

class Config {
  
  public:
    Config();
    void saveConfig();
    void init();
    byte maxOutputState[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputTime[63] = {2,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int plungerMax = 634;
    byte plungerMin = 10;
    byte plungerMid = 100;
    byte solenoidButtonMap[4] = {1,2,3,4};
    byte solenoidOutputMap[4] = {0,0,1,1};
  private:
    bool DEBUG = true;
    
};

#endif
