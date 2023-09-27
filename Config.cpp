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
    EEPROM.get(463, shiftButton);
    for (int i = 0; i < 27; i++) {
      EEPROM.get(i + 500, buttonKeyboard[i]);
    }


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
    EEPROM.write(463, shiftButton);
    for (int i = 0; i < 27; i++) {
      EEPROM.write(i + 500, buttonKeyboard[i]);
    }
    EEPROM.write(1000, 101);
    printSuccess();
}

void Config::updateConfigFromSerial() {
    done = 0;
    // get first 62 bank maximum values
    readConfigArray(toySpecialOption, 63);
    // get next 62 bank maximum values
    readConfigArray(turnOffState, 63);
    // get next 62 bank maximum values
    readConfigArray(maxOutputState, 63);
    // get next 62 bank maximum values
    readConfigArray(maxOutputTime, 63);

    plungerMax = readIntFromByte();
    plungerMin = readIntFromByte();
    plungerMid = readIntFromByte();
    readConfigArray(solenoidButtonMap, 4);
    readConfigArray(solenoidOutputMap, 4);

    orientation = blockRead();
    accelerometer = blockRead();
    accelerometerSensitivity = blockRead();
    accelerometerDeadZone = readIntFromByte();
    plungerButtonPush = blockRead();
    accelerometerTilt = readIntFromByte();
    accelerometerMax = readIntFromByte();
    plungerAverageRead = blockRead();
    nightModeButton = blockRead();
    plungerLaunchButton = blockRead();
    tiltButton = blockRead();
    shiftButton = blockRead();
    readConfigArray(buttonKeyboard, 27);

    if (done > 0) {
      printError();
    } else {
      printSuccess();
    }
}

void Config::setPlunger() {
    done = 0;
    plungerMax = readIntFromByte();
    plungerMin = readIntFromByte();
    plungerMid = readIntFromByte();
    plungerButtonPush = blockRead();
    plungerAverageRead = blockRead();
    plungerLaunchButton = blockRead();

    if (done > 0) {
      printError();
      return;
    }

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
    accelerometerDeadZone = readIntFromByte();
    orientation = blockRead();
    accelerometerTilt = readIntFromByte();
    accelerometerMax = readIntFromByte();
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
    Serial.print(F("C,"));

    printConfigArray(toySpecialOption, 63);
    printConfigArray(turnOffState, 63);
    printConfigArray(maxOutputState, 63);
    printConfigArray(maxOutputTime, 63);
  
    printIntComma(plungerMax);
    printIntComma(plungerMin);
    printIntComma(plungerMid);
    
    printConfigArray(solenoidButtonMap, 4);
    printConfigArray(solenoidOutputMap, 4);

    printComma(orientation);
    printComma(accelerometer);
    printComma(accelerometerSensitivity);
    printIntComma(accelerometerDeadZone);
    printComma(plungerButtonPush);
    printIntComma(accelerometerTilt);
    printIntComma(accelerometerMax);
    printComma(plungerAverageRead);
    printComma(nightModeButton);
    printComma(plungerLaunchButton);
    printComma(tiltButton);
    printComma(shiftButton);
    printConfigArray(buttonKeyboard, 27);
    Serial.print(F("E\r\n"));
}

void Config::printComma(byte value) {
  Serial.print(value);
  Serial.print(F(","));
}

void Config::printIntComma(int value) {
  Serial.print(value);
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

int Config::readIntFromByte() {
  return (blockRead() << 8) + blockRead();
}

void Config::readConfigArray(byte* configArray, byte size) {
  for (byte i = 0; i < size; i++) {
    configArray[i] = blockRead();
  }
}

void Config::printConfigArray(byte* configArray, byte size) {
  for (byte i = 0; i < size; i++) {
    printComma(configArray[i]);
  }
}
