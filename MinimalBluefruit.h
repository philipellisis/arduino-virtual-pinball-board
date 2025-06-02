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
    uint8_t m_response_buffer[20];
    
    // SDEP Protocol constants
    static const uint8_t SDEP_MSGTYPE_COMMAND = 0x10;
    static const uint8_t SDEP_MSGTYPE_RESPONSE = 0x20;
    static const uint8_t SDEP_MSGTYPE_ERROR = 0x80;
    static const uint16_t SDEP_CMDTYPE_AT_WRAPPER = 0x0A00;
    static const uint8_t SDEP_MAX_PACKETSIZE = 16;
    static const uint8_t SPI_IGNORED_BYTE = 0xFE;
    static const uint8_t SPI_OVERREAD_BYTE = 0xFF;
    static const uint16_t SPI_DEFAULT_DELAY_US = 10;
    
    // SDEP packet structure
    struct __attribute__((packed)) sdep_header_t {
        uint8_t msg_type;
        uint8_t cmd_id_low;
        uint8_t cmd_id_high;
        struct __attribute__((packed)) {
            uint8_t length : 7;
            uint8_t more_data : 1;
        };
    };
    
    struct __attribute__((packed)) sdep_packet_t {
        sdep_header_t header;
        uint8_t payload[SDEP_MAX_PACKETSIZE];
    };
    
    // SPI communication
    bool sendATCommand(const char* cmd);
    bool waitForOK();
    void spiInit();
    void spiEnd();
    uint8_t spiTransfer(uint8_t data);
    bool sendSDEPPacket(uint16_t cmd_id, const uint8_t* buf, uint8_t len, uint8_t more_data = 0);
    bool getSDEPPacket(void* packet_ptr);
    
    // SDEP protocol helpers
    bool sendSDEPPacket(uint16_t cmd_id, const uint8_t* buf, uint8_t len, uint8_t more_data);
    bool getSDEPPacket(void* packet);
    uint8_t spiTransfer(uint8_t data);
};

#endif