#ifndef IRTRANSMIT_H
#define IRTRANSMIT_H
#include <Arduino.h>

// IR Protocol definitions
#define IR_PROTOCOL_NONE 0
#define IR_PROTOCOL_NEC 1
#define IR_PROTOCOL_SAMSUNG 2
#define IR_PROTOCOL_SONY 3

// Carrier frequency 38kHz (26.3 microseconds period)
#define IR_CARRIER_FREQ 38000

class IRTransmit {

  public:
    IRTransmit();
    void sendCommand(unsigned char outputPin);

  private:
    void mark(uint16_t duration, unsigned char pin);
    void space(uint16_t duration, unsigned char pin);
    void sendNEC(uint32_t data, unsigned char pin);
    void sendSamsung(uint32_t data, unsigned char pin);
    void sendSony(uint32_t data, unsigned char bits, unsigned char pin);
};

#endif
