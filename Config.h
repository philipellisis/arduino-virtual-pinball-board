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

    // Bit-packed flags to save memory (14 bools = 2 bytes instead of 14 bytes)
    struct {
      uint8_t nightMode : 1;
      uint8_t debug : 1;
      uint8_t lowLatencyMode : 1;
      uint8_t plungerMoving : 1;
      uint8_t updateUSB : 1;
      uint8_t disableUSBSuspend : 1;
      uint8_t buttonPressed : 1;
      uint8_t bluetoothEnable : 1;

      uint8_t disableAccelOnPlungerMove : 1;
      uint8_t disablePlungerWhenNotInUse : 1;
      uint8_t disableButtonPressWhenKeyboardEnabled : 1;
      uint8_t enablePlunger : 1;
      uint8_t lightShowAttractEnabled : 1;
      uint8_t reverseButtonOutputPolarity : 1;
      uint8_t _reserved : 2;  // Reserved for future use
    } flags;

    // Bit-packed button state arrays (32 bools = 4 bytes instead of 32 bytes each)
    uint32_t lastButtonStatePacked = 0;
    uint32_t processedButtonStatePacked = 0;

    void updateConfigFromSerial();
    void sendConfig();
    void setPlunger();
    void setAccelerometer();

    unsigned char lightShowState = 1;

    unsigned char buttonKeyboard[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char buttonRemap[32] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    unsigned char buttonKeyDebounce[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char buttonDebounceCounter = 0;
    long restingStateMax = 160;
    // 0 is ignore accel option
    // 1 is quick release option
    // 2 ignore when not in use option
    // 3
    unsigned char  enablePlungerQuickRelease = true;

    unsigned char tiltSuppress = 10;
    unsigned char lightShowTime = 10;

    // Compact bit access helpers
    bool lastButtonState(uint8_t i) const { return lastButtonStatePacked & (1UL << i); }
    void setLastButtonState(uint8_t i, bool v) {
      v ? (lastButtonStatePacked |= (1UL << i)) : (lastButtonStatePacked &= ~(1UL << i));
    }
    bool processedButtonState(uint8_t i) const { return processedButtonStatePacked & (1UL << i); }
    void setProcessedButtonState(uint8_t i, bool v) {
      v ? (processedButtonStatePacked |= (1UL << i)) : (processedButtonStatePacked &= ~(1UL << i));
    }

  private:
    unsigned char Config::blockRead();
    void writeIntIntoEEPROM(uint16_t address, int16_t number);
    int16_t readIntFromEEPROM(uint16_t address);
    inline void printError();
    inline void printComma(unsigned char value);
    inline void printIntComma(int value);
    inline void printSuccess();
    int16_t readIntFromByte();
    void readConfigArray(unsigned char* configArray, unsigned char size);
    void printConfigArray(unsigned char* configArray, unsigned char size);
    void loadEEPROMArray(unsigned char* configArray, uint16_t address, uint8_t size);
    void saveEEPROMArray(unsigned char* configArray, uint16_t address, uint8_t size);
};

// Lightweight bit macros (only for less-frequently accessed data)
#define GET_BIT(var, i) ((var >> i) & 1)
#define SET_BIT(var, i, v) do { if (v) var |= (1UL << i); else var &= ~(1UL << i); } while(0)

#endif
