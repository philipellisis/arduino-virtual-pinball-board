#ifndef ENUMS_H
#define ENUMS_H
enum admin_type {
    BUTTONS = 1,
    OUTPUTS = 2,
    PLUNGER = 3,
    ACCEL = 4,
};

enum eeprom {
  PLUNGER_MIN = 1,
  PLUNGER_MAX = 2,
  LEFT_FLIPPER = 3,
  RIGHT_FLIPPER = 4,
  OUTPUT_X_MAX_ON = 100, // max of 3 expansion boards -- 16, 32, 48
  OUTPUT_X_MAX_TIME = 200, // max of 3 expansion boards -- 16, 32, 48
};
#endif
