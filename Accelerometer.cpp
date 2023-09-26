#include "Accelerometer.h"
#include <Arduino.h>
#include <Joystick.h>
#include "MPU6050.h"
#include "Enums.h"


MPU6050 mpu;
Accelerometer::Accelerometer() {
}

void Accelerometer::init(Joystick_* joystick, Config* config) {
  _joystick = joystick;
  _config = config;

  byte count = 0;
  if (!mpu.init()) {
    delay(1000);
  }
  while (!mpu.init()) {
    //if (DEBUG) {Serial.print(F("DEBUG,Failed to find MPU6050 chip\r\n"));}
    delay(100);
    if (count > 10) {
      _config->accelerometer = 0;
      _config->accelerometerEprom = 0;
      break;
    }
    count++;
  }
  if (count > 10) {
    return;
  } else {
    count = 0;
  }

  mpu.config();
  resetAccelerometer();
  //assign accelerometer
  delay(100);
  while (count < 10) {
    mpu.read();
    xValueOffset += mpu.getX();
    yValueOffset += mpu.getY();
    count++;
  }
  xValueOffset = xValueOffset/10;
  yValueOffset = yValueOffset/10;

  
  
  //if (DEBUG) {Serial.print(F("DEBUG,MPU6050 Found!\r\n"));}
}

void Accelerometer::resetAccelerometer() {
  _joystick->setXAxisRange(-_config->accelerometerMax, _config->accelerometerMax);
  _joystick->setYAxisRange(-_config->accelerometerMax, _config->accelerometerMax);

  mpu.setAccelerometerRange(_config->accelerometerSensitivity);
}

void Accelerometer::accelerometerRead() {
  //xValue = 0;
  //yValue = 0;
  //if(mpu.getMotionInterruptStatus()) {
  /* Get new sensor events with the readings */
  mpu.read();

  xValue = floor((mpu.getX() - xValueOffset)*100);
  yValue = floor((mpu.getY() - yValueOffset)*100);
  if (abs(xValue) < _config->accelerometerDeadZone) {
    xValue = 0;
  }
  if (abs(yValue) < _config->accelerometerDeadZone) {
    yValue = 0;
  }
  int temp = xValue;
  if (_config->orientation == RIGHT) {
    xValue = -yValue;
    yValue = temp;
  } else if (_config->orientation == FORWARD) {
    xValue = -xValue;
    yValue = -yValue;
  } else if (_config->orientation == LEFT) {
    xValue = yValue;
    yValue = -temp;
  } else if (_config->orientation == UP_BACK) {
    xValue = -xValue;
    yValue = yValue;
  } else if (_config->orientation == UP_RIGHT) {
    xValue = -yValue;
    yValue = -temp;
  } else if (_config->orientation == UP_FORWARD) {
    xValue = xValue;
    yValue = -yValue;
  } else if (_config->orientation == UP_LEFT) {
    xValue = yValue;
    yValue = temp;
  }
  if(buttonState == 0 && (abs(xValue) > _config->accelerometerTilt || abs(yValue) > _config->accelerometerTilt)) {
    _joystick->setButton(_config->tiltButton, 1);
    buttonState = 1;
  } else if (buttonState == 1 && (abs(xValue) < _config->accelerometerTilt && abs(yValue) < _config->accelerometerTilt)) {
    _joystick->setButton(_config->tiltButton, 0);
    buttonState = 0;
  }

  _joystick->setXAxis(xValue);
  _joystick->setYAxis(yValue);

  //if (DEBUG) {Serial.print(F("DEBUG,AccelX:"));}
  //if (DEBUG) {Serial.print(xValue);}
  //if (DEBUG) {Serial.print(F(","));}
  //if (DEBUG) {Serial.print(F("AccelY:"));}
  //if (DEBUG) {Serial.print(yValue);}
  //if (DEBUG) {Serial.print(F("\r\n"));}
}

void Accelerometer::sendAccelerometerState() {
  Serial.print(F("A,"));
  Serial.print((mpu.getX() - xValueOffset));
  Serial.print(F(","));
  Serial.print((mpu.getY() - yValueOffset));
  Serial.print(F(","));
  Serial.print(xValue);
  Serial.print(F(","));
  Serial.print(yValue);
  Serial.print(F("\r\n"));
  
}
