#include "Arduino.h"
#include <Wire.h>

#include "MPU6050.h"

bool MPU6050::init(TwoWire *wire)
{
    if (i2c_dev)
    {
        delete i2c_dev; // remove old interface
    }
    delay(100);
    i2c_dev = new Adafruit_I2CDevice(0x68, wire);

    // For boards with I2C bus power control, may need to delay to allow
    // MPU6050 to come up after initial power.
    bool mpu_found = false;
    mpu_found = i2c_dev->begin();

    if (!mpu_found)
        return false;

    Adafruit_BusIO_Register chip_id = Adafruit_BusIO_Register(i2c_dev, 0x75, 1);

    Adafruit_BusIO_Register power_mgmt_1 = Adafruit_BusIO_Register(i2c_dev, 0x6B, 1);
    Adafruit_BusIO_Register sig_path_reset = Adafruit_BusIO_Register(i2c_dev, 0x68, 1);
    Adafruit_BusIO_RegisterBits device_reset = Adafruit_BusIO_RegisterBits(&power_mgmt_1, 1, 7);

    // see register map page 41
    device_reset.write(1);             // reset
    while (device_reset.read() == 1) { // check for the post reset value
        delay(1);
    }
    delay(100);

    sig_path_reset.write(0x7);

    delay(100);

    config();

    //set accelerometer Range
    setAccelerometerRange(0x00); 

    Adafruit_BusIO_Register power_mgmt_2 = Adafruit_BusIO_Register(i2c_dev, 0x6B, 1);
    power_mgmt_2.write(0x01);

    delay(100);


    return true;
}

void MPU6050::setAccelerometerRange(byte new_range)
{
    Adafruit_BusIO_Register accel_config = Adafruit_BusIO_Register(i2c_dev, 0x1C, 1);
    Adafruit_BusIO_RegisterBits accel_range = Adafruit_BusIO_RegisterBits(&accel_config, 2, 3);
    accelRange = new_range;
    accel_range.write(new_range);
}



void MPU6050::read(void)
{
    // get raw readings
    Adafruit_BusIO_Register data_reg = Adafruit_BusIO_Register(i2c_dev, 0x3B, 14);

    uint8_t buffer[14];
    data_reg.read(buffer, 14);

    rawAccX = buffer[0] << 8 | buffer[1];
    rawAccY = buffer[2] << 8 | buffer[3];
    rawAccZ = buffer[4] << 8 | buffer[5];


    float accel_scale = 1;
    if (accelRange == 3)
        accel_scale = 2048;
    if (accelRange == 2)
        accel_scale = 4096;
    if (accelRange == 1)
        accel_scale = 8192;
    if (accelRange == 0)
        accel_scale = 16384;

    accX = (((float)rawAccX) / accel_scale) * 9.80665F;
    accY = (((float)rawAccY) / accel_scale) * 9.80665F;
    accZ = (((float)rawAccZ) / accel_scale) * 9.80665F;
}

float MPU6050::getX()
{
    return accX;
}

float MPU6050::getY()
{
    return accY;
}

float MPU6050::getZ()
{
    return accZ;
}

void MPU6050::config()
{
    //set sample rate div
    Adafruit_BusIO_Register sample_rate_div = Adafruit_BusIO_Register(i2c_dev, 0x19, 1);
    sample_rate_div.write(0);

    //set filter bandwidth
    Adafruit_BusIO_Register config = Adafruit_BusIO_Register(i2c_dev, 0x1A, 1);
    Adafruit_BusIO_RegisterBits filter_config = Adafruit_BusIO_RegisterBits(&config, 3, 0);
    filter_config.write(0x06);

    //set highpass filter
    Adafruit_BusIO_Register config2 = Adafruit_BusIO_Register(i2c_dev, 0x1C, 1);
    Adafruit_BusIO_RegisterBits filter_config2 = Adafruit_BusIO_RegisterBits(&config2, 3, 0);
    filter_config2.write(0x04);
}