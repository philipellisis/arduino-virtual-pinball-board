#include <Wire.h>
#include "Globals.h"
#include <XInput.h>


Plunger plunger;
Buttons buttons;
LightShow lightShow;

Accelerometer accel;
Communication comm;
Outputs outputs;
Config config;
bool DEBUG = false;
long int leftOn = 0;
long int rightOn = 0;

void setup() {

  Serial.begin(9600);
  delay(1000);
  //if (DEBUG) {Serial.print(F("DEBUG,Starting up arduino\r\n"));}
  delay(1000);
  // Initialize Gamepad1 Library
  XInput.setAutoSend(false);
  XInput.begin();
  XInput.setRange(JOY_RIGHT, -127, 127);
  XInput.setRange(JOY_LEFT, -32767, 32767);
  config.init();
  outputs.init();

  buttons.init();
  plunger.init();
  lightShow.init();
  if (config.accelerometer > 0) {
    accel.init();
  }

  
}



void loop() {
  //long int t1 = millis();

  long int currentTime = millis();

  if (!buttons.readInputs() && !USBDevice.isSuspended()) {

    uint8_t bigRumble = XInput.getRumbleLeft();
    uint8_t smallRumble = XInput.getRumbleRight();
    if (bigRumble > 1 && leftOn == 0) {
      outputs.updateOutput(0, 255);
      leftOn = currentTime;
    } else if (bigRumble == 0 && leftOn > 0 && currentTime - leftOn > 100) {
      outputs.updateOutput(0, 0);
      leftOn = 0;
    }

    if (smallRumble > 1 && rightOn == 0) {
      outputs.updateOutput(1, 255);
      rightOn = currentTime;
    } else if (smallRumble == 0 && rightOn > 0 && currentTime - rightOn > 100) {
      outputs.updateOutput(1, 0);
      rightOn = 0;
    }


    plunger.plungerRead();
    if (config.accelerometerEprom > 0) {
      accel.accelerometerRead();
    }
    lightShow.checkSetLights();
    if (config.updateUSB) {
      XInput.send();
      config.updateUSB = false;
    }
    comm.communicate();
  } else {
    XInput.send();
  }
  // long int t2 = millis();
  // Serial.print(F("DEBUG,Time taken by the task: ")); Serial.print((t2-t1)); Serial.print(F(" milliseconds\r\n"));
  // delay(100);
  //Serial.println("arduino is running");
  
}
