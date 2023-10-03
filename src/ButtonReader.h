#ifndef ShiftIn_h
#define ShiftIn_h

#include "Arduino.h"

class ButtonReader {
private:

  uint32_t lastState = 0;
  uint32_t currentState = 0;
public:
  ButtonReader() {}
  
  // setup all pins
  void init() {
    pinMode(4, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(0, INPUT);
    pinMode(1, OUTPUT);
  }
  
  inline boolean state(int id) { return bitRead(currentState, id); }
  
  uint32_t read() {
    lastState = currentState;
    uint32_t result = 0;

    digitalWrite(1, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(1, LOW);

    for(uint16_t i = 0; i < 24; i++) {
      uint32_t value = digitalRead(0);
      result |= (value << ((24-1) - i));
      digitalWrite(1, HIGH);
      digitalWrite(1, LOW);
    }
    currentState = result;
    return result;
  }
  
  boolean update() {
    return read() != lastState;
  }
};

#endif