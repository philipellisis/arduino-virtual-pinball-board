#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "Config.h"
#include "Outputs.h"


class Buttons {
  
  public:
    Buttons();
    void readInputs();
    void init();
    void sendButtonState();
    bool sendButtonPush(unsigned char i, bool currentButtonState);
    
    
  private:
    unsigned char buttonOffset = 0;
    bool DEBUG = false;
    bool lastButtonState[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned char numberButtonsPressed = 0;
};

#endif
