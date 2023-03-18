#include "Accelerometer.h"
#include <Arduino.h>
#include <Joystick.h>
#include <Adafruit_MPU6050.h>


Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_accel;
Accelerometer::Accelerometer() {
}

void Accelerometer::init(Joystick_* joystick) {
  _joystick = joystick;
  _joystick->setXAxisRange(-15, 15);
  _joystick->setYAxisRange(-15, 15);
  while (!mpu.begin()) {
    if (DEBUG) {Serial.println(F("Failed to find MPU6050 chip"));}
    delay(1000);
  }
  
  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  //assign accelerometer
  mpu_accel = mpu.getAccelerometerSensor();
  if (DEBUG) {Serial.println(F("MPU6050 Found!"));}
}

void Accelerometer::accelerometerRead() {
  //xValue = 0;
  //yValue = 0;
  //if(mpu.getMotionInterruptStatus()) {
  /* Get new sensor events with the readings */
  sensors_event_t a;
  mpu_accel->getEvent(&a);

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
  if (DEBUG) {Serial.print(F("AccelX:"));}
  if (DEBUG) {Serial.print(xValue);}
  if (DEBUG) {Serial.print(F(","));}
  if (DEBUG) {Serial.print(F("AccelY:"));}
  if (DEBUG) {Serial.println(yValue);}
}

void Accelerometer::sendAccelerometerState() {
  Serial.print(xValue);
  Serial.print(",");
  Serial.println(yValue);
  
}
