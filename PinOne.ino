#include "MinimalHID.h"
#include <Wire.h>
#include "Globals.h"
#include "SPIController.h"

Plunger plunger;
Buttons buttons;
LightShow lightShow;
Accelerometer accel;
Communication comm;
Outputs outputs;
Config config;
SPIController spiController;

unsigned char toggle = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize all components first
  config.init();
  config.debug = false;
  outputs.init();
  buttons.init();
  plunger.init();
  lightShow.init();
  
  if (config.accelerometer > 0) {
    accel.init();
  }
  
  // Initialize SPI controller after all inputs are ready
  if (config.bluetoothEnable) {
    spiController.init();
  }
  
}

void loop() {
  // Handle input processing (same as original)
  //unsigned long t1 = micros();
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


  // unsigned long t2 = micros();
  // if (config.debug) {
  //   Serial.print(F("DEBUG,Time taken by task: "));
  //   Serial.print(toggle);
  //   Serial.print(F(", "));
  //   Serial.print(t2 - t1);
  //   Serial.println(F(" microseconds"));
  // }
  

  
  // Check for button changes
  buttons.checkChanged();
  if (config.updateUSB || buttons.numberButtonsPressed > 0) {
    buttons.readInputs();
    Gamepad1.write();
    config.updateUSB = false;
    config.buttonPressed = false;
  }

  if (config.bluetoothEnable) {
    // Always call update() — sends a packet when inputs change OR every 100 ms
    // so back-channel output commands are picked up even during idle play.
    spiController.update();

    // Apply any DOF output packet forwarded from the ESP32 via the SPI back-channel.
    if (spiController.hasOutputPacket()) {
      spiController.applyOutputPacket(outputs);
    }
  }
}