#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "Config.h"
#include "Outputs.h"


class Buttons {
  
  public:
    Buttons();
    void readInputs();
    bool checkChanged();
    unsigned char numberButtonsPressed = 0;
    void init();
    void sendButtonState();
    bool sendButtonPush(unsigned char i, bool currentButtonState);
    
    
  private:
    unsigned char buttonOffset = 0;
    void sendActualButtonPress(unsigned char buttonOffset, bool currentButtonState);
    void handleMediaKey(unsigned char keyCode, bool pressed);
    void processKeyboardAction(unsigned char keyCode, bool pressed);
};

#endif
