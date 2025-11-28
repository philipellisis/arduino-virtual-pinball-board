#ifndef PLUNGER_H
#define PLUNGER_H
#include <Arduino.h>
#include "MinimalHID.h"
#include "Config.h"

class Plunger {
  
  public:
    Plunger();
    void init();
    void plungerRead();
    void sendPlungerState();
    void resetPlunger();
    int16_t getAdjustedValue() const { return adjustedValue; }
    
  private:
    bool plungerReleased = false;
    float plungerScaleFactor;
    int16_t adjustedValue;
    uint8_t buttonState;
    uint8_t buttonState2;
    int16_t priorValue = 0;
    int16_t truePriorValue = 0;
    uint32_t restingStartTime;
    uint32_t priorTime = 0;

    int16_t plungerData[35] = {0};
    uint8_t plungerDataTime[35] = {0};
    int8_t plungerDataCounter = 0;
    int8_t getDelayedPlungerValue(int8_t sensorValue, uint32_t currentTime);
    int8_t currentPlungerMax = 0;
    void updateButtonState(uint8_t& buttonState, bool condition, bool pressed);
    void updateGamepadZAxis(int8_t value, bool forceUpdate = false);

};

#endif
