#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

class Config {
  
  public:
    Config();
    void saveConfig();
    void init();
    byte maxOutputState0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputState1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputState2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    byte maxOutputTime0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputTime1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputTime2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
  private:
    bool DEBUG = true;
    
};

#endif
