#include "MinimalBluefruit.h"
#include <string.h>

// All constants and types are defined in header file

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
    
    // Send initialize pattern to reset the Bluefruit module
    sendSDEPPacket(SDEP_CMDTYPE_INITIALIZE, NULL, 0, 0);
    
    //if (m_verbose) Serial.println(F("MinimalBluefruit initialized"));
    return true;
}

bool MinimalBluefruit::factoryReset() {
    //if (m_verbose) Serial.println(F("Factory reset"));
    
    // Send initialize pattern first to reset the module
    sendSDEPPacket(SDEP_CMDTYPE_INITIALIZE, NULL, 0, 0);
    
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
    
    // For AT commands, we expect "OK" in the response
    // For simplicity, just check that we got a response packet
    return (response.header.msg_type == SDEP_MSGTYPE_RESPONSE);
}

void MinimalBluefruit::spiInit() {
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
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
    
    // Build SDEP command packet
    packet.header.msg_type = SDEP_MSGTYPE_COMMAND;
    packet.header.cmd_id_low = (uint8_t)(cmd_id & 0xFF);
    packet.header.cmd_id_high = (uint8_t)((cmd_id >> 8) & 0xFF);
    packet.header.length = len;
    packet.header.more_data = (len == SDEP_MAX_PACKETSIZE) ? more_data : 0;
    
    // Copy payload
    if (buf && len > 0) {
        memcpy(packet.payload, buf, len);
    }
    
    spiInit();
    
    unsigned long timeout = millis() + 500; // 500ms timeout
    
    // Wait for Bluefruit to be ready
    while (spiTransfer(packet.header.msg_type) == SPI_IGNORED_BYTE && millis() < timeout) {
        spiEnd();
        delayMicroseconds(SPI_DEFAULT_DELAY_US);
        spiInit();
    }
    
    bool result = (millis() < timeout);
    
    if (result) {
        // Send the rest of the packet
        spiTransfer(packet.header.cmd_id_low);
        spiTransfer(packet.header.cmd_id_high);
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
    
    unsigned long timeout = millis() + 1000; // 1 second timeout
    
    // Wait for IRQ to indicate data is ready
    while (!digitalRead(m_irq_pin) && millis() < timeout) {
        delayMicroseconds(100);
    }
    
    if (millis() >= timeout) return false;
    
    spiInit();
    
    timeout = millis() + 500; // 500ms for actual transfer
    
    // Read message type
    do {
        if (millis() >= timeout) {
            spiEnd();
            return false;
        }
        
        packet->header.msg_type = spiTransfer(0xFF);
        
        if (packet->header.msg_type == SPI_IGNORED_BYTE) {
            spiEnd();
            delayMicroseconds(SPI_DEFAULT_DELAY_US);
            spiInit();
        } else if (packet->header.msg_type == SPI_OVERREAD_BYTE) {
            spiEnd();
            delayMicroseconds(SPI_DEFAULT_DELAY_US);
            spiInit();
        }
    } while (packet->header.msg_type == SPI_IGNORED_BYTE || 
             packet->header.msg_type == SPI_OVERREAD_BYTE);
    
    // Check for valid message types
    if (packet->header.msg_type != SDEP_MSGTYPE_RESPONSE && 
        packet->header.msg_type != SDEP_MSGTYPE_ERROR) {
        spiEnd();
        return false;
    }
    
    // Read the rest of the header
    packet->header.cmd_id_low = spiTransfer(0xFF);
    packet->header.cmd_id_high = spiTransfer(0xFF);
    *((uint8_t*)&packet->header + 3) = spiTransfer(0xFF); // length + more_data
    
    // Validate length
    if (packet->header.length > SDEP_MAX_PACKETSIZE) {
        spiEnd();
        return false;
    }
    
    // Read payload
    for (uint8_t i = 0; i < packet->header.length; i++) {
        packet->payload[i] = spiTransfer(0xFF);
    }
    
    spiEnd();
    return true;
}