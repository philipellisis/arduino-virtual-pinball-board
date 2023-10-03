#include "Accelerometer.h"
#include <Arduino.h>
#include <Joystick.h>
#include "MPU6050.h"
#include "Enums.h"

MPU6050 mpu;
Accelerometer::Accelerometer()
{
}

void Accelerometer::init(Joystick_ *joystick, Config *config)
{
  _joystick = joystick;
  _config = config;

  byte count = 0;
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
      _config->accelerometer = 0;
      _config->accelerometerEprom = 0;
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

  byte count = 0;
  xValueOffset = 0;
  yValueOffset = 0;
  while (count < 50)
  {
    mpu.read();
    if (_config->orientation > 7) {
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
  if (_config->orientation > 7) {
    orientation = _config->orientation - 8;
  } else {
    orientation = _config->orientation;
  }
  _joystick->setXAxisRange(-_config->accelerometerMax, _config->accelerometerMax);
  _joystick->setYAxisRange(-_config->accelerometerMax, _config->accelerometerMax);

  mpu.setAccelerometerRange(_config->accelerometerSensitivity);
  centerAccelerometer();
}

void Accelerometer::accelerometerRead()
{
  if (_config->lightShowState == IN_RANDOM_MODE_WAITING_INPUT)
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

  if (_config->orientation > 7) {
    xValue = floor((mpu.getZ() - xValueOffset) * 100);
  } else {
    xValue = floor((mpu.getX() - xValueOffset) * 100);
  }
  yValue = floor((mpu.getY() - yValueOffset) * 100);
  
  if (abs(xValue) < _config->accelerometerDeadZone || _config->restingStateCounter != 200)
  {
    xValue = 0;
  }
  if (abs(yValue) < _config->accelerometerDeadZone || _config->restingStateCounter != 200)
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
  if (buttonState == 0 && (abs(xValue) > _config->accelerometerTilt || abs(yValue) > _config->accelerometerTilt))
  {
    _joystick->setButton(_config->tiltButton, 1);
    buttonState = 1;
  }
  else if (buttonState == 1 && (abs(xValue) < _config->accelerometerTilt && abs(yValue) < _config->accelerometerTilt))
  {
    _joystick->setButton(_config->tiltButton, 0);
    buttonState = 0;
  }

  if (priorXValue != xValue) {
    _joystick->setXAxis(xValue);
    priorXValue = xValue;
  }

  if (priorYValue != yValue) {
    _joystick->setYAxis(yValue);
    priorYValue = yValue;
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
  if (_config->orientation > 7) {
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
