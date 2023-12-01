#include "Communication.h"
#include <Arduino.h>
#include "Plunger.h"
#include "HID-Project.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Enums.h"
#include "Globals.h"





Communication::Communication() {
}



void Communication::communicate() {
  outputs.checkResetOutputs();
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
            config.lightShowState = DISABLED;
            //if (DEBUG) {Serial.print(F("DEBUG,Turning admin on\r\n"));}
            // set admin functions{
            admin = incomingData[2];
          } else if (incomingData[1] == connectionNumber) {
            Serial.print(connectedString);
          } else if (incomingData[1] == outputSingleNumber) {
            outputs.updateOutput(incomingData[2], incomingData[3]);
          } else if (incomingData[1] == outputButtonNumber) {
            switch(incomingData[2]) {
              case 28:
                Gamepad1.xAxis(-32767);
                break;
              case 29:
                Gamepad1.xAxis(32767);
                break;
              case 30:
                Gamepad1.yAxis(-32767);
                break;
              case 31:
                Gamepad1.yAxis(32767);
                break;
              case 32:
                Gamepad1.zAxis(-32767);
                break;
              case 33:
                Gamepad1.zAxis(32767);
                break;
              default:
                buttons.sendButtonPush(incomingData[2], 1);
                delay(500);
                buttons.sendButtonPush(incomingData[2], 0);
            }
            delay(500);

          } else {
            //normal operation
            //if (DEBUG) {Serial.print(F("DEBUG,sending output\r\n"));}
            if (config.lightShowState != OUTPUT_RECEIVED) {
              outputs.turnOff();
            }
            config.lightShowState = OUTPUT_RECEIVED_RESET_TIMER;
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
    delay(50);
    switch (admin)
    {
    case BUTTONS:
      buttons.sendButtonState();
      break;
    case OUTPUTS:
      outputs.sendOutputState();
      break;
    case PLUNGER:
      plunger.sendPlungerState();
      break;
    case ACCEL:
      accel.sendAccelerometerState();
      break;
    case SEND_CONFIG:
      config.sendConfig();
      admin = 0;
      break;
    case GET_CONFIG:
      config.updateConfigFromSerial();
      plunger.resetPlunger();
      accel.resetAccelerometer();
      admin = 0;
      break;
    case OFF:
      admin = 0;
      config.lightShowState = OUTPUT_RECEIVED_RESET_TIMER;
      outputs.turnOff();
      break;
    case CONNECT:
      Serial.print(connectedString);
      admin = 0;
      break;
    case VERSION:
      Serial.print(F("V,1.11.0\r\n"));
      admin = 0;
      break;
    }
    
  }
}

void Communication::updateOutputs() {
  //if (DEBUG) {Serial.print(F("DEBUG,sending output\r\n"));}
  int tempBankOffset;
  tempBankOffset = incomingData[1] - bankOffset;
  for (int i = 2; i < 9; i++) {
    if (previousDOFValues[tempBankOffset*7 + i-2] != incomingData[i]) {
      outputs.updateOutput(tempBankOffset*7 + i-2, incomingData[i]);
      previousDOFValues[tempBankOffset*7 + i-2] = incomingData[i];
    }
  }
}
