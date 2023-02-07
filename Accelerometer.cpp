#include "Accelerometer.h"
#include <Arduino.h>
#include <Joystick.h>
#include <Adafruit_MPU6050.h>


Adafruit_MPU6050 mpu;
Accelerometer::Accelerometer() {
  Serial.println("accel: About to initialize pins");

  Serial.println("plunger: pins initialized");
}

void Accelerometer::init(Joystick_* joystick) {
  Serial.println("plunger: initializing accel");
  _joystick = joystick;
  _joystick->setXAxisRange(-100, 100);
  _joystick->setYAxisRange(-100, 100);
   if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  
  Serial.println("MPU6050 Found!");
  Serial.println("accel: initialized joystick");
}

void Accelerometer::accelerometerRead() {
  xValue = 0;
  yValue = 0;
  if(mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    Serial.print("AccelX:");
    Serial.print(a.acceleration.x);
    xValue = a.acceleration.x;
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    yValue = a.acceleration.y;
  }
  _joystick->setXAxis(xValue);
  _joystick->setYAxis(yValue);
}
