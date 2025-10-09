#ifndef IRTRANSMIT_H
#define IRTRANSMIT_H
#include <Arduino.h>

// Maximum size for IR command data (in microseconds timing pairs)
// Each pair is: mark duration, space duration
// NEC protocol uses ~68 pairs, supporting up to 90 pairs
// 1 command: 1 byte count + 90 pairs × 4 bytes = 361 bytes
#define MAX_IR_PAIRS 90

class IRTransmit {

  public:
    IRTransmit();
    void init(unsigned char outputPin);
    void sendCommand();
    void setOutputPin(unsigned char pin);

  private:
    unsigned char irOutputPin;
    void mark(uint16_t duration);
    void space(uint16_t duration);
};

#endif
