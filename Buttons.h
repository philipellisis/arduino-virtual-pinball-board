#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "Config.h"
#include "Outputs.h"


class Buttons {
  
  public:
    Buttons();
    void readInputs();
    void init(Config* config, Outputs* outputs);
    void sendButtonState();
    
    
  private:
    byte buttonOffset = 0;
    bool DEBUG = false;
    bool lastButtonState[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    Config* _config;
    Outputs* _outputs;
};

#endif
