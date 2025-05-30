#ifndef BLUETOOTH_CONTROLLER_H
#define BLUETOOTH_CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
// Hardware SPI pin definitions
#define BLUEFRUIT_SPI_CS     8
#define BLUEFRUIT_SPI_IRQ    7
#define BLUEFRUIT_SPI_RST    4
#include "Buttons.h"
#include "Plunger.h"
#include "Accelerometer.h"
#include "Config.h"

class BluetoothController {
public:
    BluetoothController();
    void init();
    void update();
    bool isConnected();
    
private:
    Adafruit_BluefruitLE_SPI ble;
    
    // Previous states for change detection
    bool lastButtonStates[32];
    int16_t lastXAxis;
    int16_t lastYAxis;
    int8_t lastZAxis;
    
    void setupBLE();
    void sendGamepadReport();
    void error(const __FlashStringHelper* msg);
    
    // Helper methods for input processing
    uint32_t getButtonMask();
    int16_t getXAxisValue();
    int16_t getYAxisValue();
    int8_t getZAxisValue();
    
    bool hasInputChanged();
};

#endif