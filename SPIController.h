#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include "Config.h"
#include "Outputs.h"

class SPIController {
public:
    SPIController();
    void init();
    void update();
    bool hasInputChanged();
    void sendBleConfigPacket(const uint8_t* map);

    // Returns true when a new back-channel output command has been received
    // from the ESP32 via SPI MISO.
    bool hasOutputPacket() const;

    // Applies the received output command directly to the Outputs object and
    // clears the pending flag.  Call only after hasOutputPacket() returns true.
    // Handles both bank updates (7 outputs at once) and single-output updates,
    // mirroring what Communication::updateOutputs() does for DOF serial data.
    void applyOutputPacket(Outputs& outputs);

private:
    static const uint8_t SS_PIN = 21;
    static const uint8_t NUM_BUTTONS = 32;
    static const uint8_t BUTTON_BYTES = 32;  // 1 byte per button (keycode or 255/0)
    static const uint8_t PACKET_SIZE = 40;   // 32 buttons + 6 analog + 1 mode + 1 checksum

    // SPI back-channel: output command received from ESP32 via MISO.
    // _bcType: 0x4F = multi-bank, 0x53 = single output
    // _bcArg:  slot count (1-4) for multi-bank; output ID for single
    // _bcRaw:  packed slot data for multi-bank (up to 4×8=32 bytes);
    //          [0]=value for single-output
    uint8_t  _bcType     = 0;
    uint8_t  _bcArg      = 0;
    uint8_t  _bcRaw[32]  = {};
    bool     _bcPending  = false;

    // Heartbeat: ensures we send at least one SPI packet every 100 ms so
    // rumble commands arrive even when no inputs are changing.
    uint32_t _lastHeartbeat = 0;
    
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
    void parseBackchannelPacket(const uint8_t* rxBuf);
};

#endif