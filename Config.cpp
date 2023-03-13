#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>


Config::Config() {
}

void Config::init() {
  if (DEBUG) {Serial.println(F("Config: initializing joystick"));}

  // get first 16 bank maximum values
  for (int i = 0; i < 16; i++) {
    EEPROM.get(i, maxOutputState[i]);
  }
  
}

void Config::saveConfig() {
  // save first 16 bank maximum values
  for (int i = 0; i < 16; i++) {
    EEPROM.write(i, maxOutputState[i]);
  }
}
