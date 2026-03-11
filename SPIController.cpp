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
    uint8_t rxBuf[PACKET_SIZE] = {0};
    digitalWrite(SS_PIN, LOW);
    for (uint8_t i = 0; i < PACKET_SIZE; i++) {
        rxBuf[i] = SPI.transfer(txBuf[i]);
    }
    digitalWrite(SS_PIN, HIGH);
    parseBackchannelPacket(rxBuf);

    // 7) Update last known states
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        lastButtonStates[i] = config.lastButtonState[i];
    }
    lastXAxis = xAxis;
    lastYAxis = yAxis;
    lastPlungerValue = plungerVal;
}

void SPIController::parseBackchannelPacket(const uint8_t* rxBuf) {
    // Validate XOR checksum of bytes 0-38
    uint8_t cs = 0;
    for (uint8_t i = 0; i < PACKET_SIZE - 1; i++) cs ^= rxBuf[i];
    if (cs != rxBuf[PACKET_SIZE - 1]) return;

    uint8_t type = rxBuf[0];
    if (type == 0x00) return;  // idle — nothing to do

    if (type == 0x53) {
        // Single output: [0x53, outputId, value, ...]
        _bcType    = 0xFF;  // reuse 0xFF as "single" sentinel in applyOutputPacket
        _bcArg     = rxBuf[1];
        _bcRaw[0]  = rxBuf[2];
        _bcPending = true;
    } else if (type == 0x4F) {
        // Bank update: [0x4F, count, bankSlot0(8 bytes), ...]
        // We only apply the first bank slot to keep things simple; any additional
        // slots queued in subsequent SPI transactions will be processed then.
        uint8_t count = rxBuf[1];
        if (count > 0) {
            uint8_t bankIdx = rxBuf[2];
            _bcType    = bankIdx;
            memcpy(_bcRaw, &rxBuf[3], 7);
            _bcPending = true;
        }
    }
}

void SPIController::applyOutputPacket(Outputs& outputs) {
    if (!_bcPending) return;
    _bcPending = false;

    if (_bcType == 0xFF) {
        // Single output
        uint8_t outputId = _bcArg;
        uint8_t value    = _bcRaw[0];
        outputs.updateOutput(outputId, value);
    } else {
        // Bank update — _bcType holds the bankIndex (0-8)
        uint8_t base = _bcType * 7;
        for (uint8_t i = 0; i < 7; i++) {
            outputs.updateOutput(base + i, _bcRaw[i]);
        }
    }
}

// Shared helper: sends bytes 0-31 of data[] as an SPI config packet with the
// given mode byte.  Used for both button map (mode=2) and device name (mode=3).
static void sendConfigSPI(uint8_t ssPin, const uint8_t* data, uint8_t len, uint8_t mode) {
    uint8_t txBuf[40] = {0};
    uint8_t n = (len < 32) ? len : 32;
    for (uint8_t i = 0; i < n; i++) txBuf[i] = data[i];
    txBuf[38] = mode;
    uint8_t cs = 0;
    for (uint8_t i = 0; i < 39; i++) cs ^= txBuf[i];
    txBuf[39] = cs;
    digitalWrite(ssPin, LOW);
    for (uint8_t i = 0; i < 40; i++) SPI.transfer(txBuf[i]);
    digitalWrite(ssPin, HIGH);
}

void SPIController::sendBleConfigPacket(const uint8_t* map) {
    sendConfigSPI(SS_PIN, map, 32, 2);
    _lastHeartbeat = millis();
}

void SPIController::sendBleNamePacket(const uint8_t* name, uint8_t len) {
    sendConfigSPI(SS_PIN, name, len, 3);
    _lastHeartbeat = millis();
}