#include "Config.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Globals.h"


Config::Config() {
}

void Config::init() {

  uint8_t eepromCheck;
  EEPROM.get(1000, eepromCheck);

  if (eepromCheck == 101) {
    // get first 62 bank maximum values
    loadEEPROMArray(toySpecialOption, 0, 63);
    loadEEPROMArray(turnOffState, 100, 63);
    loadEEPROMArray(maxOutputState, 200, 63);
    loadEEPROMArray(maxOutputTime, 300, 63);

    plungerMax = readIntFromEEPROM(401);
    plungerMin = readIntFromEEPROM(403);
    plungerMid = readIntFromEEPROM(405);
    
    loadEEPROMArray(solenoidButtonMap, 407, 4);
    loadEEPROMArray(solenoidOutputMap, 417, 4);

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

    EEPROM.get(464, disableAccelOnPlungerMove);
    EEPROM.get(465, enablePlungerQuickRelease);
    EEPROM.get(466, disablePlungerWhenNotInUse);
    EEPROM.get(467, disableButtonPressWhenKeyboardEnabled);

    accelerometerTiltY = readIntFromEEPROM(468);
    accelerometerMaxY = readIntFromEEPROM(470);

    loadEEPROMArray(buttonKeyboard, 500, 32);

    loadEEPROMArray(buttonKeyDebounce, 532, 24);
    EEPROM.get(564, buttonDebounceCounter);
    EEPROM.get(565, enablePlunger);
    

    EEPROM.get(566, tiltSuppress);
    EEPROM.get(567, lightShowAttractEnabled);

    EEPROM.get(568, lightShowTime);
    EEPROM.get(569, reverseButtonOutputPolarity);
    EEPROM.get(570, disableUSBSuspend);
    EEPROM.get(571, bluetoothEnable);
    EEPROM.get(572, debug);
    loadEEPROMArray(buttonRemap, 573, 32);
    EEPROM.get(605, tiltButtonUp);
    EEPROM.get(606, tiltButtonDown);
    EEPROM.get(607, tiltButtonLeft);
    EEPROM.get(608, tiltButtonRight);

    EEPROM.get(609, irOutputPin);

  } else {
    //save default config in case it's never been done before
    saveConfig();
  }
  
  
}

void Config::saveConfig() {
    // get first 62 bank maximum values
    saveEEPROMArray(toySpecialOption, 0, 63);
    saveEEPROMArray(turnOffState, 100, 63);
    saveEEPROMArray(maxOutputState, 200, 63);
    saveEEPROMArray(maxOutputTime, 300, 63);
  
    writeIntIntoEEPROM(401, plungerMax);
    writeIntIntoEEPROM(403, plungerMin);
    writeIntIntoEEPROM(405, plungerMid);
    
    saveEEPROMArray(solenoidButtonMap, 407, 4);
    saveEEPROMArray(solenoidOutputMap, 417, 4);
    
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

    EEPROM.write(464, disableAccelOnPlungerMove);
    EEPROM.write(465, enablePlungerQuickRelease);
    EEPROM.write(466, disablePlungerWhenNotInUse);
    EEPROM.write(467, disableButtonPressWhenKeyboardEnabled);

    writeIntIntoEEPROM(468, accelerometerTiltY);
    writeIntIntoEEPROM(470, accelerometerMaxY);

    saveEEPROMArray(buttonKeyboard, 500, 32);

    saveEEPROMArray(buttonKeyDebounce, 532, 24);

    EEPROM.write(564, buttonDebounceCounter);
    EEPROM.write(565, enablePlunger);
    

    EEPROM.write(566, tiltSuppress);
    EEPROM.write(567, lightShowAttractEnabled);

    EEPROM.write(568, lightShowTime);
    EEPROM.write(569, reverseButtonOutputPolarity);
    EEPROM.write(570, disableUSBSuspend);
    EEPROM.write(571, bluetoothEnable);
    EEPROM.write(572, debug);
    saveEEPROMArray(buttonRemap, 573, 32);
    EEPROM.write(605, tiltButtonUp);
    EEPROM.write(606, tiltButtonDown);
    EEPROM.write(607, tiltButtonLeft);
    EEPROM.write(608, tiltButtonRight);

    EEPROM.write(609, irOutputPin);

    EEPROM.write(1000, 101);
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

    readConfigArray(buttonKeyboard, 32);

    disableAccelOnPlungerMove = blockRead();
    enablePlungerQuickRelease = blockRead();
    disablePlungerWhenNotInUse = blockRead();
    disableButtonPressWhenKeyboardEnabled = blockRead();
    accelerometerTiltY = readIntFromByte();
    accelerometerMaxY = readIntFromByte();


    readConfigArray(buttonKeyDebounce, 24);
    buttonDebounceCounter = blockRead();
    enablePlunger = blockRead();
    

    tiltSuppress = blockRead();
    lightShowAttractEnabled = blockRead();
    lightShowTime = blockRead();
    reverseButtonOutputPolarity = blockRead();
    disableUSBSuspend = blockRead();
    bluetoothEnable = blockRead();
    debug = blockRead();
    readConfigArray(buttonRemap, 32);
    tiltButtonUp = blockRead();
    tiltButtonDown = blockRead();
    tiltButtonLeft = blockRead();
    tiltButtonRight = blockRead();

    irOutputPin = blockRead();

    // Read IR command data (1 command, 361 bytes) and save directly to EEPROM
    // Using temporary buffer to avoid keeping 361 bytes in RAM
    for (uint16_t i = 0; i < 361; i++) {
      unsigned char irByte = blockRead();
      EEPROM.write(610 + i, irByte);
    }

    if(blockRead() != 42) {
      done = 1;
    }

    if (done > 0) {
      printError();
    } else {
      saveConfig();
      printSuccess();
    }
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
    printConfigArray(buttonKeyboard, 32);

    printComma(disableAccelOnPlungerMove);
    printComma(enablePlungerQuickRelease);
    printComma(disablePlungerWhenNotInUse);
    printComma(disableButtonPressWhenKeyboardEnabled);
    printIntComma(accelerometerTiltY);
    printIntComma(accelerometerMaxY);

    printConfigArray(buttonKeyDebounce, 24);
    printComma(buttonDebounceCounter);
    printComma(enablePlunger);
    
    printComma(tiltSuppress);
    printComma(lightShowAttractEnabled);
    printComma(lightShowTime);
    printComma(reverseButtonOutputPolarity);
    printComma(disableUSBSuspend);
    printComma(bluetoothEnable);
    printComma(debug);
    printConfigArray(buttonRemap, 32);
    printComma(tiltButtonUp);
    printComma(tiltButtonDown);
    printComma(tiltButtonLeft);
    printComma(tiltButtonRight);

    printComma(irOutputPin);

    // Send IR command data (1 command, 361 bytes) from EEPROM
    for (uint16_t i = 0; i < 361; i++) {
      printComma(EEPROM.read(610 + i));
    }

    Serial.print(F("E\r\n"));
}

void Config::printComma(unsigned char value) {
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

unsigned char Config::blockRead() {
    if (done > 0) {
      return 0;
    }
    uint32_t t1 = millis();
    uint32_t t2 = millis();
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

void Config::writeIntIntoEEPROM(uint16_t address, int16_t number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int16_t Config::readIntFromEEPROM(uint16_t address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

int16_t Config::readIntFromByte() {
  return (blockRead() << 8) + blockRead();
}

void Config::readConfigArray(unsigned char* configArray, unsigned char size) {
  for (uint8_t i = 0; i < size; i++) {
    configArray[i] = blockRead();
  }
}

void Config::loadEEPROMArray(unsigned char* configArray, uint16_t address, uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    EEPROM.get(address + i, configArray[i]);
  }
}

void Config::saveEEPROMArray(unsigned char* configArray, uint16_t address, uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    EEPROM.write(address + i, configArray[i]);
  }
}

void Config::printConfigArray(unsigned char* configArray, unsigned char size) {
  for (uint8_t i = 0; i < size; i++) {
    printComma(configArray[i]);
  }
}