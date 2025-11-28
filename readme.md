# Arduino PinOne Controller

### Description

This is an all in one pinball controller, based on the Arduino framework. It is designed for an arduino micro and also utilizes other chips for communication:
- MPU-6050 for accelerometer data
- PCA9685 for adding up to three expansion boards 16x3 outputs
- 74HC165 shift registers for the 24 button inputs

All of these components are easily purchased, so you can make your own board using them, or you can purchase a board from https://www.clevelandsoftwaredesign.com/pinball-parts

Without any of the above components, you can still use for a plunger input as well as 15 outputs, but the other button inputs will not work without 3 shift registers in place.

The board is also supported in DOF so that you can set it up as a device and allow games like Visual Pinball to send table events for solenoids, lights and other toys you might add to your cabinet.

### Features

- Support for 24 button inputs
- Real plunger support through a potentiometer resistor (measured through analog port A5)
- Full native windows controller support (will work natively in Steam without any special software or drivers)
- Ability to configure maximum output values on any of the PWM outputs
- Ability to configure maximum "on" time for any outputs to save components that might not support 100% duty cycle
- Ability to trigger any output from the button inputs (for example, to allow flipper solenoids to trigger automaticaly when the flipper buttons are pressed, regardless of software support)
- Support for analog nudge data that gets sent to the x/y axis on the Gamepad1

### How to configure and test outputs

You can run the configuration tool here: https://github.com/philipellisis/virtual-pinball-board-configurator to test all the outputs as well as configure the board

### How to build

The arduino IDE will build almost all of the application code for you, but there are a few imports you will need to make

- For the Gamepad1 functionality: https://github.com/NicoHood/HID
- For the communication to the MPU-6050: https://github.com/adafruit/Adafruit_MPU6050
- for the communication to the PCA9685: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

### Can I contribute?

Yes please! I am happy to entertain any contribution that you might have! Feel free to create a PR for me to review