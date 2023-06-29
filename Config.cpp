#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>


Config::Config() {
}

void Config::init() {
  //if (DEBUG) {Serial.print(F("DEBUG,Config: initializing\r\n"));}

  byte eepromCheck;
  EEPROM.get(1000, eepromCheck);

  if (eepromCheck == 100) {
    //if (DEBUG) {Serial.print(F("DEBUG,eeprom check indicates values are all saved, reading from eeprom\r\n"));}
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

    accelerometerMultiplier = readIntFromEEPROM(450);
    accelerometerDeadZone = readIntFromEEPROM(452);
  } else {
    //save default config in case it's never been done before
    saveConfig();
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

    writeIntIntoEEPROM(450, accelerometerMultiplier);
    writeIntIntoEEPROM(452, accelerometerDeadZone);

    EEPROM.write(1000, 100);
    Serial.print(F("RESPONSE,Config saved into EEPROM\r\n"));
}

void Config::updateConfigFromSerial() {
    done = false;
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      toySpecialOption[i] = blockRead();
      if (done == true) {
        writeConfigMessage(1);
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      turnOffState[i] = blockRead();
      if (done == true) {
        writeConfigMessage(2);
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputState[i] = blockRead();
      if (done == true) {
        writeConfigMessage(3);
        return;
      }
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputTime[i] = blockRead();
      if (done == true) {
        writeConfigMessage(4);
        return;
      }
    }

    plungerMax = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(5);
      return;
    }
    plungerMin = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(6);
      return;
    }
    plungerMid = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(7);
      return;
    }
    for (int i = 0; i < 4; i++) {
      solenoidButtonMap[i] = blockRead();
      if (done == true) {
        writeConfigMessage(8);
        return;
      }
    }
    for (int i = 0; i < 4; i++) {
      solenoidOutputMap[i] = blockRead();
      if (done == true) {
        writeConfigMessage(9);
        return;
      }
    }

    orientation = blockRead();
    if (done == true) {
      writeConfigMessage(10);
      return;
    }

    accelerometer = blockRead();
    Serial.print(F("DEBUG,Config: setting accelerometer"));Serial.print(accelerometer); Serial.print(F("\r\n"));
    if (done == true) {
      writeConfigMessage(11);
      return;
    }

    accelerometerMultiplier = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(12);
      return;
    }
    accelerometerDeadZone = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(13);
      return;
    }
    
    Serial.print(F("RESPONSE,SAVE CONFIG SUCCESS\r\n"));
}

void Config::setPlunger() {
    done = false;
    plungerMax = (blockRead() << 8) + blockRead();
    Serial.print(F("DEBUG,plunger max set to")); Serial.print(plungerMax); Serial.print(F("\r\n"));
    if (done == true) {
      writeConfigMessage(14);
      return;
    }
    plungerMin = (blockRead() << 8) + blockRead();
    Serial.print(F("DEBUG,plunger min set to")); Serial.print(plungerMin); Serial.print(F("\r\n"));
    if (done == true) {
      writeConfigMessage(15);
      return;
    }
    plungerMid = (blockRead() << 8) + blockRead();
    Serial.print(F("DEBUG,plunger mid set to")); Serial.print(plungerMid); Serial.print(F("\r\n"));
    if (done == true) {
      writeConfigMessage(16);
      return;
    }
    writeIntIntoEEPROM(401, plungerMax);
    writeIntIntoEEPROM(403, plungerMin);
    writeIntIntoEEPROM(405, plungerMid);
    Serial.print(F("RESPONSE,SAVE CONFIG SUCCESS\r\n"));
}

void Config::setAccelerometer() {
    done = false;
    accelerometerMultiplier = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(17);
      return;
    }
    accelerometerDeadZone = (blockRead() << 8) + blockRead();
    if (done == true) {
      writeConfigMessage(18);
      return;
    }
    orientation = blockRead();
    if (done == true) {
      writeConfigMessage(19);
      return;
    }
    writeIntIntoEEPROM(450, accelerometerMultiplier);
    writeIntIntoEEPROM(452, accelerometerDeadZone);
    EEPROM.write(426, orientation);
    Serial.print(F("RESPONSE,SAVE CONFIG SUCCESS\r\n"));
}

void Config::writeConfigMessage(int errorCode) {
  Serial.print(F("RESPONSE,error reading "));
  Serial.print(errorCode);
  Serial.print(F("\r\n"));
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
    Serial.print(accelerometerMultiplier);
    Serial.print(F(","));
    Serial.print(accelerometerDeadZone);
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
