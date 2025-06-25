#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include "Config.h"

class SPIController {
public:
    SPIController();
    void init();
    void update();
    bool hasInputChanged();
    
private:
    static const uint8_t SS_PIN = 21;
    static const uint8_t NUM_BUTTONS = 32;
    static const uint8_t BUTTON_BYTES = (NUM_BUTTONS + 7) / 8;  // 4 bytes for 32 buttons
    static const uint8_t PACKET_SIZE = BUTTON_BYTES + 13;  // 4 bytes buttons + 6 bytes analog (2x each: plunger, X, Y) + 1 byte button count + 2 bytes validation
    
    // Last known states for change detection
    bool lastButtonStates[NUM_BUTTONS];
    int16_t lastXAxis;
    int16_t lastYAxis;
    int16_t lastPlungerValue;
    
    // Helper methods
    void packButtonData(uint8_t* buttonPacket);
    int16_t getXAxisValue();
    int16_t getYAxisValue();
    int16_t getPlungerValue();
    void sendSPIPacket();
};

#endif