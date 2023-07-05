#include "LightShow.h"
#include "Enums.h"


LightShow::LightShow() {
}

void LightShow::init(Config* config, Outputs* outputs) {
  _config = config;
  _outputs = outputs;
}

void LightShow::checkSetLights() {
 //starts in OUTPUT_RECEIVED_RESET_TIMER, but effectively starts in OUTPUT_RECEIVED, then to WAITING_INPUT after 10 seconds
  long int currentTime = millis();

  //an output has been received and waiting to reset timer
  if (_config->lightShowState == OUTPUT_RECEIVED_RESET_TIMER) {
    timeInState = currentTime;
    //if (DEBUG) {Serial.print(F("DEBUG,output received, resetting timer\r\n"));}
    _config->lightShowState = OUTPUT_RECEIVED;
  }

  // an input has been received and lights are on high
  if (_config->lightShowState == INPUT_RECEIVED_SET_LIGHTS_LOW) {
      setLightsNormal();
      timeInState = currentTime;
      _config->lightShowState = WAITING_INPUT;
  }

  //here an output has not been received in 10 seconds, and now waiting for button input
  if (_config->lightShowState == WAITING_INPUT) {
    if (currentTime - timeInState > 20000) {
      setLightsRandom();
      _config->lightShowState = IN_RANDOM_MODE_WAITING_INPUT;
      //if (DEBUG) {Serial.print(F("DEBUG,Staring light show random\r\n"));}
      timeInState = currentTime;
    }
  }

  // means an input has been received and lights are on low
  if (_config->lightShowState == INPUT_RECEIVED_SET_LIGHTS_HIGH) {
    setLightsHigh();
    //if (DEBUG) {Serial.print(F("DEBUG,button input found, setting lights high\r\n"));}
    timeInState = currentTime;
    _config->lightShowState = INPUT_RECEIVED_BUTTON_STILL_PRESSED;
  }


  //an output has been received in the last 10 seconds
  if (_config->lightShowState == OUTPUT_RECEIVED) {
    if (currentTime - timeInState > 10000) {
      setLightsNormal();
      //if (DEBUG) {Serial.print(F("DEBUG,output received, setting to normal\r\n"));}
      _config->lightShowState = WAITING_INPUT;
    }
  }

  //in random mode
  if (_config->lightShowState == IN_RANDOM_MODE_WAITING_INPUT) {
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
