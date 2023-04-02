#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>


Config::Config() {
}

void Config::init() {
  if (DEBUG) {Serial.println(F("Config: initializing joystick"));}

  byte eepromCheck;
  EEPROM.get(1000, eepromCheck);

  if (eepromCheck == 100) {
    if (DEBUG) {Serial.println(F("eeprom check indicates values are all saved, reading from eeprom"));}
    // get first 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.get(i, toySpecialOption[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.get(i + 100, turnOffState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.get(i + 200, maxOutputState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.get(i + 300, maxOutputTime[i]);
    }

    plungerMax = readIntFromEEPROM(401);
    plungerMin = readIntFromEEPROM(403);
    plungerMid = readIntFromEEPROM(405);
    
    for (int i = 0; i < 4; i++) {
      EEPROM.get(i + 406, solenoidButtonMap[i]);
    }
    for (int i = 0; i < 4; i++) {
      EEPROM.get(i + 416, solenoidOutputMap[i]);
    }

    EEPROM.get(426, orientation);
    EEPROM.get(427, accelerometer);
  }
  
  
}

void Config::saveConfig() {
    // get first 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.write(i, toySpecialOption[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.write(i + 100, turnOffState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.write(i + 200, maxOutputState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.write(i + 300, maxOutputTime[i]);
    }
  
    writeIntIntoEEPROM(401, plungerMax);
    writeIntIntoEEPROM(403, plungerMin);
    writeIntIntoEEPROM(405, plungerMid);
    
    for (int i = 0; i < 4; i++) {
      EEPROM.write(i + 406, solenoidButtonMap[i]);
    }
    for (int i = 0; i < 4; i++) {
      EEPROM.write(i + 416, solenoidOutputMap[i]);
    }
    
    EEPROM.write(426, orientation);
    EEPROM.write(427, accelerometer);

    EEPROM.write(1000, 100);
}

void Config::updateConfigFromSerial() {

    // get first 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      toySpecialOption[i] = blockRead();
      if (done == true) {
        Serial.println(F("FAILED,error reading toySpecialOption"));
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      turnOffState[i] = blockRead();
      if (done == true) {
        Serial.println(F("FAILED,error reading turnOffState"));
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      maxOutputState[i] = blockRead();
      if (done == true) {
        Serial.println(F("FAILED,error reading maxOutputState"));
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      maxOutputTime[i] = blockRead();
      if (done == true) {
        Serial.println(F("FAILED,error reading maxOutputTime"));
        return;
      }
    }

    plungerMax = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading plungerMax"));
      return;
    }
    plungerMin = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading plungerMin"));
      return;
    }
    plungerMid = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading plungerMid"));
      return;
    }
    for (int i = 0; i < 4; i++) {
      solenoidButtonMap[i] = blockRead();
      if (done == true) {
        Serial.println(F("FAILED,error reading solenoidButtonMap"));
        return;
      }
    }
    for (int i = 0; i < 4; i++) {
      solenoidOutputMap[i] = blockRead();
      if (done == true) {
        Serial.println(F("FAILED,error reading solenoidOutputMap"));
        return;
      }
    }

    orientation = blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading orientation"));
      return;
    }

    accelerometer = blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading accelerometer"));
      return;
    }
    
    Serial.println(F("SUCCESS"));
}

void Config::setPlunger() {
    plungerMax = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading plungerMax"));
      return;
    }
    plungerMin = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading plungerMin"));
      return;
    }
    plungerMid = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.println(F("FAILED,error reading plungerMid"));
      return;
    }
}

void Config::sendConfig() {
    // get first 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      Serial.print(toySpecialOption[i]);
      Serial.print(F(","));
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      Serial.print(turnOffState[i]);
      Serial.print(F(","));
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      Serial.print(maxOutputState[i]);
      Serial.print(F(","));
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      Serial.print(maxOutputTime[i]);
      Serial.print(F(","));
    }
  
    Serial.print(plungerMax);
    Serial.print(F(","));
    Serial.print(plungerMin);
    Serial.print(F(","));
    Serial.print(plungerMid);
    Serial.print(F(","));
    
    for (int i = 0; i < 4; i++) {
      Serial.print(solenoidButtonMap[i]);
      Serial.print(F(","));
    }
    for (int i = 0; i < 4; i++) {
      Serial.print(solenoidOutputMap[i]);
      Serial.print(F(","));
    }

    Serial.print(orientation);
    Serial.print(F(","));
    Serial.print(accelerometer);
    Serial.print(F(","));
    
    Serial.println(F("END OF DATA"));
}

byte Config::blockRead() {
    long int t1 = millis();
    long int t2 = millis();
    while ((t2 - t1) < 5000) {
      if (Serial.available() > 0) {
        return Serial.read();
      }
      t2 = millis();
    }
    done = true;
    return 0;
}

void Config::writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int Config::readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}
