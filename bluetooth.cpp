#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"

// Use hardware SPI: CS, IRQ, RST
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS,
                            BLUEFRUIT_SPI_IRQ,
                            BLUEFRUIT_SPI_RST);

// Button input pin
const int buttonPin = 10;

void error(const __FlashStringHelper* msg) {
  Serial.println(msg);
  while (1);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial);

  Serial.println(F("Starting BLE HID Gamepad over SPI..."));

  // 1) Initialize the module
  if (!ble.begin(false)) {
    error(F("Couldn't find Bluefruit module via SPI. Check wiring."));
  }
  Serial.println(F("Module found."));

  // 2) Factory-reset for a clean state (optional after first flash)
  ble.factoryReset();

  // 3) Turn off AT echo to keep replies clean
  ble.echo(false);

  // 4) Set a friendly name
  ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Gamepad"));

  // 5) Query & enable BLE HID service if needed
  int32_t hidEnabled = 0;
  ble.sendCommandWithIntReply(F("AT+BLEHIDEN?"), &hidEnabled);
  if (!hidEnabled) {
    Serial.println(F("Enabling BLE HID service..."));
    ble.sendCommandCheckOK(F("AT+BLEHIDEN=1"));
  }

  // 6) Query & enable BLE HID Gamepad profile if needed
  int32_t gpEnabled = 0;
  ble.sendCommandWithIntReply(F("AT+BLEHIDGAMEPADEN?"), &gpEnabled);
  if (!gpEnabled) {
    Serial.println(F("Enabling BLE HID Gamepad profile..."));
    ble.sendCommandCheckOK(F("AT+BLEHIDGAMEPADEN=1"));
  }

  // 7) Reset module to apply any service changes
  Serial.println(F("Resetting module to apply changes..."));
  ble.reset();

  // 8) Wait for a central to connect
  Serial.print(F("Waiting for BLE connection"));
  while (!ble.isConnected()) {
    Serial.print(F("."));
    delay(500);
  }

  Serial.println(F("\nConnected! Switching to DATA mode..."));
  ble.setMode(BLUEFRUIT_MODE_DATA);
}

void loop() {
  static bool lastState = HIGH;
  bool state = digitalRead(buttonPin);

  if (state != lastState) {
    lastState = state;

    // 3-byte report: [buttons][X][Y]
    uint8_t btnMask = (state == LOW ? 0x01 : 0x00);
    
    // build AT command: AT+BLEHIDGAMEPAD=<x>,<y>,<buttons>
    // we’re only doing buttons, so x=0, y=0
    char cmd[32];
    sprintf(cmd, "AT+BLEHIDGAMEPAD=0,0,0x%02X", btnMask);
    ble.sendCommandCheckOK(cmd);

    Serial.print(F("Sent HID report: "));
    Serial.println(state);
  }

  delay(20);  // ~50 Hz updates
}
