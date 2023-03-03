#include "Accelerometer.h"
#include <Arduino.h>
#include <Joystick.h>
#include <Adafruit_MPU6050.h>


Adafruit_MPU6050 mpu;
Accelerometer::Accelerometer() {
  if (DEBUG) {Serial.println("accel: About to initialize pins");}

  if (DEBUG) {Serial.println("plunger: pins initialized");}
}

void Accelerometer::init(Joystick_* joystick) {
  if (DEBUG) {Serial.println("plunger: initializing accel");}
  _joystick = joystick;
  _joystick->setXAxisRange(-15, 15);
  _joystick->setYAxisRange(-15, 15);
  while (!mpu.begin()) {
    if (DEBUG) {Serial.println("Failed to find MPU6050 chip");}
    delay(1000);
  }

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  
  if (DEBUG) {Serial.println("MPU6050 Found!");}
  if (DEBUG) {Serial.println("accel: initialized joystick");}
}

void Accelerometer::accelerometerRead() {
  //xValue = 0;
  //yValue = 0;
  //if(mpu.getMotionInterruptStatus()) {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  xValue = a.acceleration.x - xValueOffset;
  yValue = a.acceleration.y - yValueOffset;
  //}
  if (xValueOffset == 0){
    xValueOffset = a.acceleration.x;
  }
  if (yValueOffset == 0){
    yValueOffset = a.acceleration.y;
  }
  _joystick->setXAxis(xValue);
  _joystick->setYAxis(yValue);
  if (DEBUG) {Serial.print("AccelX:");}
  if (DEBUG) {Serial.print(xValue);}
  if (DEBUG) {Serial.print(",");}
  if (DEBUG) {Serial.print("AccelY:");}
  if (DEBUG) {Serial.println(yValue);}
}

void Accelerometer::sendAccelerometerState() {
  Serial.print(xValue);
  Serial.print(",");
  Serial.println(yValue);
  
}
