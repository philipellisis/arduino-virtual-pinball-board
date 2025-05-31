#ifndef MINIMAL_BLUEFRUIT_H
#define MINIMAL_BLUEFRUIT_H

#include <Arduino.h>
#include <SPI.h>

// Minimal Bluefruit LE implementation for gamepad functionality only
class MinimalBluefruit {
public:
    MinimalBluefruit(int8_t csPin, int8_t irqPin, int8_t rstPin);
    
    // Only the methods actually used by BluetoothController
    bool begin(bool verbose = false);
    bool factoryReset();
    bool echo(bool enable);
    bool sendCommandCheckOK(const char* cmd);
    bool sendCommandWithIntReply(const char* cmd, int32_t* reply);
    bool reset();
    bool isConnected();
    bool setMode(uint8_t mode);
    
private:
    int8_t m_cs_pin;
    int8_t m_irq_pin; 
    int8_t m_rst_pin;
    bool m_connected;
    bool m_verbose;
    uint32_t m_reset_time;
    
    // Minimal SPI communication
    bool sendATCommand(const char* cmd);
    bool waitForOK();
    void spiInit();
    void spiEnd();
};

#endif