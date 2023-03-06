#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>


Config::Config() {
  if (DEBUG) {Serial.println("Config: About to initialize");}
}

void Config::init() {
  if (DEBUG) {Serial.println("Config: initializing joystick");}

  // get first 16 bank maximum values
  for (int i = 0; i < 16; i++) {
    EEPROM.get(i, maxOutputState0[i]);
    EEPROM.get(i+16, maxOutputState1[i]);
    EEPROM.get(i+32, maxOutputState2[i]);
  }
  
}

void Config::saveConfig() {
  // save first 16 bank maximum values
  for (int i = 0; i < 16; i++) {
    EEPROM.write(i, maxOutputState0[i]);
    EEPROM.write(i+16, maxOutputState1[i]);
    EEPROM.write(i+32, maxOutputState2[i]);
  }
}
