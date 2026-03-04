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
    void sendBleConfigPacket(const uint8_t* map);

private:
    static const uint8_t SS_PIN = 21;
    static const uint8_t NUM_BUTTONS = 32;
    static const uint8_t BUTTON_BYTES = 32;  // 1 byte per button (keycode or 255/0)
    static const uint8_t PACKET_SIZE = 40;   // 32 buttons + 6 analog + 1 mode + 1 checksum

    // Heartbeat: ensures we send at least one SPI packet every 100 ms.
    uint32_t _lastHeartbeat = 0;

    // Last known states for change detection
    bool    lastButtonStates[NUM_BUTTONS];
    int16_t lastXAxis;
    int16_t lastYAxis;
    int16_t lastPlungerValue;

    void    packButtonData(uint8_t* buttonPacket);
    int16_t getXAxisValue();
    int16_t getYAxisValue();
    int16_t getPlungerValue();
    void    sendSPIPacket();
};

#endif
