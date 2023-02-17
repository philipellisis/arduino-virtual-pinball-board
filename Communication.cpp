#include "Communication.h"
#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>
#include "Outputs.h"


Outputs outputs = Outputs();
Communication::Communication() {
  Serial.println("Communication: About to initialize serial");

  Serial.println("Communication: pins initialized");
}

void Communication::init(Joystick_* joystick) {
  Serial.println("Communication: initializing Communication");
  _joystick = joystick;
  outputs.init();
  Serial.println("Communication: initialized Communication");
}

void Communication::communicate() {
  if (Serial.available() > 0) {
    incomingData[dataLocation] = Serial.read();
    Serial.println("getting serial data: " + String(incomingData[dataLocation]));
    // data sent is always [0][200 + bank offset][output value 1][output value 2][output value 3][output value 4][output value 5][output value 6][output value 7]
    // check to make sure we are reading what's expected for the first 2 bytes of data. If not, reset and start the buffer over again
    // sample data coming in: 0È0000000
    // sample data coming in for test: " d       "
    // sample data coming in for test all on: " d}}}}}}}"
    if ((dataLocation == 0 && incomingData[dataLocation] != firstNumber) || (dataLocation == 1 && (incomingData[dataLocation] < bankOffset))) {
      dataLocation = 0;
      Serial.println("bad data found, resetting positions");
    } else {
      // wait until we have filled 8 slots of data, then do what needs to be done
      if (dataLocation == 8) {
        Serial.println("8 slots filled, sending outputs");
        if (incomingData[2] == adminNumber) {
          //call admin functions
        } else {
          //normal operation
          updateOutputs();
        }
        dataLocation = 0;
      } else {
        dataLocation++;
      }
    }
  }
}

void Communication::updateOutputs() {
  int tempBankOffset;
  tempBankOffset = incomingData[1] - bankOffset;
  for (int i = 2; i < 9; i++) {
    
    outputs.updateOutput(tempBankOffset*7 + i-2, int((incomingData[i]-firstNumber) * scaleFactor));
  }
}
