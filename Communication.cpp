#include "Communication.h"
#include <Arduino.h>
#include "Plunger.h"
#include "HID-Project.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Enums.h"
#include "Globals.h"
#include <avr/wdt.h>

Communication::Communication() {
}



void Communication::communicate() {
  outputs.checkResetOutputs();
  //int maxSerial = Serial.available();
  for (uint8_t i = 0; i < 9; i++) {
    if (Serial.available()) {
      incomingData[dataLocation] = Serial.read();
      // Serial.print(F("DEBUG,getting serial data: ")); Serial.print(incomingData[dataLocation]); Serial.print(F("\r\n"));
      // data sent is always [0][200 + bank offset][output value 1][output value 2][output value 3][output value 4][output value 5][output value 6][output value 7]
      // check to make sure we are reading what's expected for the first 2 bytes of data. If not, reset and start the buffer over again
      // sample data coming in: 0È0000000
      // sample data coming in for test: " d       "
      // sample data coming in for test all on : " d}}}}}}}"
      // sample data coming in for test all off: " d       "
      if ((dataLocation == 0 && incomingData[0] != firstNumber) || (dataLocation == 1 && (incomingData[1] < bankOffset))) {
        dataLocation = 0;
        // Serial.print(F("DEBUG,bad data found, resetting positions\r\n"));
      } else {
        // wait until we have filled 9 slots of data, then do what needs to be done
        if (dataLocation == 8) {
          // Serial.print(F("DEBUG,9 slots filled, sending outputs\r\n"));
          if (incomingData[1] == adminNumber) {
            config.lightShowState = DISABLED;
            //if (DEBUG) {Serial.print(F("DEBUG,Turning admin on\r\n"));}
            // set admin functions{
            admin = incomingData[2];
          } else if (incomingData[1] == connectionNumber) {
            Serial.print(connectedString);
          } else if (incomingData[1] == outputSingleNumber) {
            outputs.updateOutput(incomingData[2], incomingData[3]);
          } else {
            //normal operation
            //if (DEBUG) {Serial.print(F("DEBUG,sending output\r\n"));}
            if (config.lightShowState != OUTPUT_RECEIVED) {
              lightShow.setLightsOff();
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

    switch (admin)
    {
    case BUTTONS:
    case OUTPUTS:
    case PLUNGER:
    case ACCEL:
      handleDelayedAdmin(admin);
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
      Serial.print(F("V,1.20.0\r\n"));
      admin = 0;
      break;
    case RESET:
      pinMode(11, OUTPUT);
      digitalWrite(11, LOW);
      wdt_enable(WDTO_15MS);
      admin = 0;
    }
    
  }
}

void Communication::handleDelayedAdmin(uint8_t adminType) {
  if (shouldDelay()) return;
  
  switch (adminType) {
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
  }
}

bool Communication::shouldDelay() {
  if (delayIncrementor < 20) {
    delayIncrementor++;
    return true;
  } else {
    delayIncrementor = 0;
    return false;
  }
}

void Communication::updateOutputs() {
  //if (DEBUG) {Serial.print(F("DEBUG,sending output\r\n"));}
  uint8_t baseIndex = (incomingData[1] - bankOffset) * 7;
  for (uint8_t i = 2; i < 9; i++) {
    uint8_t outputIndex = baseIndex + i - 2;
    if (previousDOFValues[outputIndex] != incomingData[i]) {
      outputs.updateOutput(outputIndex, incomingData[i]);
      previousDOFValues[outputIndex] = incomingData[i];
    }
  }
}