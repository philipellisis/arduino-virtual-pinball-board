#ifndef GLOBALS_H
#define GLOBALS_H
#include "Plunger.h"
#include "Buttons.h"
#include "Accelerometer.h"
#include "Communication.h"
#include "Outputs.h"
#include "Config.h"
#include "LightShow.h"
#include "SPIController.h"

extern  Buttons buttons;


extern  Plunger plunger;
extern  LightShow lightShow;

extern  Accelerometer accel;
extern  Communication comm;
extern  Outputs outputs;
extern  Config config;
extern  SPIController spiController;
#endif