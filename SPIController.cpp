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
    bool inputChanged = hasInputChanged();

    // Send a heartbeat packet even when inputs are idle so PinOne can receive
    // any pending rumble commands from the ESP32 with bounded latency.
    uint32_t now = millis();
    bool heartbeat = (now - _lastHeartbeat >= 100);

    if (inputChanged || heartbeat) {
        if (heartbeat) _lastHeartbeat = now;
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

    // Pack 1 byte per button based on current mode
    // Keyboard mode (disableButtonPressWhenKeyboardEnabled == true): send keycode when pressed, 0 when released
    // Button mode (disableButtonPressWhenKeyboardEnabled == false): send 255 when pressed, 0 when released
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        if (config.processedButtonState[i]) {
            if (config.disableButtonPressWhenKeyboardEnabled) {
                // Keyboard mode: send the keycode for this button
                uint8_t keycode = config.buttonKeyboard[i];
                buttonPacket[i] = (keycode > 0) ? keycode : 255;  // Use 255 if no keycode assigned
            } else {
                // Button mode: send 255 for pressed
                buttonPacket[i] = 255;
            }
        } else {
            // Button not pressed: send 0
            buttonPacket[i] = 0;
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

    // Packet structure (40 bytes total):
    // Bytes 0-31:  Button data (1 byte per button: keycode or 255/0)
    // Bytes 32-33: Plunger value (16-bit, high byte first)
    // Bytes 34-35: X-axis (16-bit, high byte first)
    // Bytes 36-37: Y-axis (16-bit, high byte first)
    // Byte 38:     Mode flag (1 = keyboard mode, 0 = button mode)
    // Byte 39:     Checksum (XOR of bytes 0-38)

    // 1) Pack button data (32 bytes, 1 per button)
    packButtonData(txBuf);

    // 2) Get current analog values
    int16_t xAxis = getXAxisValue();
    int16_t yAxis = getYAxisValue();
    int16_t plungerVal = getPlungerValue();

    // 3) Pack analog values (high byte first)
    txBuf[32] = (plungerVal >> 8) & 0xFF;
    txBuf[33] = plungerVal & 0xFF;

    txBuf[34] = (xAxis >> 8) & 0xFF;
    txBuf[35] = xAxis & 0xFF;

    txBuf[36] = (yAxis >> 8) & 0xFF;
    txBuf[37] = yAxis & 0xFF;

    // 4) Pack mode flag (1 = keyboard mode, 0 = button mode)
    txBuf[38] = config.disableButtonPressWhenKeyboardEnabled ? 1 : 0;

    // 5) Calculate XOR checksum of bytes 0-38
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < PACKET_SIZE - 1; i++) {
        checksum ^= txBuf[i];
    }
    txBuf[39] = checksum;

    // 6) Send packet via SPI and capture the MISO response from the ESP32.
    //    SPI.transfer() returns the byte shifted in from MISO simultaneously
    //    with the byte shifted out on MOSI — zero additional overhead.
    uint8_t rxBuf[PACKET_SIZE];
    digitalWrite(SS_PIN, LOW);
    for (uint8_t i = 0; i < PACKET_SIZE; i++) {
        rxBuf[i] = SPI.transfer(txBuf[i]);
    }
    digitalWrite(SS_PIN, HIGH);

    // 7) Parse any back-channel output command carried in the MISO response
    parseBackchannelPacket(rxBuf);

    // 8) Update last known states
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        lastButtonStates[i] = config.lastButtonState[i];
    }
    lastXAxis = xAxis;
    lastYAxis = yAxis;
    lastPlungerValue = plungerVal;
}

// ---------------------------------------------------------------------------
//  SPI back-channel: output commands sent from the ESP32 via MISO
// ---------------------------------------------------------------------------
//
//  Packet layout (40 bytes) — see SpiReceiver.h for full spec:
//    Byte 0:      Type  — 0x4F='O' multi-bank, 0x53='S' single output, 0x00 idle
//  For 0x4F:
//    Byte 1:      Slot count (1–4)
//    Bytes 2–33:  Up to 4 slots × 8 bytes each: [bank_index(0-8), v0–v6]
//  For 0x53:
//    Byte 1:      Output ID (0–62)
//    Byte 2:      Value (0–255)

void SPIController::parseBackchannelPacket(const uint8_t* rxBuf) {
    if (rxBuf[0] == 0x00) return;  // idle

    uint8_t cs = 0;
    for (uint8_t i = 0; i < PACKET_SIZE - 1; i++) cs ^= rxBuf[i];
    if (cs != rxBuf[PACKET_SIZE - 1]) return;  // bad checksum

    _bcType = rxBuf[0];
    _bcArg  = rxBuf[1];
    if (_bcType == 0x4F) {
        memcpy(_bcRaw, &rxBuf[2], 32);  // up to 4 slots × 8 bytes
    } else if (_bcType == 0x53) {
        _bcRaw[0] = rxBuf[2];           // single value
    }
    _bcPending = true;
}

bool SPIController::hasOutputPacket() const {
    return _bcPending;
}

void SPIController::applyOutputPacket(Outputs& outputs) {
    if (!_bcPending) return;
    _bcPending = false;

    if (_bcType == 0x53) {
        // Single-output: _bcArg = output ID, _bcRaw[0] = value
        outputs.updateOutput(_bcArg, _bcRaw[0]);

    } else if (_bcType == 0x4F) {
        // Multi-bank: _bcArg = slot count, _bcRaw = packed slot data.
        // Each slot: [bank_index(0-8), v0–v6] — same as DOF serial bank updates
        // (bankOffset stripped; Communication.cpp uses baseIndex = bank*7).
        uint8_t count = _bcArg;
        if (count > 4) count = 4;
        for (uint8_t s = 0; s < count; s++) {
            const uint8_t* slot    = &_bcRaw[s * 8];
            uint8_t        baseOut = slot[0] * 7;
            for (uint8_t i = 0; i < 7; i++) {
                outputs.updateOutput(baseOut + i, slot[1 + i]);
            }
        }
    }
}

void SPIController::sendBleConfigPacket(const uint8_t* map) {
    uint8_t txBuf[PACKET_SIZE] = {0};

    // Bytes 0-31: BLE button slot map
    for (uint8_t i = 0; i < 32; i++) {
        txBuf[i] = map[i];
    }

    // Bytes 32-37: analog fields unused in config packet (already 0)
    // Byte 38: mode = 2 (BLE config packet)
    txBuf[38] = 2;

    // Byte 39: XOR checksum of bytes 0-38
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < PACKET_SIZE - 1; i++) {
        checksum ^= txBuf[i];
    }
    txBuf[39] = checksum;

    digitalWrite(SS_PIN, LOW);
    for (uint8_t i = 0; i < PACKET_SIZE; i++) {
        SPI.transfer(txBuf[i]);
    }
    digitalWrite(SS_PIN, HIGH);

    // Reset the heartbeat timer so update() does not immediately fire another
    // SPI packet.  The regular heartbeat packet arriving right on top of the
    // config packet is what caused the race condition where the ESP32's queue
    // (depth 1, overwrite) dropped the config before processPacket() read it.
    _lastHeartbeat = millis();
}