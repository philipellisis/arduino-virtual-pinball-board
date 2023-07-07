#include "LightShow.h"
#include "Enums.h"


LightShow::LightShow() {
}

void LightShow::init(Config* config, Outputs* outputs) {
  _config = config;
  _outputs = outputs;
  for (int i = 0; i < 62; i++) {
    if (_config->toySpecialOption[i] == 2 && startLight == 0) {
      startLight = i;
    }
    if (_config->toySpecialOption[i] == 2) {
      finishLight = i;
    }
  }
}

void LightShow::checkSetLights() {
 //starts in OUTPUT_RECEIVED_RESET_TIMER, but effectively starts in OUTPUT_RECEIVED, then to WAITING_INPUT after 10 seconds
  long int currentTime = millis();

  switch (_config->lightShowState)
  {
  case OUTPUT_RECEIVED_RESET_TIMER:
    //an output has been received and waiting to reset timer
    timeInState = currentTime;
    //if (DEBUG) {Serial.print(F("DEBUG,output received, resetting timer\r\n"));}
    _config->lightShowState = OUTPUT_RECEIVED;
    break;
  case INPUT_RECEIVED_SET_LIGHTS_LOW:
    // an input has been received and lights are on high
    setLightsNormal();
    timeInState = currentTime;
    _config->lightShowState = WAITING_INPUT;
    break;
  case WAITING_INPUT:
    //here an output has not been received in 10 seconds, and now waiting for button input
    if (currentTime - timeInState > 20000) {
      setLightsRandom();
      _config->lightShowState = IN_RANDOM_MODE_WAITING_INPUT;
      //if (DEBUG) {Serial.print(F("DEBUG,Staring light show random\r\n"));}
      timeInState = currentTime;
    }
    break;
  case INPUT_RECEIVED_SET_LIGHTS_HIGH:
    // means an input has been received and lights are on low
    setLightsHigh();
    //if (DEBUG) {Serial.print(F("DEBUG,button input found, setting lights high\r\n"));}
    timeInState = currentTime;
    _config->lightShowState = INPUT_RECEIVED_BUTTON_STILL_PRESSED;
    break;
  case OUTPUT_RECEIVED:
    //an output has been received in the last 10 seconds
    if (currentTime - timeInState > 10000) {
      setLightsNormal();
      //if (DEBUG) {Serial.print(F("DEBUG,output received, setting to normal\r\n"));}
      _config->lightShowState = WAITING_INPUT;
    }
    break;
  case IN_RANDOM_MODE_WAITING_INPUT:
    //in random mode
    if (currentTime - timeInState > 10) {
      //if (DEBUG) {Serial.print(F("DEBUG,Incrementing light show random\r\n"));}
      incrementRandom();
      timeInState = currentTime;
    }
    break;
  default:
    break;
  }

}

void LightShow::setLightsNormal() {
  for (int i = startLight; i < finishLight; i++) {
    if (_config->toySpecialOption[i] == 2) {
      _outputs->updateOutput(i, 60);
    }
  }
}

void LightShow::setLightsHigh() {
  for (int i = startLight; i < finishLight; i++) {
    if (_config->toySpecialOption[i] == 2) {
      _outputs->updateOutput(i, 255);
    }
  }
}

void LightShow::setLightsRandom() {
  for (int i = startLight; i < finishLight; i++) {
    if (_config->toySpecialOption[i] == 2) {
      byte rnd = random(256);
      _outputs->updateOutput(i, rnd);
      outputDirection[i] = rnd % 2;
      outputValues[i] = rnd;
    }
  }
}

void LightShow::incrementRandom() {
  for (int i = startLight; i < finishLight; i++) {
    // if (i == 15) {
    //   Serial.print(F("Output Direction: "));
    //   Serial.print(outputDirection[i]);
    //   Serial.print(F(" Output value: "));
    //   Serial.print(outputValues[i]);
    //   Serial.print(F("\r\n"));
    // }
    if (_config->toySpecialOption[i] == 2) {
      if (outputDirection[i] == 1) {
        if (outputValues[i] >= 255 - incrementor) {
          outputDirection[i] = 0;
        } else {
          outputValues[i] += incrementor;
        }
        _outputs->updateOutput(i, outputValues[i]);
      } else {
        if (outputValues[i] <= incrementor) {
          outputDirection[i] = 1;
        } else {
          outputValues[i] -= incrementor;
        }
        
        _outputs->updateOutput(i, outputValues[i]);
      }
    }
  }
}
