#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>


Config::Config() {
}

void Config::init() {
  if (DEBUG) {Serial.print(F("DEBUG,Config: initializing\r\n"));}

  byte eepromCheck;
  EEPROM.get(1000, eepromCheck);

  if (eepromCheck == 100) {
    if (DEBUG) {Serial.print(F("DEBUG,eeprom check indicates values are all saved, reading from eeprom\r\n"));}
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.get(i, toySpecialOption[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.get(i + 100, turnOffState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 62; i++) {
      EEPROM.get(i + 200, maxOutputState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.get(i + 300, maxOutputTime[i]);
    }

    plungerMax = readIntFromEEPROM(401);
    plungerMin = readIntFromEEPROM(403);
    plungerMid = readIntFromEEPROM(405);
    
    for (int i = 0; i < 4; i++) {
      EEPROM.get(i + 407, solenoidButtonMap[i]);
    }
    for (int i = 0; i < 4; i++) {
      EEPROM.get(i + 417, solenoidOutputMap[i]);
    }

    EEPROM.get(426, orientation);
    EEPROM.get(427, accelerometer);
    accelerometerEprom = accelerometer;
  }
  
  
}

void Config::saveConfig() {
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.write(i, toySpecialOption[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.write(i + 100, turnOffState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.write(i + 200, maxOutputState[i]);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.write(i + 300, maxOutputTime[i]);
    }
  
    writeIntIntoEEPROM(401, plungerMax);
    writeIntIntoEEPROM(403, plungerMin);
    writeIntIntoEEPROM(405, plungerMid);
    
    for (int i = 0; i < 4; i++) {
      EEPROM.write(i + 407, solenoidButtonMap[i]);
    }
    for (int i = 0; i < 4; i++) {
      EEPROM.write(i + 417, solenoidOutputMap[i]);
    }
    
    EEPROM.write(426, orientation);
    EEPROM.write(427, accelerometer);

    EEPROM.write(1000, 100);
    Serial.print(F("RESPONSE,Config saved into EEPROM\r\n"));
}

void Config::updateConfigFromSerial() {
    done = false;
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      toySpecialOption[i] = blockRead();
      if (done == true) {
        Serial.print(F("RESPONSE,error reading toySpecialOption\r\n"));
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      turnOffState[i] = blockRead();
      if (done == true) {
        Serial.print(F("RESPONSE,error reading turnOffState\r\n"));
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputState[i] = blockRead();
      if (done == true) {
        Serial.print(F("RESPONSE,error reading maxOutputState\r\n"));
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputTime[i] = blockRead();
      if (done == true) {
        Serial.print(F("RESPONSE,error reading maxOutputTime\r\n"));
        return;
      }
    }

    plungerMax = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.print(F("RESPONSE,error reading plungerMax\r\n"));
      return;
    }
    plungerMin = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.print(F("RESPONSE,error reading plungerMin\r\n"));
      return;
    }
    plungerMid = (blockRead() << 8) + blockRead();
    if (done == true) {
      Serial.print(F("RESPONSE,error reading plungerMid\r\n"));
      return;
    }
    for (int i = 0; i < 4; i++) {
      solenoidButtonMap[i] = blockRead();
      if (done == true) {
        Serial.print(F("RESPONSE,error reading solenoidButtonMap\r\n"));
        return;
      }
    }
    for (int i = 0; i < 4; i++) {
      solenoidOutputMap[i] = blockRead();
      if (done == true) {
        Serial.print(F("RESPONSE,error reading solenoidOutputMap\r\n"));
        return;
      }
    }

    orientation = blockRead();
    if (done == true) {
      Serial.print(F("RESPONSE,error reading orientation\r\n"));
      return;
    }

    accelerometer = blockRead();
    Serial.print(F("DEBUG,Config: setting accelerometer"));Serial.print(accelerometer); Serial.print(F("\r\n"));
    if (done == true) {
      Serial.print(F("RESPONSE,error reading accelerometer\r\n"));
      return;
    }
    
    Serial.print(F("RESPONSE,SAVE CONFIG SUCCESS\r\n"));
}

void Config::setPlunger() {
    done = false;
    plungerMax = (blockRead() << 8) + blockRead();
    Serial.print(F("DEBUG,plunger max set to")); Serial.print(plungerMax); Serial.print(F("\r\n"));
    if (done == true) {
      Serial.print(F("RESPONSE,error reading plungerMax\r\n"));
      return;
    }
    plungerMin = (blockRead() << 8) + blockRead();
    Serial.print(F("DEBUG,plunger min set to")); Serial.print(plungerMin); Serial.print(F("\r\n"));
    if (done == true) {
      Serial.print(F("RESPONSE,error reading plungerMin\r\n"));
      return;
    }
    plungerMid = (blockRead() << 8) + blockRead();
    Serial.print(F("DEBUG,plunger mid set to")); Serial.print(plungerMid); Serial.print(F("\r\n"));
    if (done == true) {
      Serial.print(F("RESPONSE,error reading plungerMid\r\n"));
      return;
    }
}

void Config::sendConfig() {
    // get first 62 bank maximum values
    Serial.print(F("CONFIG,"));
    for (int i = 0; i < 63; i++) {
      Serial.print(toySpecialOption[i]);
      Serial.print(F(","));
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      Serial.print(turnOffState[i]);
      Serial.print(F(","));
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      Serial.print(maxOutputState[i]);
      Serial.print(F(","));
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
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
    
    Serial.print(F("END OF DATA\r\n"));
}

byte Config::blockRead() {
    long int t1 = millis();
    long int t2 = millis();
    while ((t2 - t1) < 5000) {
      if (Serial.available() > 0) {
        return Serial.read();
      }
      t2 = millis();
      delay(50);
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
