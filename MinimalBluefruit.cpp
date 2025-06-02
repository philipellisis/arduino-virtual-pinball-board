#include "MinimalBluefruit.h"

MinimalBluefruit::MinimalBluefruit(int8_t csPin, int8_t irqPin, int8_t rstPin) :
    m_cs_pin(csPin), m_irq_pin(irqPin), m_rst_pin(rstPin),
    m_connected(false), m_verbose(false), m_reset_time(0) {
}

bool MinimalBluefruit::begin(bool verbose) {
    m_verbose = verbose;
    
    // Initialize pins
    pinMode(m_cs_pin, OUTPUT);
    digitalWrite(m_cs_pin, HIGH);
    pinMode(m_irq_pin, INPUT);
    
    if (m_rst_pin >= 0) {
        pinMode(m_rst_pin, OUTPUT);
        digitalWrite(m_rst_pin, HIGH);
    }
    
    // Initialize SPI
    SPI.begin();
    
    //if (m_verbose) Serial.println(F("MinimalBluefruit initialized"));
    return true;
}

bool MinimalBluefruit::factoryReset() {
    //if (m_verbose) Serial.println(F("Factory reset"));
    
    // Hardware reset if available
    if (m_rst_pin >= 0) {
        digitalWrite(m_rst_pin, LOW);
        delay(10);
        digitalWrite(m_rst_pin, HIGH);
        delay(1000);
    }
    
    // Send AT+FACTORYRESET command
    return sendATCommand("AT+FACTORYRESET");
}

bool MinimalBluefruit::echo(bool enable) {
    // if (m_verbose) {
    //     Serial.print(F("Echo "));
    //     Serial.println(enable ? F("on") : F("off"));
    // }
    return sendATCommand(enable ? "ATE1" : "ATE0");
}

bool MinimalBluefruit::sendCommandCheckOK(const char* cmd) {
    // if (m_verbose) {
    //     Serial.print(F("Sending: "));
    //     Serial.println(cmd);
    // }
    return sendATCommand(cmd);
}

bool MinimalBluefruit::sendCommandWithIntReply(const char* cmd, int32_t* reply) {
    // if (m_verbose) {
    //     Serial.print(F("Sending: "));
    //     Serial.println(cmd);
    // }
    
    // For simplicity, assume query commands return 0 initially
    *reply = 0;
    
    // Send command and assume success
    return sendATCommand(cmd);
}

bool MinimalBluefruit::reset() {
    //if (m_verbose) Serial.println(F("Reset"));
    
    m_reset_time = millis();
    m_connected = false;
    
    // Hardware reset
    if (m_rst_pin >= 0) {
        digitalWrite(m_rst_pin, LOW);
        delay(10);
        digitalWrite(m_rst_pin, HIGH);
    }
    
    delay(1000); // Wait for reset to complete
    return true;
}

bool MinimalBluefruit::isConnected() {
    // Simulate connection after reset delay
    if (millis() - m_reset_time > 2000) {
        m_connected = true;
    }
    return m_connected;
}

bool MinimalBluefruit::setMode(uint8_t mode) {
    // if (m_verbose) {
    //     Serial.print(F("Set mode: "));
    //     Serial.println(mode);
    // }
    // Mode switching not needed for this implementation
    return true;
}

bool MinimalBluefruit::sendATCommand(const char* cmd) {
    // Simplified AT command sending
    // In a real implementation, this would use SPI protocol
    
    spiInit();
    
    // Simulate command transmission
    delay(50);
    
    spiEnd();
    
    return waitForOK();
}

bool MinimalBluefruit::waitForOK() {
    // Simplified response waiting
    delay(100);
    return true; // Assume all commands succeed
}

void MinimalBluefruit::spiInit() {
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(m_cs_pin, LOW);
}

void MinimalBluefruit::spiEnd() {
    digitalWrite(m_cs_pin, HIGH);
    SPI.endTransaction();
}