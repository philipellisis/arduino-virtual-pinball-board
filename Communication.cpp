#include "Communication.h"
#include <Arduino.h>
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Enums.h"



Communication::Communication() {
}




void Communication::init(Plunger* plunger, Accelerometer* accel, Buttons* buttons, Config* config, Outputs* outputs, Joystick_* joystick) {
  //if (DEBUG) {Serial.print(F("Communication: initializing Communication\r\n"));}
  _outputs = outputs;
  _plunger = plunger;
  _accelerometer = accel;
  _buttons = buttons;
  _config = config;
  _joystick = joystick;
}

void Communication::communicate() {
  _outputs->checkResetOutputs();
  //int maxSerial = Serial.available();
  for (int i = 0; i < 9; i++) {
    if (Serial.available()) {
      incomingData[dataLocation] = Serial.read();
      //if (DEBUG) {Serial.print(F("DEBUG,getting serial data: ")); Serial.print(incomingData[dataLocation]); Serial.print(F("\r\n"));}
      // data sent is always [0][200 + bank offset][output value 1][output value 2][output value 3][output value 4][output value 5][output value 6][output value 7]
      // check to make sure we are reading what's expected for the first 2 bytes of data. If not, reset and start the buffer over again
      // sample data coming in: 0È0000000
      // sample data coming in for test: " d       "
      // sample data coming in for test all on : " d}}}}}}}"
      // sample data coming in for test all off: " d       "
      if ((dataLocation == 0 && incomingData[0] != firstNumber) || (dataLocation == 1 && (incomingData[1] < bankOffset))) {
        dataLocation = 0;
        //if (DEBUG) {Serial.print(F("DEBUG,bad data found, resetting positions\r\n"));}
      } else {
        // wait until we have filled 9 slots of data, then do what needs to be done
        if (dataLocation == 8) {
          //if (DEBUG) {Serial.print(F("DEBUG,9 slots filled, sending outputs\r\n"));}
          if (incomingData[1] == adminNumber) {
            _config->lightShowState = DISABLED;
            //if (DEBUG) {Serial.print(F("DEBUG,Turning admin on\r\n"));}
            // set admin functions{
            admin = incomingData[2];
          } else if (incomingData[1] == connectionNumber) {
            Serial.print(connectedString);
          } else if (incomingData[1] == outputSingleNumber) {
            _outputs->updateOutput(incomingData[2], incomingData[3]);
          } else if (incomingData[1] == outputButtonNumber) {
            if (incomingData[2] < 28) {
              _joystick->setButton(incomingData[2], 1);
              delay(500);
              _joystick->setButton(incomingData[2], 0);
            } else if (incomingData[2] == 28) {
              _joystick->setXAxis(-100000);
            } else if (incomingData[2] == 29) {
              _joystick->setXAxis(100000);
            } else if (incomingData[2] == 30) {
              _joystick->setYAxis(-100000);
            } else if (incomingData[2] == 31) {
              _joystick->setYAxis(100000);
            } else if (incomingData[2] == 32) {
              _joystick->setZAxis(-100000);
            } else {
              _joystick->setZAxis(100000);
            }
            delay(500);

          } else {
            //normal operation
            //if (DEBUG) {Serial.print(F("DEBUG,sending output\r\n"));}
            _config->lightShowState = OUTPUT_RECEIVED_RESET_TIMER;
            updateOutputs();

          }
          dataLocation = 0;
        } else {
          dataLocation++;
        }
      }
    } else {
      break;
    }
  }
  sendAdmin();
}

void Communication::sendAdmin() {


    

  if (admin > 0) {
    switch (admin)
    {
    case BUTTONS:
      _buttons->sendButtonState();
      break;
    case OUTPUTS:
      _outputs->sendOutputState();
      break;
    case PLUNGER:
      _plunger->sendPlungerState();
      break;
    case ACCEL:
      _accelerometer->sendAccelerometerState();
      break;
    case SEND_CONFIG:
      _config->sendConfig();
      admin = 0;
      break;
    case GET_CONFIG:
      _config->updateConfigFromSerial();
      admin = 0;
      break;
    case SET_PLUNGER:
      _config->setPlunger();
      _plunger->resetPlunger();
      admin = 3;
      break;
    case SET_ACCEL:
      _config->setAccelerometer();
      _accelerometer->resetAccelerometer();
      admin = 4;
      break;
    case SAVE_CONFIG:
      _config->saveConfig();
      admin = 0;
      break;
    case OFF:
      admin = 0;
      _config->lightShowState = OUTPUT_RECEIVED_RESET_TIMER;
      _outputs->turnOff();
      break;
    case CONNECT:
      Serial.print(connectedString);
      admin = 0;
      break;
    case VERSION:
      Serial.print(F("V,1.7.0\r\n"));
      admin = 0;
      break;
    }
    delay(50);
  }
}

void Communication::updateOutputs() {
  //if (DEBUG) {Serial.print(F("DEBUG,sending output\r\n"));}
  int tempBankOffset;
  tempBankOffset = incomingData[1] - bankOffset;
  for (int i = 2; i < 9; i++) {
    _outputs->updateOutput(tempBankOffset*7 + i-2, int((incomingData[i]-firstNumber) * scaleFactor));
  }
}
