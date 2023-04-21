#include "Accelerometer.h"
#include <Arduino.h>
#include <Joystick.h>
#include <Adafruit_MPU6050.h>


Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_accel;
Accelerometer::Accelerometer() {
}

void Accelerometer::init(Joystick_* joystick, Config* config) {
  _joystick = joystick;
  _config = config;
  _joystick->setXAxisRange(-1500, 1500);
  _joystick->setYAxisRange(-1500, 1500);
  byte count = 0;
  while (!mpu.begin() && config->accelerometer == 0) {
    if (DEBUG) {Serial.print(F("DEBUG,Failed to find MPU6050 chip\r\n"));}
    delay(1000);
    count++;
    if (count > 5) {
      config->accelerometer = 2;
      break;
    }
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
  if (DEBUG) {Serial.print(F("DEBUG,MPU6050 Found!\r\n"));}
}

void Accelerometer::accelerometerRead() {
  //xValue = 0;
  //yValue = 0;
  //if(mpu.getMotionInterruptStatus()) {
  /* Get new sensor events with the readings */
  sensors_event_t a;
  mpu_accel->getEvent(&a);

  xValue = floor((a.acceleration.x - xValueOffset)*100);
  yValue = floor((a.acceleration.y - yValueOffset)*100);
  //}
  if (xValueOffset == 0){
    xValueOffset = a.acceleration.x;
  }
  if (yValueOffset == 0){
    yValueOffset = a.acceleration.y;
  }
  _joystick->setXAxis(xValue);
  _joystick->setYAxis(yValue);
  if (DEBUG) {Serial.print(F("DEBUG,AccelX:"));}
  if (DEBUG) {Serial.print(xValue);}
  if (DEBUG) {Serial.print(F(","));}
  if (DEBUG) {Serial.print(F("AccelY:"));}
  if (DEBUG) {Serial.print(yValue);}
  if (DEBUG) {Serial.print(F("\r\n"));}
}

void Accelerometer::sendAccelerometerState() {
  if (DEBUG) {Serial.print(F("ACCEL,"));}
  Serial.print(xValue);
  Serial.print(F(","));
  Serial.print(yValue);
  Serial.print(F("\r\n"));
  
}
