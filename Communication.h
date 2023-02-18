#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <Arduino.h>
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"


class Communication {
  
  public:
    Communication();
    void init(Plunger* plunger, Accelerometer* accel, Buttons* buttons);
    //void init();
    void communicate();
    
  private:
    int firstNumber = 32; //0 normally
    int bankOffset = 100; //200 normally
    int adminNumber = 126; //255 normally
    int maxNumber = 125; //255 normally
    byte firstNumberByte = 32;
    float scaleFactor = 255/float(maxNumber-firstNumber);
    Accelerometer* _accelerometer;
    Plunger* _plunger;
    Buttons* _buttons;
    byte incomingData[10];
    int dataLocation = 0;
    void updateOutputs();
    int admin = 0;
};

#endif
