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
    unsigned char solenoidButtonMap[4] = {0};
    unsigned char solenoidOutputMap[4] = {0};
    unsigned char irOutputPin = 255;
    unsigned char irButton = 255;
    unsigned char irProtocol = 0;
    uint32_t irCode = 0;
    unsigned char irBits = 0;
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

    unsigned char tiltButtonUp = 9;
    unsigned char tiltButtonDown = 10;
    unsigned char tiltButtonLeft = 11;
    unsigned char tiltButtonRight = 12;

    unsigned char done = 0;
    bool nightMode = false;
    bool debug = false;
    void updateConfigFromSerial();
    void sendConfig();
    void setPlunger();
    void setAccelerometer();
    bool lowLatencyMode = false;

    unsigned char lightShowState = 1;

    unsigned char buttonKeyboard[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char buttonRemap[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    unsigned char buttonKeyDebounce[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool lastButtonState[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool processedButtonState[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char buttonDebounceCounter = 0;
    bool plungerMoving = false; 
    long restingStateMax = 160;
    bool updateUSB = false;
    bool disableUSBSuspend = true;
    bool buttonPressed = false;
    // 0 is ignore accel option
    // 1 is quick release option
    // 2 ignore when not in use option
    // 3 
    bool disableAccelOnPlungerMove = true;
    unsigned char  enablePlungerQuickRelease = true;
    bool disablePlungerWhenNotInUse = true;
    bool disableButtonPressWhenKeyboardEnabled = true;
    bool enablePlunger = true;
    bool bluetoothEnable = false;

    unsigned char tiltSuppress = 10;
    bool lightShowAttractEnabled = true;
    unsigned char lightShowTime = 10;
    bool reverseButtonOutputPolarity = true;
    


  private:
    unsigned char Config::blockRead();
    void writeIntIntoEEPROM(uint16_t address, int16_t number);
    int16_t readIntFromEEPROM(uint16_t address);
    void printError();
    void printComma(unsigned char value);
    void printIntComma(int value);
    void printSuccess();
    int16_t readIntFromByte();
    void readConfigArray(unsigned char* configArray, unsigned char size);
    void printConfigArray(unsigned char* configArray, unsigned char size);
    void loadEEPROMArray(unsigned char* configArray, uint16_t address, uint8_t size);
    void saveEEPROMArray(unsigned char* configArray, uint16_t address, uint8_t size);
    
    
};

#endif
