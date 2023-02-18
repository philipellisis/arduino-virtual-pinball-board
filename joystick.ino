//#include <Adafruit_PWMServoDriver.h>
//#include <EEPROM.h>
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Communication.h"
#include <Joystick.h>

#include <Wire.h>

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  24, 0,                  // Button Count, Hat Switch Count
  true, true, true,      // X and Y, Z axis for plunger
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering
Plunger plunger = Plunger();
Buttons buttons = Buttons();

Accelerometer accel = Accelerometer();
Communication comm = Communication();

void setup() {

  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting up arduino");
  delay(1000);
  // Initialize Joystick Library
  Joystick.begin();
  buttons.init(&Joystick);
  plunger.init(&Joystick);
  accel.init(&Joystick);
  comm.init(&plunger, &accel, &buttons);
  //comm.init();
  
}



void loop() {
  long int t1 = millis();
  buttons.readInputs();
  plunger.plungerRead();
  accel.accelerometerRead();
  comm.communicate();
  long int t2 = millis();
  Serial.print("Time taken by the task: "); Serial.print(t2-t1); Serial.println(" milliseconds");

  //Serial.println("arduino is running");
  delay(100);
}
