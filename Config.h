#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>


class Config {
  
  public:
    Config();
    void saveConfig();
    void init();
    //first unsigned char is for noisy toy, other 7 bits can be used for other things
    unsigned char toySpecialOption[63] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char turnOffState[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char maxOutputState[63] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
    unsigned char maxOutputTime[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int plungerMax = 842;
    int plungerMin = 61;
    int plungerMid = 203;
    unsigned char solenoidButtonMap[4] = {0,0,0,0};
    unsigned char solenoidOutputMap[4] = {0,0,0,0};
    unsigned char orientation = 0;
    unsigned char accelerometerEprom = 0;
    unsigned char accelerometer = 1;
    unsigned char accelerometerSensitivity = 0;
    int accelerometerDeadZone = 30;
    int accelerometerTilt = 250;
    int accelerometerTiltY = 250;
    int accelerometerMax = 280;
    int accelerometerMaxY = 280;
    unsigned char plungerButtonPush = 0;

    unsigned char plungerAverageRead = 10;
    unsigned char nightModeButton = 14;
    unsigned char plungerLaunchButton = 23;
    unsigned char tiltButton = 22;
    unsigned char shiftButton = 2;

    unsigned char done = 0;
    bool nightMode = false;
    void updateConfigFromSerial();
    void sendConfig();
    void setPlunger();
    void setAccelerometer();

    unsigned char lightShowState = 1;

    unsigned char buttonKeyboard[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char buttonKeyDebounce[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool lastButtonState[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char buttonDebounceCounter = 0;
    bool plungerMoving = false; 
    long restingStateMax = 160;
    bool updateUSB = false;
    bool buttonPressed = false;
    // 0 is ignore accel option
    // 1 is quick release option
    // 2 ignore when not in use option
    // 3 
    bool disableAccelOnPlungerMove = true;
    bool enablePlungerQuickRelease = true;
    bool disablePlungerWhenNotInUse = true;
    bool disableButtonPressWhenKeyboardEnabled = true;
    bool enablePlunger = true;

    unsigned char tiltSuppress = 10;
    bool lightShowAttractEnabled = true;
    unsigned char lightShowTime = 10;
    bool reverseButtonOutputPolarity = true;
    unsigned char plungerDelayTime = 100;
    


  private:
    bool DEBUG = false;
    unsigned char Config::blockRead();
    void writeIntIntoEEPROM(int address, int number);
    int readIntFromEEPROM(int address);
    void printError();
    void printComma(unsigned char value);
    void printIntComma(int value);
    void printSuccess();
    int readIntFromByte();
    void readConfigArray(unsigned char* configArray, unsigned char size);
    void printConfigArray(unsigned char* configArray, unsigned char size);
    
    
};

#endif
