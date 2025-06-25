#include "SPIController.h"
#include "Globals.h"

SPIController::SPIController() : 
    lastXAxis(0),
    lastYAxis(0),
    lastPlungerValue(0)
{
    // Initialize button states
    for (int i = 0; i < NUM_BUTTONS; i++) {
        lastButtonStates[i] = false;
    }
}

void SPIController::init() {
    pinMode(SS_PIN, OUTPUT);
    digitalWrite(SS_PIN, HIGH);  // Idle high
    
    // Begin SPI as Master, MODE0, MSB-first
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    // Use a reasonable clock speed - can adjust if needed
    SPI.setClockDivider(SPI_CLOCK_DIV16);  // 1MHz on 16MHz Arduino
}

void SPIController::update() {
    // Only send update if inputs have changed
    if (hasInputChanged()) {
        sendSPIPacket();
    }
}

bool SPIController::hasInputChanged() {
    // Check button changes
    for (int i = 0; i < NUM_BUTTONS && i < 32; i++) {
        if (lastButtonStates[i] != config.lastButtonState[i]) {
            return true;
        }
    }
    
    // Check axis changes
    int16_t currentX = getXAxisValue();
    int16_t currentY = getYAxisValue();
    int16_t currentPlunger = getPlungerValue();
    
    if (currentX != lastXAxis || currentY != lastYAxis || currentPlunger != lastPlungerValue) {
        return true;
    }
    
    return false;
}

void SPIController::packButtonData(uint8_t* buttonPacket) {
    memset(buttonPacket, 0x00, BUTTON_BYTES);
    
    // Pack button states into bytes
    for (uint8_t i = 0; i < NUM_BUTTONS && i < 32; i++) {
        if (config.lastButtonState[i]) {
            buttonPacket[i / 8] |= (1 << (i % 8));
        }
    }
}

int16_t SPIController::getXAxisValue() {
    // Get X-axis value from accelerometer
    if (config.accelerometer > 0) {
        int xValue = accel.getXValue();
        return static_cast<int16_t>(static_cast<float>(xValue) / config.accelerometerMax * 32767);
    }
    return 0;
}

int16_t SPIController::getYAxisValue() {
    // Get Y-axis value from accelerometer
    if (config.accelerometer > 0) {
        int yValue = accel.getYValue();
        return static_cast<int16_t>(static_cast<float>(yValue) / config.accelerometerMaxY * 32767);
    }
    return 0;
}

int16_t SPIController::getPlungerValue() {
    // Get plunger value
    if (config.enablePlunger) {
        return plunger.getAdjustedValue();
    }
    return 0;
}

void SPIController::sendSPIPacket() {
    uint8_t txBuf[PACKET_SIZE];
    
    // 1) Pack button data (4 bytes for 32 buttons)
    packButtonData(txBuf);
    
    // 2) Get current values
    int16_t xAxis = getXAxisValue();
    int16_t yAxis = getYAxisValue();
    int16_t plungerVal = getPlungerValue();
    
    // 3) Pack data into packet with duplicate values for validation
    // Bytes 0-3: Button data
    // Bytes 4-5: Plunger value #1 (16-bit, split into 2 bytes)
    // Bytes 6-7: X-axis #1 (16-bit, split into 2 bytes)
    // Bytes 8-9: Y-axis #1 (16-bit, split into 2 bytes)
    // Bytes 10-11: Plunger value #2 (duplicate for validation)
    // Bytes 12-13: X-axis #2 (duplicate for validation)
    // Bytes 14-15: Y-axis #2 (duplicate for validation)
    
    // First set of values
    txBuf[4] = (plungerVal >> 8) & 0xFF;  // High byte
    txBuf[5] = plungerVal & 0xFF;         // Low byte
    
    txBuf[6] = (xAxis >> 8) & 0xFF;       // High byte
    txBuf[7] = xAxis & 0xFF;              // Low byte
    
    txBuf[8] = (yAxis >> 8) & 0xFF;       // High byte
    txBuf[9] = yAxis & 0xFF;              // Low byte
    
    // Duplicate set of values for validation
    txBuf[10] = (plungerVal >> 8) & 0xFF; // High byte duplicate
    txBuf[11] = plungerVal & 0xFF;        // Low byte duplicate
    
    txBuf[12] = (xAxis >> 8) & 0xFF;      // High byte duplicate
    txBuf[13] = xAxis & 0xFF;             // Low byte duplicate
    
    txBuf[14] = (yAxis >> 8) & 0xFF;      // High byte duplicate
    txBuf[15] = yAxis & 0xFF;             // Low byte duplicate
    
    // Count pressed buttons for validation (based on what was actually packed)
    uint8_t buttonCount = 0;
    for (int i = 0; i < NUM_BUTTONS && i < 32; i++) {
        if (txBuf[i / 8] & (1 << (i % 8))) {
            buttonCount++;
        }
    }
    
    // Add button count and simple checksum for validation
    if (PACKET_SIZE > 16) {
        txBuf[16] = buttonCount;    // Button count for validation
        
        // Calculate simple checksum (sum of all bytes except checksum byte)
        uint8_t checksum = 0;
        for (int i = 0; i < PACKET_SIZE - 1; i++) {
            checksum ^= txBuf[i];  // XOR checksum
        }
        txBuf[PACKET_SIZE - 1] = checksum;    // Checksum at end
    }
    
    // 4) Send packet via SPI
    digitalWrite(SS_PIN, LOW);
    for (uint8_t i = 0; i < PACKET_SIZE; i++) {
        SPI.transfer(txBuf[i]);
    }
    digitalWrite(SS_PIN, HIGH);
    
    // 5) Update last known states
    for (int i = 0; i < NUM_BUTTONS && i < 32; i++) {
        lastButtonStates[i] = config.lastButtonState[i];
    }
    lastXAxis = xAxis;
    lastYAxis = yAxis;
    lastPlungerValue = plungerVal;
}