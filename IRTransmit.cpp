#include "IRTransmit.h"
#include "Config.h"
#include "Globals.h"   // assumes a global 'config' exists

IRTransmit::IRTransmit() {}

void IRTransmit::sendCommand(uint8_t outputPin) {
  if (config.irProtocol == IR_PROTOCOL_NONE) return;

  if (!_initialized || _lastPin != outputPin) {
    pinMode(outputPin, OUTPUT);
    digitalWrite(outputPin, LOW);
    _initialized = true;
    _lastPin = outputPin;
  }

  // Idle low before starting
  digitalWrite(outputPin, LOW);

  switch (config.irProtocol) {
    case IR_PROTOCOL_NEC: {
      // config.irCode is expected as AA ~AA CC ~CC (e.g., 0x40BF12ED)
      sendNEC(config.irCode, outputPin);
      delay(40);
      necSendRepeat(outputPin);
      delay(40);
      necSendRepeat(outputPin);
      break;
    }

    case IR_PROTOCOL_SAMSUNG: {
      // Many Samsung sets accept full-frame repeats
      sendSamsung(config.irCode, outputPin);
      delay(40);
      sendSamsung(config.irCode, outputPin);
      delay(40);
      sendSamsung(config.irCode, outputPin);
      break;
    }

    case IR_PROTOCOL_SONY: {
      // config.irBits typically 12/15/20
      sendSony(config.irCode, config.irBits, outputPin);
      delay(45);
      sendSony(config.irCode, config.irBits, outputPin);
      break;
    }

    default:
      break;
  }

  Serial.print(F("DEBUG,IR transmission complete\r\n"));
  Serial.flush();
}

// ===== NEC =====
// wordAAiiCCjj: 32-bit word with bytes [A, ~A, C, ~C]; each is sent LSB-first on-air.
void IRTransmit::sendNEC(uint32_t word, uint8_t pin) {
  // NEC format in 32-bit word (LSB to MSB): b0, b1, b2, b3
  // Standard NEC: addr, ~addr, cmd, ~cmd
  // Extended NEC: addrLo, addrHi, cmd, ~cmd
  uint8_t b0 = (word      ) & 0xFF;
  uint8_t b1 = (word >>  8) & 0xFF;
  uint8_t b2 = (word >> 16) & 0xFF;
  uint8_t b3 = (word >> 24) & 0xFF;

  // Check if command byte and its inverse are valid (b2 and b3)
  bool cmdValid = ((uint8_t)~b2 == b3);
  // Check if address bytes follow standard NEC format (addr, ~addr)
  bool addrStandard = ((uint8_t)~b0 == b1);

  if (cmdValid && addrStandard) {
    // Standard NEC: addr = b0, cmd = b2
    necSendFrame(b0, b2, pin);
  } else if (cmdValid) {
    // Extended NEC: 16-bit address (b0 = addrLo, b1 = addrHi), cmd = b2
    uint16_t addr16 = ((uint16_t)b1 << 8) | b0;
    necSendFrameExtended(addr16, b2, pin);
  } else {
    // Fallback: send as standard with what we have
    necSendFrame(b0, b2, pin);
  }
}

void IRTransmit::necSendFrame(uint8_t addr, uint8_t cmd, uint8_t pin) {
  // Leader
  mark(9000, pin); space(4500, pin);

  // Byte order: Address, ~Address, Command, ~Command (each LSB-first)
  necSendByteLSB(addr, pin);
  necSendByteLSB((uint8_t)~addr, pin);
  necSendByteLSB(cmd,  pin);
  necSendByteLSB((uint8_t)~cmd,  pin);

  // Stop
  mark(560, pin);
  space(0, pin);
}

void IRTransmit::necSendFrameExtended(uint16_t addr16, uint8_t cmd, uint8_t pin) {
  // Leader
  mark(9000, pin); space(4500, pin);

  // Send low byte of address, then high byte (each LSB-first)
  necSendByteLSB((uint8_t)(addr16 & 0xFF), pin);
  necSendByteLSB((uint8_t)(addr16 >> 8),   pin);

  // Command and its inverse
  necSendByteLSB(cmd, pin);
  necSendByteLSB((uint8_t)~cmd, pin);

  // Stop
  mark(560, pin); space(0, pin);
}

void IRTransmit::necSendRepeat(uint8_t pin) {
  // NEC repeat: 9ms mark, 2.25ms space, 560us mark
  mark(9000, pin);
  space(2250, pin);
  mark(560, pin);
  space(0, pin);
}

void IRTransmit::necSendByteLSB(uint8_t b, uint8_t pin) {
  for (uint8_t i = 0; i < 8; i++) {
    mark(560, pin);
    space((b & 0x01) ? 1690 : 560, pin);
    b >>= 1;
  }
}

// ===== Samsung (NEC-like) =====
void IRTransmit::sendSamsung(uint32_t wordAAiiCCjj, uint8_t pin) {
  // Leader (Samsung uses 4.5ms/4.5ms)
  mark(4500, pin); space(4500, pin);

  // 32 bits, LSB-first (byte order on-air AA, ~AA, CC, ~CC)
  uint32_t data = wordAAiiCCjj;
  for (uint8_t i = 0; i < 32; i++) {
    mark(560, pin);
    space((data & 0x1) ? 1690 : 560, pin);
    data >>= 1;
  }

  mark(560, pin);
  space(0, pin);
}

// ===== Sony SIRC =====
void IRTransmit::sendSony(uint32_t data, uint8_t bits, uint8_t pin) {
  // Leader
  mark(2400, pin);
  space(600, pin);

  // LSB-first
  for (uint8_t i = 0; i < bits; i++) {
    mark(600, pin);
    space((data & 0x1) ? 1200 : 600, pin);
    data >>= 1;
  }

  space(0, pin);
}

// ===== Carrier helpers (38 kHz via bit-banging) =====
void IRTransmit::mark(uint16_t usec, uint8_t pin) {
  // 38kHz = 26.3µs period, ~33% duty cycle
  // Calculate number of carrier cycles needed
  uint16_t cycles = (usec * 38UL) / 1000UL;

  // Get port and bitmask for fast direct port access
  volatile uint8_t *outPort = portOutputRegister(digitalPinToPort(pin));
  uint8_t pinMask = digitalPinToBitMask(pin);

  // Disable interrupts for precise timing
  noInterrupts();

  for (uint16_t i = 0; i < cycles; i++) {
    *outPort |= pinMask;   // HIGH
    delayMicroseconds(9);
    *outPort &= ~pinMask;  // LOW
    delayMicroseconds(17);
  }

  interrupts();
}

void IRTransmit::space(uint16_t usec, uint8_t pin) {
  digitalWrite(pin, LOW);
  if (usec > 0) delayMicroseconds(usec);
}
