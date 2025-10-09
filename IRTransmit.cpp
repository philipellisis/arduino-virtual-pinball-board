#include "IRTransmit.h"
#include "Config.h"
#include "Globals.h"
#include <EEPROM.h>

IRTransmit::IRTransmit() {
  irOutputPin = 255;
}

void IRTransmit::init(unsigned char outputPin) {
  irOutputPin = outputPin;
  if (irOutputPin != 255) {
    pinMode(irOutputPin, OUTPUT);
    digitalWrite(irOutputPin, LOW);
  }
}

void IRTransmit::setOutputPin(unsigned char pin) {
  irOutputPin = pin;
  if (irOutputPin != 255) {
    pinMode(irOutputPin, OUTPUT);
    digitalWrite(irOutputPin, LOW);
  }
}

void IRTransmit::sendCommand() {
  if (irOutputPin > 62) {
    return;
  }

  // Read number of pairs directly from EEPROM
  // IR command data starts at EEPROM address 610
  // Format: [0] = numPairs, [1-4] = mark0 high/low, space0 high/low, etc.
  unsigned char numPairs = EEPROM.read(610);

  if (numPairs == 0 || numPairs > MAX_IR_PAIRS) {
    return;
  }

  // Disable interrupts for precise timing
  noInterrupts();

  // Send the IR sequence, reading directly from EEPROM
  for (unsigned char i = 0; i < numPairs; i++) {
    // Each pair uses 4 bytes: mark_high, mark_low, space_high, space_low
    uint16_t eepromOffset = 611 + (i * 4);
    uint16_t markTime = (EEPROM.read(eepromOffset) << 8) |
                        EEPROM.read(eepromOffset + 1);
    uint16_t spaceTime = (EEPROM.read(eepromOffset + 2) << 8) |
                         EEPROM.read(eepromOffset + 3);

    if (markTime > 0) {
      mark(markTime);
    }
    if (spaceTime > 0) {
      space(spaceTime);
    }
  }

  // Ensure LED is off
  digitalWrite(irOutputPin, LOW);

  // Re-enable interrupts
  interrupts();
}

void IRTransmit::mark(uint16_t duration) {
  // 38kHz carrier frequency (26.3 microseconds period)
  // 13.1 microseconds high, 13.1 microseconds low
  uint32_t endTime = micros() + duration;

  while (micros() < endTime) {
    digitalWrite(irOutputPin, HIGH);
    delayMicroseconds(13);
    digitalWrite(irOutputPin, LOW);
    delayMicroseconds(13);
  }
}

void IRTransmit::space(uint16_t duration) {
  digitalWrite(irOutputPin, LOW);
  delayMicroseconds(duration);
}
