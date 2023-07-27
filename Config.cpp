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

    EEPROM.get(450, accelerometerSensitivity);
    accelerometerDeadZone = readIntFromEEPROM(452);

    EEPROM.get(454, plungerButtonPush);

    accelerometerTilt = readIntFromEEPROM(455);
    accelerometerMax = readIntFromEEPROM(457);

    EEPROM.get(459, plungerAverageRead);
    EEPROM.get(460, nightModeButton);
    EEPROM.get(461, plungerLaunchButton);
    EEPROM.get(462, tiltButton);

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

    EEPROM.write(450, accelerometerSensitivity);
    writeIntIntoEEPROM(452, accelerometerDeadZone);

    EEPROM.write(454, plungerButtonPush);
    writeIntIntoEEPROM(455, accelerometerTilt);
    writeIntIntoEEPROM(457, accelerometerMax);

    EEPROM.write(459, plungerAverageRead);
    EEPROM.write(460, nightModeButton);
    EEPROM.write(461, plungerLaunchButton);
    EEPROM.write(462, tiltButton);

    EEPROM.write(1000, 101);
    printSuccess();
}

void Config::updateConfigFromSerial() {
    done = 0;
    // get first 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      toySpecialOption[i] = blockRead();
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      turnOffState[i] = blockRead();
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputState[i] = blockRead();
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      maxOutputTime[i] = blockRead();

    }

    plungerMax = (blockRead() << 8) + blockRead();
    plungerMin = (blockRead() << 8) + blockRead();
    plungerMid = (blockRead() << 8) + blockRead();
    for (int i = 0; i < 4; i++) {
      solenoidButtonMap[i] = blockRead();
    }
    for (int i = 0; i < 4; i++) {
      solenoidOutputMap[i] = blockRead();
    }

    orientation = blockRead();
    accelerometer = blockRead();
    accelerometerSensitivity = blockRead();
    accelerometerDeadZone = (blockRead() << 8) + blockRead();
    plungerButtonPush = blockRead();
    accelerometerTilt = (blockRead() << 8) + blockRead();
    accelerometerMax = (blockRead() << 8) + blockRead();
    plungerAverageRead = blockRead();
    nightModeButton = blockRead();
    plungerLaunchButton = blockRead();
    tiltButton = blockRead();

    if (done > 0) {
      printError();
      return;
    }
    
    printSuccess();
}

void Config::setPlunger() {
    done = 0;
    plungerMax = (blockRead() << 8) + blockRead();
    plungerMin = (blockRead() << 8) + blockRead();
    plungerMid = (blockRead() << 8) + blockRead();
    if (done > 0) {
      printError();
      return;
    }


    plungerButtonPush = blockRead();

    plungerAverageRead = blockRead();
    plungerLaunchButton = blockRead();

    writeIntIntoEEPROM(401, plungerMax);
    writeIntIntoEEPROM(403, plungerMin);
    writeIntIntoEEPROM(405, plungerMid);
    EEPROM.write(454, plungerButtonPush);
    EEPROM.write(459, plungerAverageRead);
    EEPROM.write(461, plungerLaunchButton);
    

    printSuccess();
}

void Config::setAccelerometer() {
    done = 0;
    accelerometerSensitivity = blockRead();
    accelerometerDeadZone = (blockRead() << 8) + blockRead();
    orientation = blockRead();
    accelerometerTilt = (blockRead() << 8) + blockRead();
    accelerometerMax = (blockRead() << 8) + blockRead();
    tiltButton = blockRead();

    if (done > 0) {
      printError();
      return;
    }

    EEPROM.write(450, accelerometerSensitivity);
    writeIntIntoEEPROM(452, accelerometerDeadZone);
    EEPROM.write(426, orientation);
    writeIntIntoEEPROM(455, accelerometerTilt);
    writeIntIntoEEPROM(457, accelerometerMax);
    EEPROM.write(462, tiltButton);
    printSuccess();
}

void Config::printError() {
  Serial.print(F("R,E\r\n"));
}

void Config::sendConfig() {
    // get first 62 bank maximum values
    Serial.print(F("C,"));
    for (int i = 0; i < 63; i++) {
      Serial.print(toySpecialOption[i]);
      printComma();
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      Serial.print(turnOffState[i]);
      printComma();
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      Serial.print(maxOutputState[i]);
      printComma();
    }
  
    // get next 62 bank maximum values
    for (int i = 0; i < 63; i++) {
      Serial.print(maxOutputTime[i]);
      printComma();
    }
  
    Serial.print(plungerMax);
    printComma();
    Serial.print(plungerMin);
    printComma();
    Serial.print(plungerMid);
    printComma();
    
    for (int i = 0; i < 4; i++) {
      Serial.print(solenoidButtonMap[i]);
      printComma();
    }
    for (int i = 0; i < 4; i++) {
      Serial.print(solenoidOutputMap[i]);
      printComma();
    }

    Serial.print(orientation);
    printComma();
    Serial.print(accelerometer);
    printComma();
    Serial.print(accelerometerSensitivity);
    printComma();
    Serial.print(accelerometerDeadZone);
    printComma();
    Serial.print(plungerButtonPush);
    printComma();
    Serial.print(accelerometerTilt);
    printComma();
    Serial.print(accelerometerMax);
    printComma();
    Serial.print(plungerAverageRead);
    printComma();
    Serial.print(nightModeButton);
    printComma();
    Serial.print(plungerLaunchButton);
    printComma();
    Serial.print(tiltButton);
    printComma();
    
    Serial.print(F("E\r\n"));
}

void Config::printComma() {
  Serial.print(F(","));
}

void Config::printSuccess() {
  Serial.print(F("R,S\r\n"));
}

byte Config::blockRead() {
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
    done = 1;
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
