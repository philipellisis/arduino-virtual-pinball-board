#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

class Config {
  
  public:
    Config();
    void saveConfig();
    void init();
    //first byte is for noisy toy, other 7 bits can be used for other things
    byte toySpecialOption[63] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte turnOffState[63] = {0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputState[63] = {0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputTime[63] = {2,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int plungerMax = 634;
    int plungerMin = 10;
    int plungerMid = 100;
    byte solenoidButtonMap[4] = {1,2,3,4};
    byte solenoidOutputMap[4] = {0,0,1,1};
    byte orientation = 0;
    byte accelerometer = 0;
    

    bool done = false;
    bool nightMode = false;
    void updateConfigFromSerial();
    void sendConfig();
    void setPlunger();
    
  private:
    bool DEBUG = false;
    byte Config::blockRead();
    void writeIntIntoEEPROM(int address, int number);
    int readIntFromEEPROM(int address);
    
    
};

#endif
