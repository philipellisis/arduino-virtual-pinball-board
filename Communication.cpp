#include "Communication.h"
#include "Plunger.h"
#include <Arduino.h>
#include <Joystick.h>
#include "Outputs.h"



Communication::Communication() {
  Serial.println("Communication: About to initialize serial");

  Serial.println("Communication: pins initialized");
}

void Communication::init(Joystick_* joystick) {
  Serial.println("Communication: initializing Communication");
  _joystick = joystick;
  Serial.println("Communication: initialized Communication");
}

void Communication::communicate() {
  if (Serial.available() > 0) {
    incomingData[dataLocation] = Serial.read();
    // data sent is always [0][200 + bank offset][output value 1][output value 2][output value 3][output value 4][output value 5][output value 6][output value 7]
    // check to make sure we are reading what's expected for the first 2 bytes of data. If not, reset and start the buffer over again
    if ((dataLocation == 0 && incomingData[dataLocation] != 0) || (dataLocation == 1 && (incomingData[dataLocation] < 200))) {
      dataLocation = 0;
    } else {
      // wait until we have filled 8 slots of data, then do what needs to be done
      if (dataLocation == 8) {
        if (incomingData[2] == 255) {
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
  int bankOffset;
  bankOffset = incomingData[1] - 200;
  for (int i = 2; i < 9; i++) {
    
  }
}
