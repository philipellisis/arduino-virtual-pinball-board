#include "LightShow.h"


LightShow::LightShow() {
}

void LightShow::init(Config* config, Outputs* outputs) {
  _config = config;
  _outputs = outputs;
}

void LightShow::checkSetLights() {
  // 0 is in initial state
  // 1 means an output has been received and waiting to reset timer
  // 2 means an input has been received and lights are on high
  // 3 means an input is ready -- it will also be here when in random mode if it sits here for too long
  // 4 means an input has been received and lights are on low
  // 5 means an output has been received in the last 10 seconds
  // 6 means it's in random mode and just waiting for the next update
  long int currentTime = millis();

  // an input has been received and lights are on high
  if (_config->lightShowState == 2) {
    if (currentTime - timeInState > 500) {
      setLightsNormal();
      //if (DEBUG) {Serial.print(F("DEBUG,no button input found, setting lights back to low\r\n"));}
      _config->lightShowState = 3;
    }
  }

  // means an input has been received and lights are on low
  if (_config->lightShowState == 4) {
    setLightsHigh();
    //if (DEBUG) {Serial.print(F("DEBUG,button input found, setting lights high\r\n"));}
    timeInState = currentTime;
    _config->lightShowState = 2;
  }

  //an output has been received and waiting to reset timer
  if (_config->lightShowState == 1) {
    timeInState = currentTime;
    //if (DEBUG) {Serial.print(F("DEBUG,output received, resetting timer\r\n"));}
    _config->lightShowState = 5;
  }

  //an output has been received in the last 10 seconds
  if (_config->lightShowState == 5) {
    if (currentTime - timeInState > 10000) {
      setLightsNormal();
      //if (DEBUG) {Serial.print(F("DEBUG,output received, setting to normal\r\n"));}
      _config->lightShowState = 3;
    }
  }

  //here an output has not been received in 10 seconds, and now waiting for button input
  if (_config->lightShowState == 3) {
    if (currentTime - timeInState > 10000) {
      setLightsRandom();
      _config->lightShowState = 6;
      //if (DEBUG) {Serial.print(F("DEBUG,Staring light show random\r\n"));}
      timeInState = currentTime;
    }
  }

  //in random mode
  if (_config->lightShowState == 6) {
    if (currentTime - timeInState > 10) {
      incrementRandom();
      timeInState = currentTime;
    }
  }

}

void LightShow::setLightsNormal() {
  for (int i = 0; i < 62; i++) {
    if (_config->toySpecialOption[i] == 2) {
      _outputs->updateOutput(i, 60);
    }
  }
}

void LightShow::setLightsHigh() {
  for (int i = 0; i < 62; i++) {
    if (_config->toySpecialOption[i] == 2) {
      _outputs->updateOutput(i, 255);
    }
  }
}

void LightShow::setLightsRandom() {
  for (int i = 0; i < 62; i++) {
    if (_config->toySpecialOption[i] == 2) {
      _outputs->updateOutput(i, i * 4);
      // TODO use random to set values and direction
      outputDirection[i] = 1;
      outputValues[i] = i*4;
    }
  }
}

void LightShow::incrementRandom() {
  for (int i = 0; i < 62; i++) {
    if (_config->toySpecialOption[i] == 2) {
      if (outputDirection[i] == 1) {
        if (outputValues[i] == 255) {
          outputDirection[i] == 0;
        } else {
          outputValues[i] += 1;
          _outputs->updateOutput(i, outputValues[i]);
        }
      } else {
        if (outputValues[i] == 0) {
          outputDirection[i] == 1;
        } else {
          outputValues[i] -= 1;
          _outputs->updateOutput(i, outputValues[i]);
        }
      }
    }
  }
}
