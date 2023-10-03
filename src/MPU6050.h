#ifndef MPU6050_H
#define MPU6050_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class MPU6050 {
  
  public:

    bool init(TwoWire *wire = &Wire);
    void setAccelerometerRange(byte);
    void read();
    float getX();
    float getY();
    float getZ();
    void config();

  private:
    float accX,accY, accZ;
    int16_t rawAccX, rawAccY, rawAccZ;
    int accelRange = 0;
    Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
    
};


#endif