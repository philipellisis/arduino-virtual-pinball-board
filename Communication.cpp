#include "Communication.h"
#include <Arduino.h>
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Enums.h"



Communication::Communication() {
}




void Communication::init(Plunger* plunger, Accelerometer* accel, Buttons* buttons, Config* config, Outputs* outputs) {
  if (DEBUG) {Serial.println(F("Communication: initializing Communication"));}
  _outputs = outputs;
  _plunger = plunger;
  _accelerometer = accel;
  _buttons = buttons;
  _config = config;
}

void Communication::communicate() {
  _outputs->checkResetOutputs();
  while (Serial.available() > 0) {
    incomingData[dataLocation] = Serial.read();
    if (DEBUG) {Serial.print(F("getting serial data: ")); Serial.println(incomingData[dataLocation]);}
    // data sent is always [0][200 + bank offset][output value 1][output value 2][output value 3][output value 4][output value 5][output value 6][output value 7]
    // check to make sure we are reading what's expected for the first 2 bytes of data. If not, reset and start the buffer over again
    // sample data coming in: 0È0000000
    // sample data coming in for test: " d       "
    // sample data coming in for test all on : " d}}}}}}}"
    // sample data coming in for test all off: " d       "
    if ((dataLocation == 0 && incomingData[0] != firstNumber) || (dataLocation == 1 && (incomingData[1] < bankOffset))) {
      dataLocation = 0;
      if (DEBUG) {Serial.println(F("bad data found, resetting positions"));}
    } else {
      // wait until we have filled 9 slots of data, then do what needs to be done
      if (dataLocation == 8) {
        if (DEBUG) {Serial.println(F("9 slots filled, sending outputs"));}
        if (incomingData[1] == adminNumber) {
          if (DEBUG) {Serial.println(F("Turning admin on"));}
          // set admin functions{
          admin = incomingData[2];
        } else if (incomingData[1] == connectionNumber) {
          Serial.println(F("CSD Board Connected"));
        } else if (incomingData[1] == outputSingleNumber) {
          _outputs->updateOutput(incomingData[2], incomingData[3]);
        } else {
          //normal operation
          if (DEBUG) {Serial.println(F("sending output"));}
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
    if (DEBUG) {Serial.println(F("going into admin mode"));}
    if (admin == BUTTONS) {
      _buttons->sendButtonState();
    }
    if (admin == OUTPUTS) {
      _outputs->sendOutputState();
    }
    if (admin == PLUNGER) {
      _plunger->sendPlungerState();
    }
    if (admin == ACCEL) {
      _accelerometer->sendAccelerometerState();
    }
    if (admin == SEND_CONFIG) {
      _config->sendConfig();
      admin = 0;
    }
    if (admin == GET_CONFIG) {
      _config->updateConfigFromSerial();
      admin = 0;
    }
    if (admin == SET_PLUNGER) {
      _config->setPlunger();
      admin = 0;
    }
    if (admin == OFF) {
      admin = 0;
    }
    if (admin == CONNECT) {
      Serial.println(F("CSD Board Connected"));
      admin = 0;
    }
  }
}

void Communication::updateOutputs() {
  if (DEBUG) {Serial.println(F("sending output"));}
  int tempBankOffset;
  tempBankOffset = incomingData[1] - bankOffset;
  for (int i = 2; i < 9; i++) {
    _outputs->updateOutput(tempBankOffset*7 + i-2, int((incomingData[i]-firstNumber) * scaleFactor));
  }
}
