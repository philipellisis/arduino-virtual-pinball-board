# Bluetooth Controller Module

This module provides Bluetooth HID gamepad functionality for the Arduino Virtual Pinball Board, transmitting all button, plunger, and accelerometer inputs wirelessly.

## Files Added

- `BluetoothController.h` - Header file defining the BluetoothController class
- `BluetoothController.cpp` - Implementation of Bluetooth HID gamepad functionality
- `PinOneBluetooth.ino` - Example main file showing integration with existing code

## Hardware Requirements

- Adafruit Bluefruit LE SPI module
- SPI connections:
  - CS pin: 8
  - IRQ pin: 7
  - RST pin: 4

## Features

- Transmits all 32 button states as a button mask
- Sends accelerometer X/Y axis data (if enabled)
- Sends plunger position as Z-axis data (if enabled)
- Only sends updates when input states change (efficient)
- Follows the same initialization pattern as the original bluetooth.cpp

## Integration

The BluetoothController integrates with the existing codebase by:

1. Using the same global objects (buttons, plunger, accel, config)
2. Reading button states from `config.lastButtonState[]`
3. Getting accelerometer values via new getter methods
4. Getting plunger values via new getter methods
5. Sending combined HID gamepad reports over Bluetooth

## Usage

1. Include the new files in your Arduino project
2. Use `PinOneBluetooth.ino` as your main file instead of `PinOne.ino`
3. The controller will automatically initialize and start sending data once a Bluetooth central device connects

## Modifications Made

To enable access to calculated input values, the following getter methods were added:

- `Accelerometer::getXValue()` - Returns processed X-axis value
- `Accelerometer::getYValue()` - Returns processed Y-axis value  
- `Plunger::getAdjustedValue()` - Returns processed plunger position

These methods provide read-only access to the internally calculated values without exposing private member variables.