#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Communication.h"
#include "Outputs.h"
#include <Joystick.h>
#include "Config.h"
#include <Wire.h>
#include "LightShow.h"

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  28, 0,                  // Button Count, Hat Switch Count
  true, true, true,      // X and Y, Z axis for plunger
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering
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
  // Initialize Joystick Library
  config.init();
  outputs.init(&config);
  Joystick.begin();
  buttons.init(&Joystick, &config, &outputs);
  plunger.init(&Joystick, &config);
  lightShow.init(&config, &outputs);
  if (config.accelerometer > 0) {
    accel.init(&Joystick, &config);
  }
  
  comm.init(&plunger, &accel, &buttons, &config, &outputs, &Joystick);
  
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
    comm.communicate();
  }
  //long int t2 = millis();
  //Serial.print(F("DEBUG,Time taken by the task: ")); Serial.print((t2-t1)); Serial.print(F(" milliseconds\r\n"));

  //Serial.println("arduino is running");
  //delay(100);
}
