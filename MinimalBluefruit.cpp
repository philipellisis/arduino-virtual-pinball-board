#include "MinimalBluefruit.h"
#include <string.h>

// All constants and types are defined in header file

MinimalBluefruit::MinimalBluefruit(int8_t csPin, int8_t irqPin, int8_t rstPin) :
    m_cs_pin(csPin), m_irq_pin(irqPin), m_rst_pin(rstPin),
    m_connected(false), m_verbose(false), m_reset_time(0), m_mode(BLUEFRUIT_MODE_COMMAND), m_timeout(BLE_DEFAULT_TIMEOUT) {
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
        delay(10); // Ensure reset pin is stable
    }
    
    // Initialize SPI
    SPI.begin();
    
    // Hardware reset to ensure clean state (matches Adafruit sequence)
    if (m_rst_pin >= 0) {
        digitalWrite(m_rst_pin, HIGH);  // Ensure high first
        digitalWrite(m_rst_pin, LOW);   // Pull to GND for 10ms
        delay(10);
        digitalWrite(m_rst_pin, HIGH);  // Release reset
        // Don't delay here - let the initialize pattern + 1s delay handle timing
    }
    
    // Send initialize pattern to reset the Bluefruit module  
    sendSDEPPacket(SDEP_CMDTYPE_INITIALIZE, NULL, 0, 0);
    
    // Bluefruit takes 1 second to reboot (matches Adafruit blocking behavior)
    delay(1000);
    
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
        delay(1000); // Wait for module to fully boot
    }
    
    // Send initialize pattern first to reset the module
    sendSDEPPacket(SDEP_CMDTYPE_INITIALIZE, NULL, 0, 0);
    delay(100);
    
    // Send AT+FACTORYRESET command
    bool result = sendATCommand("AT+FACTORYRESET");
    
    if (result) {
        delay(2000); // Factory reset takes time
        m_reset_time = millis();
        m_connected = false;
    }
    
    return result;
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
    
    bool result;
    uint8_t current_mode = m_mode;
    
    // Switch to COMMAND mode if necessary to execute AT command
    if (current_mode == BLUEFRUIT_MODE_DATA) {
        setMode(BLUEFRUIT_MODE_COMMAND);
    }
    
    // Execute the AT command
    result = sendATCommand(cmd);
    
    // Switch back to original mode if necessary
    if (current_mode == BLUEFRUIT_MODE_DATA) {
        setMode(BLUEFRUIT_MODE_DATA);
    }
    
    return result;
}

bool MinimalBluefruit::sendCommandWithIntReply(const char* cmd, int32_t* reply) {
    // if (m_verbose) {
    //     Serial.print(F("Sending: "));
    //     Serial.println(cmd);
    // }
    
    if (!cmd || !reply) return false;
    
    bool result;
    uint8_t current_mode = m_mode;
    
    // Switch to COMMAND mode if necessary to execute AT command
    if (current_mode == BLUEFRUIT_MODE_DATA) {
        setMode(BLUEFRUIT_MODE_COMMAND);
    }
    
    size_t cmd_len = strlen(cmd);
    if (cmd_len == 0) {
        result = false;
        goto cleanup;
    }
    
    // Send command in SDEP packets
    size_t bytes_sent = 0;
    while (bytes_sent < cmd_len) {
        size_t chunk_size = (cmd_len - bytes_sent < SDEP_MAX_PACKETSIZE) ? (cmd_len - bytes_sent) : SDEP_MAX_PACKETSIZE;
        uint8_t more_data = (bytes_sent + chunk_size < cmd_len) ? 1 : 0;
        
        if (!sendSDEPPacket(SDEP_CMDTYPE_AT_WRAPPER, 
                           (const uint8_t*)(cmd + bytes_sent), 
                           chunk_size, more_data)) {
            result = false;
            goto cleanup;
        }
        
        bytes_sent += chunk_size;
    }
    
    // Wait for and parse response
    unsigned long timeout = millis() + 1000;
    
    while (!digitalRead(m_irq_pin) && millis() < timeout) {
        delay(10);
    }
    
    if (millis() >= timeout) {
        result = false;
        goto cleanup;
    }
    
    // Read response packet
    sdep_packet_t response;
    if (!getSDEPPacket(&response)) {
        result = false;
        goto cleanup;
    }
    
    // Check for error response
    if (response.header.msg_type == SDEP_MSGTYPE_ERROR) {
        result = false;
        goto cleanup;
    }
    
    // Parse integer from response payload
    if (response.header.length > 0 && response.header.length < SDEP_MAX_PACKETSIZE) {
        // Convert response payload to string and parse integer
        response.payload[response.header.length] = '\0';
        *reply = atol((char*)response.payload);
        result = true;
    } else {
        *reply = 0;
        result = true;
    }
    
cleanup:
    // Switch back to original mode if necessary
    if (current_mode == BLUEFRUIT_MODE_DATA) {
        setMode(BLUEFRUIT_MODE_DATA);
    }
    
    return result;
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
    // Check actual connection status via AT command
    int32_t connected = 0;
    if (sendCommandWithIntReply("AT+GAPGETCONN", &connected)) {
        m_connected = (connected == 1);
    } else {
        // Fallback to timing-based check if command fails
        if (millis() - m_reset_time > 2000) {
            m_connected = true;
        }
    }
    return m_connected;
}

bool MinimalBluefruit::setMode(uint8_t mode) {
    // if (m_verbose) {
    //     Serial.print(F("Set mode: "));
    //     Serial.println(mode);
    // }
    
    // Validate mode
    if (mode != BLUEFRUIT_MODE_COMMAND && mode != BLUEFRUIT_MODE_DATA) {
        return false;
    }
    
    // Already in the requested mode
    if (m_mode == mode) {
        return true;
    }
    
    // SPI uses different SDEP command types for DATA/COMMAND mode
    // Just update the internal mode - no AT command needed for SPI
    m_mode = mode;
    
    return true;
}

bool MinimalBluefruit::sendATCommand(const char* cmd) {
    if (!cmd) return false;
    
    size_t cmd_len = strlen(cmd);
    if (cmd_len == 0) return false;
    
    // Send command in SDEP packets
    size_t bytes_sent = 0;
    while (bytes_sent < cmd_len) {
        size_t chunk_size = (cmd_len - bytes_sent < SDEP_MAX_PACKETSIZE) ? (cmd_len - bytes_sent) : SDEP_MAX_PACKETSIZE;
        uint8_t more_data = (bytes_sent + chunk_size < cmd_len) ? 1 : 0;
        
        if (!sendSDEPPacket(SDEP_CMDTYPE_AT_WRAPPER, 
                           (const uint8_t*)(cmd + bytes_sent), 
                           chunk_size, more_data)) {
            return false;
        }
        
        bytes_sent += chunk_size;
    }
    
    return waitForOK();
}

bool MinimalBluefruit::waitForOK() {
    unsigned long timeout = millis() + 1000; // 1 second timeout
    
    // Wait for IRQ to go high indicating response available
    while (!digitalRead(m_irq_pin) && millis() < timeout) {
        delay(10);
    }
    
    if (millis() >= timeout) {
        return false; // Timeout
    }
    
    // Try to read response
    sdep_packet_t response;
    if (!getSDEPPacket(&response)) {
        return false;
    }
    
    // Check if it's an error response
    if (response.header.msg_type == SDEP_MSGTYPE_ERROR) {
        return false;
    }
    
    // Check for valid response packet
    if (response.header.msg_type != SDEP_MSGTYPE_RESPONSE) {
        return false;
    }
    
    // For AT commands, check if response contains "OK"
    if (response.header.length >= 2 && response.header.length < SDEP_MAX_PACKETSIZE) {
        // Null-terminate the response for string comparison
        response.payload[response.header.length] = '\0';
        // Check if response contains "OK"
        return (strstr((char*)response.payload, "OK") != NULL);
    }
    
    // If no payload, assume OK for commands that don't return data
    return true;
}

void MinimalBluefruit::spiInit() {
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    digitalWrite(m_cs_pin, LOW);
}

void MinimalBluefruit::spiEnd() {
    digitalWrite(m_cs_pin, HIGH);
    SPI.endTransaction();
}

uint8_t MinimalBluefruit::spiTransfer(uint8_t data) {
    return SPI.transfer(data);
}

bool MinimalBluefruit::sendSDEPPacket(uint16_t cmd_id, const uint8_t* buf, uint8_t len, uint8_t more_data) {
    if (len > SDEP_MAX_PACKETSIZE) return false;
    
    sdep_packet_t packet;
    
    // Build SDEP command packet (matches Adafruit exactly)
    packet.header.msg_type = SDEP_MSGTYPE_COMMAND;
    packet.header.cmd_id_high = (uint8_t)((cmd_id >> 8) & 0xFF);
    packet.header.cmd_id_low = (uint8_t)(cmd_id & 0xFF);
    packet.header.length = len;
    packet.header.more_data = (len == SDEP_MAX_PACKETSIZE) ? more_data : 0;
    
    // Copy payload
    if (buf && len > 0) {
        memcpy(packet.payload, buf, len);
    }
    
    // Starting SPI transaction (matches Adafruit)
    spiInit();
    
    // Timeout handling (matches Adafruit TimeoutTimer)
    unsigned long timeout_start = millis();
    unsigned long timeout_ms = m_timeout;
    
    // Wait for Bluefruit to be ready (matches Adafruit retry logic)
    while ((spiTransfer(packet.header.msg_type) == SPI_IGNORED_BYTE) && 
           ((millis() - timeout_start) < timeout_ms)) {
        // Disable & Re-enable CS with delay (matches Adafruit exactly)
        spiEnd();
        delayMicroseconds(SPI_DEFAULT_DELAY_US);
        spiInit();
    }
    
    bool result = ((millis() - timeout_start) < timeout_ms);
    
    if (result) {
        // Transfer the rest of the data (matches Adafruit transfer order)
        spiTransfer(packet.header.cmd_id_high);
        spiTransfer(packet.header.cmd_id_low);
        spiTransfer(*((uint8_t*)&packet.header + 3)); // length + more_data byte
        
        // Send payload
        for (uint8_t i = 0; i < len; i++) {
            spiTransfer(packet.payload[i]);
        }
    }
    
    spiEnd();
    return result;
}

bool MinimalBluefruit::getSDEPPacket(void* packet_ptr) {
    sdep_packet_t* packet = (sdep_packet_t*)packet_ptr;
    
    // Wait until IRQ is asserted, double timeout (matches Adafruit)
    unsigned long timeout_start = millis();
    unsigned long timeout_ms = 2 * m_timeout;
    
    while (!digitalRead(m_irq_pin)) {
        if ((millis() - timeout_start) >= timeout_ms) return false;
    }
    
    spiInit();
    
    // Reset timeout for actual transfer (matches Adafruit)
    timeout_start = millis();
    timeout_ms = m_timeout;
    
    // Read message type with retry logic (matches Adafruit exactly)
    do {
        if ((millis() - timeout_start) >= timeout_ms) {
            spiEnd();
            return false;
        }
        
        packet->header.msg_type = spiTransfer(0xFF);
        
        if (packet->header.msg_type == SPI_IGNORED_BYTE) {
            // Bluefruit may not be ready (matches Adafruit comment)
            spiEnd();
            delayMicroseconds(SPI_DEFAULT_DELAY_US);
            spiInit();
        } else if (packet->header.msg_type == SPI_OVERREAD_BYTE) {
            // IRQ may not be pulled down (matches Adafruit comment)
            spiEnd();
            delayMicroseconds(SPI_DEFAULT_DELAY_US);
            spiInit();
        }
    } while (packet->header.msg_type == SPI_IGNORED_BYTE || 
             packet->header.msg_type == SPI_OVERREAD_BYTE);
    
    bool result = false;
    
    // Check for valid message types (matches Adafruit logic)
    if (packet->header.msg_type == SDEP_MSGTYPE_RESPONSE || 
        packet->header.msg_type == SDEP_MSGTYPE_ERROR) {
        
        // Read the rest of the header (matches Adafruit order)
        packet->header.cmd_id_high = spiTransfer(0xFF);
        packet->header.cmd_id_low = spiTransfer(0xFF);
        *((uint8_t*)&packet->header + 3) = spiTransfer(0xFF); // length + more_data
        
        // Validate length
        if (packet->header.length <= SDEP_MAX_PACKETSIZE) {
            // Read payload
            for (uint8_t i = 0; i < packet->header.length; i++) {
                packet->payload[i] = spiTransfer(0xFF);
            }
            result = true;
        }
    }
    
    spiEnd();
    return result;
}