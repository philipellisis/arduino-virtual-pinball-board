#include "Accelerometer.h"
#include <Arduino.h>
#include "MinimalHID.h"
#include "MPU6050.h"
#include "Enums.h"
#include "Globals.h"



MPU6050 mpu;
Accelerometer::Accelerometer()
{
}

void Accelerometer::init()
{

  uint8_t count = 0;
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

  uint8_t count = 0;
  int32_t offsetxCounter = 0;
  int32_t offsetycounter = 0;

  xValueOffset = 0;
  yValueOffset = 0;
  while (count < 10)
  {
    mpu.read();
    offsetxCounter += getRawAccelValue();
    offsetycounter += mpu.getY();

    count++;
  }
  xValueOffset = offsetxCounter / 10;
  yValueOffset = offsetycounter / 10;
}

void Accelerometer::resetAccelerometer()
{
  if (config.orientation > 7) {
    orientation = config.orientation - 8;
  } else {
    orientation = config.orientation;
  }
  localMax = config.accelerometerMax;
  localMaxY = config.accelerometerMaxY;
  mpu.setAccelerometerRange(config.accelerometerSensitivity);
  centerAccelerometer();
}

void Accelerometer::accelerometerRead()
{
  if (config.plungerMoving == true && config.disableAccelOnPlungerMove == 1) {
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

  xValue = floor((getRawAccelValue() - xValueOffset));
  yValue = floor((mpu.getY() - yValueOffset));

  // Serial.print(F("DEBUG,AccelX:"));
  // Serial.print(xValue);
  // Serial.print(F("DEBUG,AccelXOffset:"));
  // Serial.print(xValueOffset);
  
  if (abs(xValue) < config.accelerometerDeadZone)
  {
    xValue = 0;
  }
  if (abs(yValue) < config.accelerometerDeadZone)
  {
    yValue = 0;
  }
  
  applyOrientationTransform(xValue, yValue);
  processTiltButton();



updateXAxis();
updateYAxis();

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

int16_t Accelerometer::getRawAccelValue() {
  return (config.orientation > 7) ? mpu.getZ() : mpu.getX();
}

void Accelerometer::applyOrientationTransform(int16_t& x, int16_t& y) {
  int16_t temp = x;
  switch (orientation) {
    case RIGHT:
      x = -y; y = temp;
      break;
    case FORWARD:
      x = -x; y = -y;
      break;
    case LEFT:
      x = y; y = -temp;
      break;
    case UP_BACK:
      x = -x;
      break;
    case UP_RIGHT:
      x = -y; y = -temp;
      break;
    case UP_FORWARD:
      y = -y;
      break;
    case UP_LEFT:
      x = y; y = temp;
      break;
  }
}

void Accelerometer::processTiltButton() {
  if (config.tiltButton >= 24) return;
  
  if (tiltSuppressTime > 0) {
    tiltSuppressTime--;
    return;
  }
  
  bool tiltThresholdExceeded = (abs(xValue) > config.accelerometerTilt || abs(yValue) > config.accelerometerTiltY);
  uint8_t currentTiltState = config.lastButtonState[config.tiltButton];
  
  if (currentTiltState == 0 && tiltThresholdExceeded) {
    config.lastButtonState[config.tiltButton] = buttons.sendButtonPush(config.tiltButton, 1);
    if (xValue > config.accelerometerTilt) {
      config.lastButtonState[config.tiltButtonRight] = buttons.sendButtonPush(config.tiltButtonRight, 1);
    }
    if (xValue < -config.accelerometerTilt) {
      config.lastButtonState[config.tiltButtonLeft] = buttons.sendButtonPush(config.tiltButtonLeft, 1);
    }
    if (yValue > config.accelerometerTiltY) {
      config.lastButtonState[config.tiltButtonUp] = buttons.sendButtonPush(config.tiltButtonUp, 1);
    }
    if (yValue < -config.accelerometerTiltY) {
      config.lastButtonState[config.tiltButtonRight] = buttons.sendButtonPush(config.tiltButtonRight, 1);
    }

  } else if (currentTiltState == 1 && !tiltThresholdExceeded) {
    config.lastButtonState[config.tiltButton] = buttons.sendButtonPush(config.tiltButton, 0);
    config.lastButtonState[config.tiltButtonRight] = buttons.sendButtonPush(config.tiltButtonRight, 0);
    config.lastButtonState[config.tiltButtonLeft] = buttons.sendButtonPush(config.tiltButtonLeft, 0);
    config.lastButtonState[config.tiltButtonUp] = buttons.sendButtonPush(config.tiltButtonUp, 0);
    config.lastButtonState[config.tiltButtonRight] = buttons.sendButtonPush(config.tiltButtonRight, 0);
    tiltSuppressTime = config.tiltSuppress;
  }
}

void Accelerometer::updateXAxis() {
  if (priorXValue != xValue) {
    Gamepad1.xAxis(static_cast<int16_t>(static_cast<float>(xValue) / localMax * 32767));
    priorXValue = xValue;
    config.updateUSB = true;
  }
}

void Accelerometer::updateYAxis() {
  if (priorYValue != yValue) {
    Gamepad1.yAxis(static_cast<int16_t>(static_cast<float>(yValue) / localMaxY * 32767));
    priorYValue = yValue;
    config.updateUSB = true;
  }
}

void Accelerometer::sendAccelerometerState()
{
  Serial.print(F("A,"));
  Serial.print((getRawAccelValue() - xValueOffset));
  Serial.print(F(","));
  Serial.print((mpu.getY() - yValueOffset));
  Serial.print(F(","));
  Serial.print(xValue);
  Serial.print(F(","));
  Serial.print(yValue);
  Serial.print(F("\r\n"));
}