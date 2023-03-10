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
    void init(Plunger* plunger, Accelerometer* accel, Buttons* buttons, Config* config, Outputs* outputs);
    //void init();
    void communicate();
    
  private:
    bool DEBUG = true;
    byte firstNumber = 32; //0 normally
    byte bankOffset = 100; //200 normally
    byte adminNumber = 126; //255 normally
    byte connectionNumber = 127;
    byte maxNumber = 125; //255 normally
    byte firstNumberByte = 32;
    float scaleFactor = 255/float(maxNumber-firstNumber);
    Accelerometer* _accelerometer;
    Plunger* _plunger;
    Buttons* _buttons;
    Config* _config;
    Outputs* _outputs;
    byte incomingData[10];
    int dataLocation = 0;
    void updateOutputs();
    int admin = 0;
    void sendAdmin();
};

#endif
