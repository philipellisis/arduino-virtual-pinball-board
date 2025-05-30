/**************************************************************************/
/*!
    @file     Adafruit_BLE.cpp
    @author   Minimal implementation for BluetoothController
*/
/**************************************************************************/

#include "Adafruit_BLE.h"

bool Adafruit_BLE::sendCommandCheckOK(const char* cmd) {
    // Simplified implementation - just return true for now
    // In a real implementation, this would send the AT command and check for OK response
    if (_verbose) {
        Serial.print(F("Sending: "));
        Serial.println(cmd);
    }
    delay(100); // Give some time for command processing
    return true;
}

bool Adafruit_BLE::sendCommandWithIntReply(const char* cmd, int32_t* reply) {
    // Simplified implementation - assume commands return 0 (disabled state)
    if (_verbose) {
        Serial.print(F("Sending: "));
        Serial.println(cmd);
    }
    *reply = 0; // Default to disabled state
    delay(100);
    return true;
}

bool Adafruit_BLE::factoryReset() {
    if (_verbose) Serial.println(F("Factory reset"));
    delay(1000); // Simulate reset time
    return true;
}

bool Adafruit_BLE::echo(bool enable) {
    if (_verbose) {
        Serial.print(F("Echo "));
        Serial.println(enable ? F("enabled") : F("disabled"));
    }
    return true;
}

bool Adafruit_BLE::reset() {
    if (_verbose) Serial.println(F("Reset"));
    _reset_started_timestamp = millis();
    delay(1000); // Simulate reset time
    return true;
}

bool Adafruit_BLE::isConnected() {
    // Simplified - assume always connected after reset completes
    return (millis() - _reset_started_timestamp) > 2000;
}