#include "BluetoothController.h"
#include "Globals.h"

BluetoothController::BluetoothController() : 
    ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST),
    lastXAxis(0),
    lastYAxis(0),
    lastZAxis(0)
{
    // Initialize button states
    for (int i = 0; i < 32; i++) {
        lastButtonStates[i] = false;
    }
}


void BluetoothController::init() {
    //Serial.println(F("Starting BLE HID Gamepad Controller..."));
    
    setupBLE();
}

void BluetoothController::setupBLE() {
    // 1) Initialize the module
    if (!ble.begin(false)) {
        //Serial.println(F("BLE Module not found!!!"));
        return;
    }
        //error(F("Couldn't find Bluefruit module via SPI. Check wiring."));
    
    //Serial.println(F("BLE Module found."));

    // 2) Factory-reset for a clean state (optional after first flash)
    ble.factoryReset();

    // 3) Turn off AT echo to keep replies clean
    ble.echo(false);

    // 4) Set a friendly name
    ble.sendCommandCheckOK("AT+GAPDEVNAME=CSD_Controller");

    // 5) Query & enable BLE HID service if needed
    int32_t hidEnabled = 0;
    ble.sendCommandWithIntReply("AT+BLEHIDEN?", &hidEnabled);
    if (!hidEnabled) {
        //Serial.println(F("Enabling BLE HID service..."));
        ble.sendCommandCheckOK("AT+BLEHIDEN=1");
    }

    // 6) Query & enable BLE HID Gamepad profile if needed
    int32_t gpEnabled = 0;
    ble.sendCommandWithIntReply("AT+BLEHIDGAMEPADEN?", &gpEnabled);
    if (!gpEnabled) {
        //Serial.println(F("Enabling BLE HID Gamepad profile..."));
        ble.sendCommandCheckOK("AT+BLEHIDGAMEPADEN=1");
    }

    // 7) Reset module to apply any service changes
    //Serial.println(F("Resetting BLE module to apply changes..."));
    ble.reset();

    // 8) Wait for a central to connect
    //Serial.print(F("Waiting for BLE connection"));
    while (!ble.isConnected()) {
        Serial.print(F("."));
        delay(500);
    }

    //Serial.println(F("\nBLE Connected! Switching to DATA mode..."));
    ble.setMode(0); // DATA mode
}

bool BluetoothController::isConnected() {
    return ble.isConnected();
}

void BluetoothController::update() {
    if (!isConnected()) {
        return;
    }
    
    // Only send update if inputs have changed
    if (hasInputChanged()) {
        sendGamepadReport();
    }
}

bool BluetoothController::hasInputChanged() {
    // Check button changes
    for (int i = 0; i < 32; i++) {
        if (lastButtonStates[i] != config.lastButtonState[i]) {
            return true;
        }
    }
    
    // Check axis changes
    int16_t currentX = getXAxisValue();
    int16_t currentY = getYAxisValue();
    int8_t currentZ = getZAxisValue();
    
    if (currentX != lastXAxis || currentY != lastYAxis || currentZ != lastZAxis) {
        return true;
    }
    
    return false;
}

uint32_t BluetoothController::getButtonMask() {
    uint32_t buttonMask = 0;
    
    // Pack button states into a 32-bit mask
    for (int i = 0; i < 32; i++) {
        if (config.lastButtonState[i]) {
            buttonMask |= (1UL << i);
        }
    }
    
    return buttonMask;
}

int16_t BluetoothController::getXAxisValue() {
    // Get X-axis value from accelerometer
    if (config.accelerometer > 0) {
        // Use the same calculation as in Accelerometer.cpp:185
        int xValue = accel.getXValue();
        return static_cast<int16_t>(static_cast<float>(xValue) / config.accelerometerMax * 32767);
    }
    return 0;
}

int16_t BluetoothController::getYAxisValue() {
    // Get Y-axis value from accelerometer
    if (config.accelerometer > 0) {
        // Use the same calculation as in Accelerometer.cpp:191
        int yValue = accel.getYValue();
        return static_cast<int16_t>(static_cast<float>(yValue) / config.accelerometerMaxY * 32767);
    }
    return 0;
}

int8_t BluetoothController::getZAxisValue() {
    // Get Z-axis value from plunger
    if (config.enablePlunger) {
        // Plunger already outputs in the correct range
        return static_cast<int8_t>(plunger.getAdjustedValue());
    }
    return 0;
}

void BluetoothController::sendGamepadReport() {
    uint32_t buttonMask = getButtonMask();
    int16_t xAxis = getXAxisValue();
    int16_t yAxis = getYAxisValue();
    int8_t zAxis = getZAxisValue();
    
    // Build AT command using correct 3-parameter format from firmware docs
    // Format: AT+BLEHIDGAMEPAD=x,y,buttons
    // x: -1=LEFT, 0=NONE, 1=RIGHT
    // y: -1=UP, 0=NONE, 1=DOWN  
    // buttons: 0x00-0xFF (8 buttons only, bits 0-7)
    
    // Convert accelerometer values to -1, 0, 1 format
    int8_t gamepadX = 0;
    if (xAxis < -10000) gamepadX = -1;
    else if (xAxis > 10000) gamepadX = 1;
    
    int8_t gamepadY = 0; 
    if (yAxis < -10000) gamepadY = -1;
    else if (yAxis > 10000) gamepadY = 1;
    
    // Limit to 8 buttons (firmware limitation)
    uint8_t buttons8bit = (uint8_t)(buttonMask & 0xFF);
    
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+BLEHIDGAMEPAD=%d,%d,0x%02X", 
             gamepadX, gamepadY, buttons8bit);
    
    ble.sendCommandCheckOK(cmd);
    
    // Update last known states
    for (int i = 0; i < 32; i++) {
        lastButtonStates[i] = config.lastButtonState[i];
    }
    lastXAxis = xAxis;
    lastYAxis = yAxis;
    lastZAxis = zAxis;
    
    // Debug output to verify commands are being sent
    Serial.print(F("BT: "));
    Serial.print(cmd);
    Serial.print(F(" - Raw: X="));
    Serial.print(xAxis);
    Serial.print(F(", Y="));
    Serial.print(yAxis);
    Serial.print(F(", Buttons=0x"));
    Serial.print(buttonMask, HEX);
    Serial.print(F(" -> GP: X="));
    Serial.print(gamepadX);
    Serial.print(F(", Y="));
    Serial.print(gamepadY);
    Serial.print(F(", B8=0x"));
    Serial.println(buttons8bit, HEX);
}