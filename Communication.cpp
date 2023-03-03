#include "Communication.h"
#include <Arduino.h>
#include "Outputs.h"
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Enums.h"


Outputs outputs = Outputs();
Communication::Communication() {
  if (DEBUG) {Serial.println("Communication: About to initialize serial");}

  if (DEBUG) {Serial.println("Communication: pins initialized");}
}




void Communication::init(Plunger* plunger, Accelerometer* accel, Buttons* buttons) {
  if (DEBUG) {Serial.println("Communication: initializing Communication");}
  _plunger = plunger;
  _accelerometer = accel;
  _buttons = buttons;
  outputs.init();
  if (DEBUG) {Serial.println("Communication: initialized Communication");}
}

void Communication::communicate() {
  while (Serial.available() > 0) {
    incomingData[dataLocation] = Serial.read();
    if (DEBUG) {Serial.print("getting serial data: "); Serial.println(incomingData[dataLocation]);}
    // data sent is always [0][200 + bank offset][output value 1][output value 2][output value 3][output value 4][output value 5][output value 6][output value 7]
    // check to make sure we are reading what's expected for the first 2 bytes of data. If not, reset and start the buffer over again
    // sample data coming in: 0È0000000
    // sample data coming in for test: " d       "
    // sample data coming in for test all on : " d}}}}}}}"
    // sample data coming in for test all off: " d       "
    if ((dataLocation == 0 && incomingData[dataLocation] != firstNumber) || (dataLocation == 1 && (incomingData[dataLocation] < bankOffset))) {
      dataLocation = 0;
      if (DEBUG) {Serial.println("bad data found, resetting positions");}
    } else {
      // wait until we have filled 8 slots of data, then do what needs to be done
      if (dataLocation == 8) {
        if (DEBUG) {Serial.println("8 slots filled, sending outputs");}
        if (incomingData[1] == adminNumber) {
          if (DEBUG) {Serial.println("Turning admin on");}
          // set admin functions
          if (incomingData[3] == 0) {
            admin = incomingData[4];
          }
        } else if (incomingData[1] == connectionNumber) {
          if (DEBUG) {Serial.println("CSD Board Connected");}
        } else {
          //normal operation
          if (DEBUG) {Serial.println("sending output");}
          updateOutputs();
        }
        dataLocation = 0;
      } else {
        dataLocation++;
      }
    }
    sendAdmin();
  }
}

void Communication::sendAdmin() {
  if (admin > 0) {
    if (DEBUG) {Serial.println("going into admin mode");}
    // buttons
    if (admin == BUTTONS) {
      _buttons->sendButtonState();
    }
    // getoutputs
    if (admin == OUTPUTS) {
      outputs.sendOutputState();
    }
    // plunger
    if (admin == PLUNGER) {
      _plunger->sendPlungerState();
    }
    // accel
    if (admin == ACCEL) {
      _accelerometer->sendAccelerometerState();
    }
  }
}

void Communication::updateOutputs() {
  if (DEBUG) {Serial.println("sending output");}
  int tempBankOffset;
  tempBankOffset = incomingData[1] - bankOffset;
  for (int i = 2; i < 9; i++) {
    
    outputs.updateOutput(tempBankOffset*7 + i-2, int((incomingData[i]-firstNumber) * scaleFactor));
  }
}
