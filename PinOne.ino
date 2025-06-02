#include "MinimalHID.h"
#include <Wire.h>
#include "Globals.h"

Plunger plunger;
Buttons buttons;
LightShow lightShow;
Accelerometer accel;
Communication comm;
Outputs outputs;
Config config;
BluetoothController bluetoothController;

bool DEBUG = false;
unsigned char toggle = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize all components first
  config.init();
  outputs.init();
  buttons.init();
  plunger.init();
  lightShow.init();
  
  if (config.accelerometer > 0) {
    accel.init();
  }
  
  // Initialize Bluetooth controller after all inputs are ready
  //bluetoothController.init();
  
}

void loop() {
  // Handle input processing (same as original)
  if (toggle == 0) {
    plunger.plungerRead();
  } else if (toggle == 1 && config.accelerometerEprom > 0) {
    accel.accelerometerRead();
  } else if (toggle == 2) {
    lightShow.checkSetLights();
  } else if (toggle == 3) {
    comm.communicate();
  }
  
  toggle++;
  if (toggle > 3) {
    toggle = 0;
  }
  
  // Check for button changes
  buttons.checkChanged();
  
  // Update Bluetooth controller with current input states
  //bluetoothController.update();
  
  // Small delay to prevent overwhelming the Bluetooth connection
  //delay(20);  // ~50 Hz updates
}