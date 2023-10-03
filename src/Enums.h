#ifndef ENUMS_H
#define ENUMS_H
enum admin_type {
    BUTTONS = 1,
    OUTPUTS = 2,
    PLUNGER = 3,
    ACCEL = 4,
    SEND_CONFIG = 5,
    GET_CONFIG = 6,
    SET_PLUNGER = 7,
    OFF = 255,
    CONNECT = 8,
    SAVE_CONFIG = 9,
    SET_ACCEL = 10,
    VERSION = 11,
};

  // 1 means an output has been received and waiting to reset timer
  // 2 means an input has been received and lights are on high
  // 3 means an input is ready -- it will also be here when in random mode if it sits here for too long
  // 4 means an input has been received and lights are on low
  // 5 means an output has been received in the last 10 seconds
  // 6 means it's in random mode and just waiting for the next update
  // 7 means input has been recieved and lights are on high, waiting for button to be depressed
enum light_show_state {
  DISABLED = 0,
  OUTPUT_RECEIVED_RESET_TIMER = 1,
  INPUT_RECEIVED_SET_LIGHTS_LOW = 2,
  WAITING_INPUT = 3,
  INPUT_RECEIVED_SET_LIGHTS_HIGH = 4,
  INPUT_RECEIVED_BUTTON_STILL_PRESSED = 7,
  OUTPUT_RECEIVED = 5,
  IN_RANDOM_MODE_WAITING_INPUT = 6

};
enum output_type {
  NONE = 0,
  NOISY = 1,
  LIGHT_SHOW_MEDIUM = 2,
  LIGHT_SHOW_HIGH = 3,
  SHARED = 4,
};

enum eeprom {
  PLUNGER_MIN = 1,
  PLUNGER_MAX = 2,
  LEFT_FLIPPER = 3,
  RIGHT_FLIPPER = 4,
  OUTPUT_X_MAX_ON = 100, // max of 3 expansion boards -- 16, 32, 48
  OUTPUT_X_MAX_TIME = 200, // max of 3 expansion boards -- 16, 32, 48
};

enum accel_direction {
  BACK = 0,
  RIGHT = 1,
  FORWARD = 2,
  LEFT = 3,
  UP_BACK = 4,
  UP_RIGHT = 5,
  UP_FORWARD = 6,
  UP_LEFT = 7,
};

#endif
