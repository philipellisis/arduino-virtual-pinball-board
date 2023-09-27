#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

class Config {
  
  public:
    Config();
    void saveConfig();
    void init();
    //first byte is for noisy toy, other 7 bits can be used for other things
    byte toySpecialOption[63] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte turnOffState[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte maxOutputState[63] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
    byte maxOutputTime[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int plungerMax = 842;
    int plungerMin = 61;
    int plungerMid = 203;
    byte solenoidButtonMap[4] = {0,0,0,0};
    byte solenoidOutputMap[4] = {0,0,0,0};
    byte orientation = 0;
    byte accelerometerEprom = 0;
    byte accelerometer = 1;
    byte accelerometerSensitivity = 0;
    int accelerometerDeadZone = 30;
    int accelerometerTilt = 800;
    int accelerometerMax = 1000;
    byte plungerButtonPush = 0;

    byte plungerAverageRead = 10;
    byte nightModeButton = 14;
    byte plungerLaunchButton = 23;
    byte tiltButton = 22;
    byte shiftButton = 2;

    byte done = 0;
    bool nightMode = false;
    void updateConfigFromSerial();
    void sendConfig();
    void setPlunger();
    void setAccelerometer();

    byte lightShowState = 1;

    byte buttonKeyboard[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte restingStateCounter = 200;
  private:
    bool DEBUG = false;
    byte Config::blockRead();
    void writeIntIntoEEPROM(int address, int number);
    int readIntFromEEPROM(int address);
    void printError();
    void printComma(byte value);
    void printIntComma(int value);
    void printSuccess();
    int readIntFromByte();
    void readConfigArray(byte* configArray, byte size);
    void printConfigArray(byte* configArray, byte size);
    
    
};

#endif
