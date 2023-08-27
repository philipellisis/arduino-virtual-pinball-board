#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <Arduino.h>
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Config.h"
#include "Outputs.h"


class Communication {
  
  public:
    Communication();
    void init(Plunger* plunger, Accelerometer* accel, Buttons* buttons, Config* config, Outputs* outputs, Joystick_* joystick);
    //void init();
    void communicate();
    
  private:
    bool DEBUG = false;
    char* connectedString = "DEBUG,CSD Board Connected\r\n";
    byte firstNumber = 0; //0 normally
    byte bankOffset = 200; //200 normally
    byte adminNumber = 250; //250 normally
    byte connectionNumber = 251;//251 normally
    byte outputSingleNumber = 252;//252 normally
    byte outputButtonNumber = 253;//252 normally
    byte maxNumber = 255; //255 normally
    float scaleFactor = 255/float(maxNumber-firstNumber);
    Accelerometer* _accelerometer;
    Plunger* _plunger;
    Buttons* _buttons;
    Config* _config;
    Outputs* _outputs;
    Joystick_* _joystick;
    byte incomingData[10];
    int dataLocation = 0;
    void updateOutputs();
    int admin = 0;
    void sendAdmin();
};

#endif
