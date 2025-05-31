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
    unsigned char firstNumber = 0; //0 normally
    unsigned char bankOffset = 200; //200 normally
    unsigned char adminNumber = 250; //250 normally
    unsigned char connectionNumber = 251;//251 normally
    unsigned char outputSingleNumber = 252;//252 normally
    unsigned char outputButtonNumber = 253;//252 normally
    unsigned char maxNumber = 255; //255 normally
    unsigned char delayIncrementor = 0;

    unsigned char incomingData[10];
    int dataLocation = 0;
    void updateOutputs();
    int admin = 0;
    void sendAdmin();
    bool shouldDelay();

    unsigned char previousDOFValues[63] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
};

#endif
