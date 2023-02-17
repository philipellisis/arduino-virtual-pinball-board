#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <Arduino.h>
#include <Joystick.h>


class Communication {
  
  public:
    Communication();
    void init(Joystick_* joystick);
    void communicate();
    
  private:
    int firstNumber = 32; //0 normally
    int bankOffset = 100; //200 normally
    int adminNumber = 126; //255 normally
    int maxNumber = 125; //255 normally
    byte firstNumberByte = 32;
    float scaleFactor = 255/float(maxNumber-firstNumber);
    Joystick_* _joystick;
    byte incomingData[10];
    int dataLocation = 0;
    void updateOutputs();
};

#endif
