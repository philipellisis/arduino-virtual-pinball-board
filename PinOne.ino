#include "HID-Project.h"
#include <Wire.h>
#include "Globals.h"


Plunger plunger;
Buttons buttons;
LightShow lightShow;

Accelerometer accel;
Communication comm;
Outputs outputs;
Config config;
bool DEBUG = false;

void setup() {

  Serial.begin(9600);
  delay(1000);
  //if (DEBUG) {Serial.print(F("DEBUG,Starting up arduino\r\n"));}
  delay(1000);
  // Initialize Gamepad1 Library
  Gamepad1.begin();
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
  // unsigned long t1 = micros();
  if (!buttons.readInputs() && !USBDevice.isSuspended()) {
    plunger.plungerRead();
    if (config.accelerometerEprom > 0) {
      accel.accelerometerRead();
    }
    lightShow.checkSetLights();
    if (config.updateUSB) {
      Gamepad1.write();
      config.updateUSB = false;
    }
    comm.communicate();
  } else {
    Gamepad1.write();
    //unsigned long t2 = micros();
    // Serial.print(F("DEBUG,Time taken by the task: "));
    // Serial.print(t2 - t1);
    // Serial.println(F(" microseconds"));
    // delay(100);
  }

  //Serial.println("arduino is running");
  
}