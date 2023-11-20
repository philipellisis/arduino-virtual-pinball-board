#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Communication.h"
#include "Outputs.h"
#include "HID-Project.h"
#include "Config.h"
#include <Wire.h>
#include "LightShow.h"
#include "singleGamepad1.h"



Plunger plunger = Plunger();
Buttons buttons = Buttons();
LightShow lightShow = LightShow();

Accelerometer accel = Accelerometer();
Communication comm = Communication();
Outputs outputs = Outputs();
Config config = Config();
bool DEBUG = false;

void setup() {

  Serial.begin(9600);
  delay(1000);
  //if (DEBUG) {Serial.print(F("DEBUG,Starting up arduino\r\n"));}
  delay(1000);
  // Initialize Gamepad1 Library
  Gamepad1.begin();
  config.init();
  outputs.init(&config);

  buttons.init(&config, &outputs);
  plunger.init(&config);
  lightShow.init(&config, &outputs);
  if (config.accelerometer > 0) {
    accel.init(&config);
  }
  
  comm.init(&plunger, &accel, &buttons, &config, &outputs);
  
}



void loop() {
  //long int t1 = millis();
  buttons.readInputs();
  if (!USBDevice.isSuspended()) {
    plunger.plungerRead();
    if (config.accelerometerEprom > 0) {
      accel.accelerometerRead();
    }
    lightShow.checkSetLights();
    Gamepad1.write();
    comm.communicate();
  }
  //long int t2 = millis();
  //Serial.print(F("DEBUG,Time taken by the task: ")); Serial.print((t2-t1)); Serial.print(F(" milliseconds\r\n"));
  //delay(100);
  //Serial.println("arduino is running");
  
}
