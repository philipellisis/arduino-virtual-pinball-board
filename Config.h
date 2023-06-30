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
    byte turnOffState[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputState[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputTime[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int plungerMax = 634;
    int plungerMin = 10;
    int plungerMid = 100;
    byte solenoidButtonMap[4] = {0,0,0,0};
    byte solenoidOutputMap[4] = {0,0,0,0};
    byte orientation = 0;
    byte accelerometerEprom = 0;
    byte accelerometer = 1;
    int accelerometerMultiplier = 100;
    int accelerometerDeadZone = 10;
    byte plungerButtonPush = 0;
    

    bool done = false;
    bool nightMode = false;
    void updateConfigFromSerial();
    void sendConfig();
    void setPlunger();
    void setAccelerometer();
    
  private:
    bool DEBUG = false;
    byte Config::blockRead();
    void writeIntIntoEEPROM(int address, int number);
    int readIntFromEEPROM(int address);
    void writeConfigMessage(int errorCode);
    
    
};

#endif
