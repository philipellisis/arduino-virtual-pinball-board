#include "Outputs.h"
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "Enums.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);
Outputs::Outputs() {
   for (int index = 0; index < numberOutputs; index++) {
    pinMode(outputList[index], OUTPUT);
   }
  
  //if (DEBUG) {Serial.print(F("DEBUG,Communication: pins initialized\r\n"));}
}

void Outputs::init(Config* config) {
  _config = config;
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(90);  // This is the maximum PWM frequency
  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(90);  // This is the maximum PWM frequency
  pwm2.begin();
  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(90);  // This is the maximum PWM frequency
  Wire.setClock(400000);
  Wire.setWireTimeout(3000, true);

  turnOff();

}

void Outputs::turnOff() {
  for (int i = 0; i < 62; i++) {
    updateOutput(i, 0);
  }
}

void Outputs::updateOutput(byte outputId, byte outputValue) {
  if (outputValue > _config->maxOutputState[outputId]) {
    outputValue = _config->maxOutputState[outputId];
  }
  if (_config->nightMode == true && (_config->toySpecialOption[outputId] == NOISY || _config->toySpecialOption[outputId] == SHARED)) {
    outputValue = 0;
  }
  if (_config->toySpecialOption[outputId] == SHARED && outputValues[outputId] > 0) {
    for (int i = 0; i < 9; i++) {
      if(_config->toySpecialOption[i] == SHARED && outputValues[i] == 0) {
        outputId = i;
        //Serial.print(F("DEBUG,switching to another unused output since this one already enabled\r\n"));
        break;
      }
    }
  }
  if (_config->toySpecialOption[outputId] == SHARED && outputValue == 0) {
    for (int i = 0; i < 9; i++) {
      if(_config->toySpecialOption[i] == SHARED && outputValues[i] > 0) {
        updateOutputInternal(i, 0);
        //Serial.print(F("DEBUG,turning off shared outputs\r\n"));
      }
    }
  } else {
    updateOutputInternal(outputId, outputValue);
  }
}

void Outputs::updateOutputInternal(byte outputId, byte outputValue) {
  //if (DEBUG) {Serial.print(F("DEBUG,output ")); Serial.print(outputId); Serial.print(F(" set to ")); Serial.print(outputValue); Serial.print("\r\n");}
  outputValues[outputId] = outputValue;
  if (outputValue != 0) {
    timeTurnedOn[outputId] = millis();
  }
  if (outputId < 15) {
    if (outputId < 5) {
      analogWrite(outputList[outputId], outputValue);
    } else {
      if (outputValue > 127) {
        digitalWrite(outputList[outputId], HIGH);
      } else {
        digitalWrite(outputList[outputId], LOW);
      }
    }
  } else {
    // if the output is the button board output, then invert the value
    if (outputId < 31) {
      outputValue = 255 - outputValue;
    }
    if (outputValue == 255) {
      updateOutputActual(outputId - 15, 4096, 0);
    } else if (outputValue == 0) {
      updateOutputActual(outputId - 15, 0, 4096);
    } else {
      updateOutputActual(outputId - 15, 1, outputValue * 16);
    }
  }
  

}

void Outputs::checkResetOutputs() {
  long int t1 = millis();
  for(int i = 0; i < 5; i++) {
    if (resetOutputNumber < 62) {
      resetOutputNumber++;
    } else {
      resetOutputNumber = 0;
    }
    if (outputValues[resetOutputNumber] > _config->turnOffState[resetOutputNumber] && _config->maxOutputTime[resetOutputNumber] > 0) {
      //if (DEBUG) {Serial.print(F("DEBUG,Output is turned on, checking if it has been on for too long "));Serial.print(resetOutputNumber); Serial.print(F("\r\n"));}
      if (t1 - timeTurnedOn[resetOutputNumber] >= _config->maxOutputTime[resetOutputNumber] * 100) {
        //if (DEBUG) {Serial.print(F("DEBUG,Output has reached max time, turning off\r\n"));}
        updateOutput(resetOutputNumber, _config->turnOffState[resetOutputNumber]);
      }
    }
  }
}

void Outputs::updateOutputActual(byte outputId, int outputValueStart, int outputValueFinish) {
  if (outputId < 16) {
    pwm.setPWM(outputId, outputValueStart, outputValueFinish);
  }
  else if (outputId >= 16 && outputId < 32) {
    pwm1.setPWM(outputId - 16, outputValueStart, outputValueFinish);
  }
  else if (outputId >= 32 && outputId < 48) {
    pwm2.setPWM(outputId - 32, outputValueStart, outputValueFinish);
  }
}

void Outputs::sendOutputState() {
  Serial.print(F("O,"));
  for (int i = 0; i < 62; i++) {
    Serial.print(outputValues[i]);
    Serial.print(F(","));
  }
  Serial.print(outputValues[62]);
  Serial.print(F("\r\n"));
}
