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
  while (!mpu.begin() && _config->accelerometer == 0) {
    if (DEBUG) {Serial.print(F("DEBUG,Failed to find MPU6050 chip\r\n"));}
    delay(5000);

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

  sensors_event_t a;
  while (count < 10) {
    mpu_accel->getEvent(&a);
    xValueOffset += a.acceleration.x;
    yValueOffset += a.acceleration.y;
    count++;
  }
  xValueOffset = xValueOffset/10;
  yValueOffset = yValueOffset/10;


  
  if (DEBUG) {Serial.print(F("DEBUG,MPU6050 Found!\r\n"));}
}

void Accelerometer::accelerometerRead() {
  //xValue = 0;
  //yValue = 0;
  //if(mpu.getMotionInterruptStatus()) {
  /* Get new sensor events with the readings */
  sensors_event_t a;
  mpu_accel->getEvent(&a);

  xValue = floor((a.acceleration.x - xValueOffset)*_config->accelerometerMultiplier);
  yValue = floor((a.acceleration.y - yValueOffset)*_config->accelerometerMultiplier);
  if (abs(xValue) < _config->accelerometerDeadZone) {
    xValue = 0;
  }
  if (abs(yValue) < _config->accelerometerDeadZone) {
    yValue = 0;
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
  sensors_event_t a;
  mpu_accel->getEvent(&a);
  Serial.print(F("ACCEL,"));
  Serial.print((a.acceleration.x - xValueOffset));
  Serial.print(F(","));
  Serial.print((a.acceleration.y - yValueOffset));
  Serial.print(F(","));
  Serial.print(xValue);
  Serial.print(F(","));
  Serial.print(yValue);
  Serial.print(F("\r\n"));
  
}
