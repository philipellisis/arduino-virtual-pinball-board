#include "Accelerometer.h"
#include <Arduino.h>
#include "HID-Project.h"
#include "MPU6050.h"
#include "Enums.h"
#include "Globals.h"



MPU6050 mpu;
Accelerometer::Accelerometer()
{
}

void Accelerometer::init()
{

  unsigned char count = 0;
  if (!mpu.init())
  {
    delay(1000);
  }
  while (!mpu.init())
  {
    // if (DEBUG) {Serial.print(F("DEBUG,Failed to find MPU6050 chip\r\n"));}
    delay(100);
    if (count > 10)
    {
      config.accelerometer = 0;
      config.accelerometerEprom = 0;
      break;
    }
    count++;
  }
  if (count > 10)
  {
    return;
  }
  else
  {
    count = 0;
  }

  mpu.config();
  resetAccelerometer();

  // if (DEBUG) {Serial.print(F("DEBUG,MPU6050 Found!\r\n"));}
}

void Accelerometer::centerAccelerometer()
{
  delay(400);

  unsigned char count = 0;
  xValueOffset = 0;
  yValueOffset = 0;
  while (count < 50)
  {
    mpu.read();
    if (config.orientation > 7) {
      xValueOffset += mpu.getZ();
    } else {
      xValueOffset += mpu.getX();
    }
    yValueOffset += mpu.getY();

    count++;
  }
  xValueOffset = xValueOffset / 50;
  yValueOffset = yValueOffset / 50;
}

void Accelerometer::resetAccelerometer()
{
  if (config.orientation > 7) {
    orientation = config.orientation - 8;
  } else {
    orientation = config.orientation;
  }
  localMax = static_cast<float>(config.accelerometerMax);
  mpu.setAccelerometerRange(config.accelerometerSensitivity);
  centerAccelerometer();
}

void Accelerometer::accelerometerRead()
{
  if (config.restingStateCounter != config.restingStateMax && config.disableAccelOnPlungerMove == 1) {
    return;
  }
  if (config.lightShowState == IN_RANDOM_MODE_WAITING_INPUT)
  {
    if (recentered == false)
    {
      centerAccelerometer();
      recentered = true;
    }
  }
  else
  {
    recentered = false;
  }
  // xValue = 0;
  // yValue = 0;
  // if(mpu.getMotionInterruptStatus()) {
  /* Get new sensor events with the readings */
  mpu.read();

  if (config.orientation > 7) {
    xValue = floor((mpu.getZ() - xValueOffset) * 100);
  } else {
    xValue = floor((mpu.getX() - xValueOffset) * 100);
  }
  yValue = floor((mpu.getY() - yValueOffset) * 100);
  
  if (abs(xValue) < config.accelerometerDeadZone)
  {
    xValue = 0;
  }
  if (abs(yValue) < config.accelerometerDeadZone)
  {
    yValue = 0;
  }
  int temp = xValue;
  if (orientation == RIGHT)
  {
    xValue = -yValue;
    yValue = temp;
  }
  else if (orientation == FORWARD)
  {
    xValue = -xValue;
    yValue = -yValue;
  }
  else if (orientation == LEFT)
  {
    xValue = yValue;
    yValue = -temp;
  }
  else if (orientation == UP_BACK)
  {
    xValue = -xValue;
    yValue = yValue;
  }
  else if (orientation == UP_RIGHT)
  {
    xValue = -yValue;
    yValue = -temp;
  }
  else if (orientation == UP_FORWARD)
  {
    xValue = xValue;
    yValue = -yValue;
  }
  else if (orientation == UP_LEFT)
  {
    xValue = yValue;
    yValue = temp;
  }

  if (tiltSuppressTime > 0) {
    tiltSuppressTime--;
  } else {
    if (buttonState == 0 && (abs(xValue) > config.accelerometerTilt || abs(yValue) > config.accelerometerTilt))
    {
      buttonState = buttons.sendButtonPush(config.tiltButton, 1);
    }
    else if (buttonState == 1 && (abs(xValue) < config.accelerometerTilt && abs(yValue) < config.accelerometerTilt))
    {
      buttonState = buttons.sendButtonPush(config.tiltButton, 0);
      tiltSuppressTime = config.tiltSuppress;
    }
  }


  if (priorXValue != xValue) {
    Gamepad1.xAxis(static_cast<int16_t>(xValue / localMax * 32767));
    priorXValue = xValue;
    config.updateUSB = true;
  }

  if (priorYValue != yValue) {
    Gamepad1.yAxis(static_cast<int16_t>(yValue / localMax * 32767));
    priorYValue = yValue;
    config.updateUSB = true;
  }

  // Serial.print(F("DEBUG,AccelX:"));
  // Serial.print(xValue);
  // Serial.print(F(","));
  // Serial.print(F("AccelY:"));
  // Serial.print(yValue);
  // Serial.print(F(","));
  // Serial.print(F("offsetX:"));
  // Serial.print(xValueOffset);
  // Serial.print(F(","));
  // Serial.print(F("offsetY:"));
  // Serial.print(yValueOffset);
  // Serial.print(F(","));
  // Serial.print(F("rawX:"));
  // Serial.print(mpu.getZ());
  // Serial.print(F(","));
  // Serial.print(F("rawY:"));
  // Serial.print(mpu.getY());
  // Serial.print(F("\r\n"));
}

void Accelerometer::sendAccelerometerState()
{
  Serial.print(F("A,"));
  if (config.orientation > 7) {
     Serial.print((mpu.getZ() - xValueOffset));
  } else {
     Serial.print((mpu.getX() - xValueOffset));
  }
  Serial.print(F(","));
  Serial.print((mpu.getY() - yValueOffset));
  Serial.print(F(","));
  Serial.print(xValue);
  Serial.print(F(","));
  Serial.print(yValue);
  Serial.print(F("\r\n"));
}