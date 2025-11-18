#ifndef IRTRANSMIT_H
#define IRTRANSMIT_H
#include <Arduino.h>

enum IR_PROTOCOL : uint8_t {
  IR_PROTOCOL_NONE = 0,
  IR_PROTOCOL_NEC,
  IR_PROTOCOL_SAMSUNG,
  IR_PROTOCOL_SONY
};

class IRTransmit {
public:
  IRTransmit();

  // Uses global 'config' (protocol, code, bits) and the provided output pin.
  void sendCommand(uint8_t outputPin);

private:
  // Protocol implementations
  void sendNEC(uint32_t wordAAiiCCjj, uint8_t pin);
  void sendSamsung(uint32_t wordAAiiCCjj, uint8_t pin);
  void sendSony(uint32_t data, uint8_t bits, uint8_t pin);

  // NEC helpers
  void necSendByteLSB(uint8_t b, uint8_t pin);
  void necSendFrame(uint8_t addr, uint8_t cmd, uint8_t pin);
  void necSendFrameExtended(uint16_t addr16, uint8_t cmd, uint8_t pin);
  void necSendRepeat(uint8_t pin);

  // Carrier helpers
  void mark(uint16_t usec, uint8_t pin);
  void space(uint16_t usec, uint8_t pin);

  // Lazy init cache
  bool    _initialized = false;
  uint8_t _lastPin     = 0xFF;
};

#endif
