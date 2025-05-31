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

void BluetoothController::error(const __FlashStringHelper* msg) {
    Serial.println(msg);
    while (1);
}

void BluetoothController::init() {
    Serial.println(F("Starting BLE HID Gamepad Controller..."));
    
    setupBLE();
}

void BluetoothController::setupBLE() {
    // 1) Initialize the module
    if (!ble.begin(false)) {
        error(F("Couldn't find Bluefruit module via SPI. Check wiring."));
    }
    Serial.println(F("BLE Module found."));

    // 2) Factory-reset for a clean state (optional after first flash)
    ble.factoryReset();

    // 3) Turn off AT echo to keep replies clean
    ble.echo(false);

    // 4) Set a friendly name
    ble.sendCommandCheckOK("AT+GAPDEVNAME=Pinball Controller");

    // 5) Query & enable BLE HID service if needed
    int32_t hidEnabled = 0;
    ble.sendCommandWithIntReply("AT+BLEHIDEN?", &hidEnabled);
    if (!hidEnabled) {
        Serial.println(F("Enabling BLE HID service..."));
        ble.sendCommandCheckOK("AT+BLEHIDEN=1");
    }

    // 6) Query & enable BLE HID Gamepad profile if needed
    int32_t gpEnabled = 0;
    ble.sendCommandWithIntReply("AT+BLEHIDGAMEPADEN?", &gpEnabled);
    if (!gpEnabled) {
        Serial.println(F("Enabling BLE HID Gamepad profile..."));
        ble.sendCommandCheckOK("AT+BLEHIDGAMEPADEN=1");
    }

    // 7) Reset module to apply any service changes
    Serial.println(F("Resetting BLE module to apply changes..."));
    ble.reset();

    // 8) Wait for a central to connect
    Serial.print(F("Waiting for BLE connection"));
    while (!ble.isConnected()) {
        Serial.print(F("."));
        delay(500);
    }

    Serial.println(F("\nBLE Connected! Switching to DATA mode..."));
    ble.setMode(1); // DATA mode
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
    
    // Build AT command for extended gamepad report
    // Format: AT+BLEHIDGAMEPAD=<x>,<y>,<buttons>,<hat>,<brake>,<accelerator>,<steering>
    // We'll use brake for Z-axis (plunger)
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+BLEHIDGAMEPAD=%d,%d,0x%08lX,0,%d,0,0", 
             xAxis, yAxis, buttonMask, zAxis);
    
    ble.sendCommandCheckOK(cmd);
    
    // Update last known states
    for (int i = 0; i < 32; i++) {
        lastButtonStates[i] = config.lastButtonState[i];
    }
    lastXAxis = xAxis;
    lastYAxis = yAxis;
    lastZAxis = zAxis;
    
    // Debug output can be enabled if needed
    // Serial.print(F("BT Report - Buttons: 0x"));
    // Serial.print(buttonMask, HEX);
    // Serial.print(F(", X: "));
    // Serial.print(xAxis);
    // Serial.print(F(", Y: "));
    // Serial.print(yAxis);
    // Serial.print(F(", Z: "));
    // Serial.println(zAxis);
}