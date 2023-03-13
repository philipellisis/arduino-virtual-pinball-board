#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

class Config {
  
  public:
    Config();
    void saveConfig();
    void init();
    byte maxOutputState[48] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputTime[48] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
    int plungerMax = 634;
    byte plungerMin = 10;
    byte plungerMid = 100;
    byte solenoidButtonMap[4] = {0,0,0,0};
    byte solenoidOutputMap[4] = {0,0,0,0};
  private:
    bool DEBUG = true;
    
};

#endif
