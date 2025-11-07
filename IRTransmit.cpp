#include "IRTransmit.h"
#include "Config.h"
#include "Globals.h"

IRTransmit::IRTransmit() {
}

void IRTransmit::sendCommand(unsigned char outputPin) {

  if (config.irProtocol == IR_PROTOCOL_NONE) {
    return;
  }

  // Ensure pin starts LOW before transmission
  digitalWrite(outputPin, LOW);

  // Send the command 3 times for better reception
  for (unsigned char repeat = 0; repeat < 10; repeat++) {
    switch (config.irProtocol) {
      case IR_PROTOCOL_NEC:
        sendNEC(config.irCode, outputPin);
        break;
      case IR_PROTOCOL_SAMSUNG:
        sendSamsung(config.irCode, outputPin);
        break;
      case IR_PROTOCOL_SONY:
        sendSony(config.irCode, config.irBits, outputPin);
        break;
    }

    // Delay between repetitions (40ms is typical for IR protocols)
    if (repeat < 9) {
      delay(40);
    }
  }

  Serial.print(F("DEBUG,IR transmission complete\r\n"));
  Serial.flush();
}

void IRTransmit::sendNEC(uint32_t data, unsigned char pin) {
  // NEC Protocol format:
  // 9ms AGC burst, 4.5ms space, 32 bits (address + command), stop bit

  // AGC burst
  mark(9000, pin);
  space(4500, pin);

  // Send 32 bits (LSB first)
  for (unsigned char i = 0; i < 32; i++) {
    mark(560, pin);
    if (data & 0x00000001) {
      space(1690, pin); // Logical '1'
    } else {
      space(560, pin);  // Logical '0'
    }
    data >>= 1;
  }

  // Stop bit
  mark(560, pin);
  space(0, pin);
}

void IRTransmit::sendSamsung(uint32_t data, unsigned char pin) {
  // Samsung Protocol format (similar to NEC with different timing)
  // 4.5ms AGC burst, 4.5ms space, 32 bits, stop bit

  // AGC burst
  mark(4500, pin);
  space(4500, pin);

  // Send 32 bits (LSB first)
  for (unsigned char i = 0; i < 32; i++) {
    mark(560, pin);
    if (data & 0x00000001) {
      space(1690, pin); // Logical '1'
    } else {
      space(560, pin);  // Logical '0'
    }
    data >>= 1;
  }

  // Stop bit
  mark(560, pin);
  space(0, pin);
}

void IRTransmit::sendSony(uint32_t data, unsigned char bits, unsigned char pin) {
  // Sony SIRC Protocol format:
  // 2.4ms AGC burst, then bits (usually 12, 15, or 20 bits)
  // Bit: 600us burst + 600us space (0) or 1200us space (1)

  // AGC burst
  mark(2400, pin);
  space(600, pin);

  // Send bits (LSB first)
  for (unsigned char i = 0; i < bits; i++) {
    mark(600, pin);
    if (data & 0x00000001) {
      space(1200, pin); // Logical '1'
    } else {
      space(600, pin);  // Logical '0'
    }
    data >>= 1;
  }
}

// 38 kHz using tone(): start tone for 'duration', then stop
void IRTransmit::mark(uint16_t duration, unsigned char pin) {
  tone(pin, 38000);                // 38 kHz carrier
  delayMicroseconds(duration);
  noTone(pin);
  // Ensure line is low between bursts
  digitalWrite(pin, LOW);
}

void IRTransmit::space(uint16_t duration, unsigned char pin) {
  // No carrier; line held LOW
  digitalWrite(pin, LOW);
  if (duration > 0) delayMicroseconds(duration);
}