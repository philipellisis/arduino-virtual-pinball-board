#include "LightShow.h"
#include "Enums.h"
#include "Globals.h"

LightShow::LightShow() {
}

void LightShow::init() {
  for (int i = 0; i < 62; i++) {
    if ((config.toySpecialOption[i] == LIGHT_SHOW_MEDIUM || config.toySpecialOption[i] == LIGHT_SHOW_HIGH) && startLight == 0) {
      startLight = i;
    }
    if ((config.toySpecialOption[i] == LIGHT_SHOW_MEDIUM || config.toySpecialOption[i] == LIGHT_SHOW_HIGH)) {
      finishLight = i + 1;
    }
  }
  lightShowStartTime = config.lightShowTime * 1000L;
}

void LightShow::checkSetLights() {
 //starts in OUTPUT_RECEIVED_RESET_TIMER, but effectively starts in OUTPUT_RECEIVED, then to WAITING_INPUT after 10 seconds
  long int currentTime = millis();

  switch (config.lightShowState)
  {
  case OUTPUT_RECEIVED_RESET_TIMER:
    //an output has been received and waiting to reset timer
    timeInState = currentTime;
    config.lightShowState = OUTPUT_RECEIVED;
    break;
  case INPUT_RECEIVED_SET_LIGHTS_LOW:
    // an input has been received and lights are on high
    setLightsNormal();
    transitionToState(WAITING_INPUT, currentTime);
    break;
  case WAITING_INPUT:
    //here an output has not been received in 10 seconds, and now waiting for button input
    if (currentTime - timeInState > (lightShowStartTime * 2) && config.lightShowAttractEnabled == true) {
      setLightsRandom();
      if (doneSettingLights == true) {
        config.lightShowState = IN_RANDOM_MODE_WAITING_INPUT;
        timeInState = currentTime;
      }
      
    }
    break;
  case INPUT_RECEIVED_SET_LIGHTS_HIGH:
    // means an input has been received and lights are on low
    setLightsHigh();
    
    transitionToState(INPUT_RECEIVED_BUTTON_STILL_PRESSED, currentTime);
    break;
  case OUTPUT_RECEIVED:
    //an output has been received in the last 10 seconds
    if (currentTime - timeInState > lightShowStartTime) {
      setLightsNormal();
      transitionToState(WAITING_INPUT, currentTime);
    }
    break;
  case IN_RANDOM_MODE_WAITING_INPUT:
    //in random mode
    //if (currentTime - timeInState > 10) {
      incrementRandom();
      timeInState = currentTime;
    //}
    break;
  default:
    break;
  }

}

void LightShow::setStartFinishLoops() {
  if (doneSettingLights == true) {
    currentStartLight = startLight;
    doneSettingLights = false;
  } else {
    currentStartLight += 5;
  }
  currentFinishLight = currentStartLight + 5;
  if (currentFinishLight >= finishLight) {
    currentFinishLight = finishLight;
    doneSettingLights = true;
  }
}

void LightShow::setLightsNormal() {

  setStartFinishLoops();

  for (int i = currentStartLight; i < currentFinishLight; i++) {
    if (config.toySpecialOption[i] == LIGHT_SHOW_MEDIUM) {
      outputs.updateOutput(i, 60);
    } else if (config.toySpecialOption[i] == LIGHT_SHOW_HIGH) {
        outputs.updateOutput(i, 255);
    }
  }
}

void LightShow::setLightsHigh() {

  setStartFinishLoops();

  for (int i = currentStartLight; i < currentFinishLight; i++) {
    if (config.toySpecialOption[i] == LIGHT_SHOW_MEDIUM) {
      outputs.updateOutput(i, 255);
    }
  }
}

void LightShow::setLightsOff() {

  for (int i = startLight; i < finishLight; i++) {
    if (isLightShowOutput(i)) {
      outputs.updateOutput(i, 0);
    }
  }
}

void LightShow::setLightsRandom() {

  setStartFinishLoops();

  for (int i = currentStartLight; i < currentFinishLight; i++) {
    if (isLightShowOutput(i)) {
      unsigned char rnd = rand() % 256;
      outputs.updateOutput(i, rnd);
      outputDirection[i] = rnd % 2;
      outputValues[i] = rnd;
    }
  }
}

void LightShow::incrementRandom() {
  setStartFinishLoops();
  for (int i = currentStartLight; i < currentFinishLight; i++) {
    if (isLightShowOutput(i)) {
      if (outputDirection[i] == 1) {
        if (outputValues[i] >= 255 - incrementor) {
          outputDirection[i] = 0;
        } else {
          outputValues[i] += incrementor;
        }
      } else {
        if (outputValues[i] <= incrementor) {
          outputDirection[i] = 1;
        } else {
          outputValues[i] -= incrementor;
        }
      }
      outputs.updateOutput(i, outputValues[i]);
    }
  }
}

void LightShow::transitionToState(unsigned char newState, long currentTime) {
  if (doneSettingLights) {
    timeInState = currentTime;
    config.lightShowState = newState;
  }
}

bool LightShow::isLightShowOutput(int i) {
  return (config.toySpecialOption[i] == LIGHT_SHOW_MEDIUM || config.toySpecialOption[i] == LIGHT_SHOW_HIGH);
}

bool LightShow::isMediumLight(int i) {
  return config.toySpecialOption[i] == LIGHT_SHOW_MEDIUM;
}
