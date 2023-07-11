#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>


Config::Config() {
}

void Config::init() {
  //if (DEBUG) {Serial.print(F("DEBUG,Config: initializing\r\n"));}

  byte eepromCheck;
  EEPROM.get(1000, eepromCheck);

  if (eepromCheck == 101) {
    //if (DEBUG) {Serial.print(F("DEBUG,eeprom check indicates values are all saved, reading from eeprom\r\n"));}
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.get(i, toySpecialOption[i]);
      EEPROM.get(i + 100, turnOffState[i]);
      EEPROM.get(i + 200, maxOutputState[i]);
      EEPROM.get(i + 300, maxOutputTime[i]);
    }

    plungerMax = readIntFromEEPROM(401);
    plungerMin = readIntFromEEPROM(403);
    plungerMid = readIntFromEEPROM(405);
    
    for (int i = 0; i < 4; i++) {
      EEPROM.get(i + 407, solenoidButtonMap[i]);
      EEPROM.get(i + 417, solenoidOutputMap[i]);
    }

    EEPROM.get(426, orientation);
    EEPROM.get(427, accelerometer);
    accelerometerEprom = accelerometer;

    accelerometerMultiplier = readIntFromEEPROM(450);
    accelerometerDeadZone = readIntFromEEPROM(452);

    EEPROM.get(454, plungerButtonPush);

    accelerometerTilt = readIntFromEEPROM(455);
    accelerometerMax = readIntFromEEPROM(457);
  } else {
    //save default config in case it's never been done before
    saveConfig();
  }
  
  
}

void Config::saveConfig() {
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      EEPROM.write(i, toySpecialOption[i]);
      EEPROM.write(i + 100, turnOffState[i]);
      EEPROM.write(i + 200, maxOutputState[i]);
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

    EEPROM.write(454, plungerButtonPush);
    writeIntIntoEEPROM(455, accelerometerTilt);
    writeIntIntoEEPROM(457, accelerometerMax);

    EEPROM.write(1000, 101);
    Serial.print(F("RESPONSE,Config saved into EEPROM\r\n"));
}

void Config::updateConfigFromSerial() {
    done = 0;
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      toySpecialOption[i] = blockRead(1);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      turnOffState[i] = blockRead(2);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputState[i] = blockRead(3);
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputTime[i] = blockRead(4);

    }

    plungerMax = (blockRead(5) << 8) + blockRead(6);
    plungerMin = (blockRead(7) << 8) + blockRead(8);
    plungerMid = (blockRead(9) << 8) + blockRead(10);
    for (int i = 0; i < 4; i++) {
      solenoidButtonMap[i] = blockRead(11);
    }
    for (int i = 0; i < 4; i++) {
      solenoidOutputMap[i] = blockRead(12);
    }

    orientation = blockRead(13);
    accelerometer = blockRead(14);
    accelerometerMultiplier = (blockRead(15) << 8) + blockRead(16);
    accelerometerDeadZone = (blockRead(17) << 8) + blockRead(18);
    plungerButtonPush = blockRead(19);
    accelerometerTilt = (blockRead(20) << 8) + blockRead(21);
    accelerometerMax = (blockRead(22) << 8) + blockRead(23);

    if (done > 0) {
      writeConfigMessage(done);
      return;
    }
    
    Serial.print(F("RESPONSE,SAVE CONFIG SUCCESS\r\n"));
}

void Config::setPlunger() {
    done = 0;
    plungerMax = (blockRead(50) << 8) + blockRead(51);
    plungerMin = (blockRead(52) << 8) + blockRead(53);
    plungerMid = (blockRead(54) << 8) + blockRead(55);
    if (done > 0) {
      writeConfigMessage(done);
      return;
    }


    plungerButtonPush = blockRead(56);
    writeIntIntoEEPROM(401, plungerMax);
    writeIntIntoEEPROM(403, plungerMin);
    writeIntIntoEEPROM(405, plungerMid);
    EEPROM.write(454, plungerButtonPush);
    

    Serial.print(F("RESPONSE,SAVE CONFIG SUCCESS\r\n"));
}

void Config::setAccelerometer() {
    done = 0;
    accelerometerMultiplier = (blockRead(100) << 8) + blockRead(101);
    accelerometerDeadZone = (blockRead(102) << 8) + blockRead(103);
    orientation = blockRead(104);
    accelerometerTilt = (blockRead(105) << 8) + blockRead(106);
    accelerometerMax = (blockRead(107) << 8) + blockRead(108);

    if (done > 0) {
      writeConfigMessage(done);
      return;
    }

    writeIntIntoEEPROM(450, accelerometerMultiplier);
    writeIntIntoEEPROM(452, accelerometerDeadZone);
    EEPROM.write(426, orientation);
    writeIntIntoEEPROM(455, accelerometerTilt);
    writeIntIntoEEPROM(457, accelerometerMax);
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
    Serial.print(plungerButtonPush);
    Serial.print(F(","));
    Serial.print(accelerometerTilt);
    Serial.print(F(","));
    Serial.print(accelerometerMax);
    Serial.print(F(","));
    
    Serial.print(F("END OF DATA\r\n"));
}

byte Config::blockRead(byte errorCode) {
    if (done > 0) {
      return 0;
    }
    long int t1 = millis();
    long int t2 = millis();
    while ((t2 - t1) < 5000) {
      if (Serial.available() > 0) {
        return Serial.read();
      }
      t2 = millis();
      delay(50);
    }
    done = errorCode;
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
