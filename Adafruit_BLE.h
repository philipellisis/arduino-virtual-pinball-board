/**************************************************************************/
/*!
    @file     Adafruit_BLE.h
    @author   Minimal implementation for BluetoothController

    Simplified base class that only includes what's needed for the 
    BluetoothController functionality to save space.
*/
/**************************************************************************/

#ifndef _ADAFRUIT_BLE_H_
#define _ADAFRUIT_BLE_H_

#include <Arduino.h>

// Bluefruit LE transport modes
#define BLUEFRUIT_TRANSPORT_HWSPI           1
#define BLUEFRUIT_TRANSPORT_SWSPI           2

// Bluefruit LE operational modes  
#define BLUEFRUIT_MODE_COMMAND              0
#define BLUEFRUIT_MODE_DATA                 1

// SDEP Message Types
#define SDEP_MSGTYPE_COMMAND                0x10
#define SDEP_MSGTYPE_RESPONSE               0x20
#define SDEP_MSGTYPE_ERROR                  0x80

// SDEP Command Types
#define SDEP_CMDTYPE_INITIALIZE             0xBEEF
#define SDEP_CMDTYPE_AT_WRAPPER             0x0A00
#define SDEP_CMDTYPE_BLE_UARTTX             0x0A01
#define SDEP_CMDTYPE_BLE_UARTRX             0x0A02

// Buffer constants
#define SDEP_MAX_PACKETSIZE                 16
#define BLE_BUFSIZE                         64

// Timeout class for SPI operations
class TimeoutTimer {
  public:
    TimeoutTimer(uint32_t ms) : _timeout(ms), _start(millis()) {}
    void set(uint32_t ms) { _timeout = ms; _start = millis(); }
    bool expired() { return (millis() - _start) >= _timeout; }
  private:
    uint32_t _timeout;
    uint32_t _start;
};

// SDEP packet structures
typedef struct {
  uint8_t  msg_type;
  uint8_t  cmd_id_high;
  uint8_t  cmd_id_low;
  uint8_t  length;
  uint8_t  more_data;
} sdepMsgHeader_t;

typedef struct {
  sdepMsgHeader_t header;
  uint8_t         payload[SDEP_MAX_PACKETSIZE];
} sdepMsgCommand_t;

typedef struct {
  sdepMsgHeader_t header;
  uint8_t         payload[SDEP_MAX_PACKETSIZE];
} sdepMsgResponse_t;

// Minimal base class - only includes what BluetoothController needs
class Adafruit_BLE {
  public:
    Adafruit_BLE() : _mode(BLUEFRUIT_MODE_COMMAND), _verbose(false), _timeout(1000) {}
    
    // Core methods used by BluetoothController
    virtual bool begin(bool verbose = false, bool blocking = true) = 0;
    virtual bool sendCommandCheckOK(const char* cmd);
    virtual bool sendCommandWithIntReply(const char* cmd, int32_t* reply);
    virtual bool factoryReset();
    virtual bool echo(bool enable);
    virtual bool reset();
    virtual bool isConnected();
    virtual bool setMode(uint8_t mode) = 0;

  protected:
    uint8_t  _mode;
    uint8_t  _physical_transport;
    bool     _verbose;
    uint32_t _timeout;
    uint32_t _reset_started_timestamp;
};

#endif