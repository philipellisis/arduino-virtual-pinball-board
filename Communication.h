#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <Arduino.h>
#include "Plunger.h"
#include "HID-Project.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Config.h"
#include "Outputs.h"


class Communication {
  
  public:
    Communication();
    void communicate();
    
  private:
    char* connectedString = "DEBUG,CSD Board Connected\r\n";
    uint8_t firstNumber = 0; //0 normally
    uint8_t bankOffset = 200; //200 normally
    uint8_t adminNumber = 250; //250 normally
    uint8_t connectionNumber = 251;//251 normally
    uint8_t outputSingleNumber = 252;//252 normally
    uint8_t outputButtonNumber = 253;//252 normally
    uint8_t maxNumber = 255; //255 normally
    uint8_t delayIncrementor = 0;

    uint8_t incomingData[10];
    uint8_t dataLocation = 0;
    void updateOutputs();
    uint8_t admin = 0;
    void sendAdmin();
    bool shouldDelay();
    void handleDelayedAdmin(uint8_t adminType);

    uint8_t previousDOFValues[63] = {0};
};

#endif
