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
    Joystick_* _joystick;
    byte incomingData[10];
    int dataLocation = 0;
    void updateOutputs();
};

#endif
