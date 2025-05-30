/**************************************************************************/
/*!
    @file     Adafruit_BluefruitLE_SPI.cpp
    @author   hathach, ktown (Adafruit Industries)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2015, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include "Adafruit_BluefruitLE_SPI.h"
#include <Arduino.h>
#include <stdlib.h>

#ifndef min
  #define min(a,b) ((a) < (b) ? (a) : (b))
#endif


SPISettings bluefruitSPI(4000000, MSBFIRST, SPI_MODE0);


/******************************************************************************/
/*!
    @brief Instantiates a new instance of the Adafruit_BluefruitLE_SPI class

    @param[in]  csPin
                The location of the CS pin for the SPI interface
    @param[in]  irqPin
                The location of the HW IRQ pin (pin 2 or pin 3 on the Arduino
                Uno). This must be a HW interrupt pin!
    @param[in]  rstPin
*/
/******************************************************************************/
Adafruit_BluefruitLE_SPI::Adafruit_BluefruitLE_SPI(int8_t csPin, int8_t irqPin, int8_t rstPin)
{
  _physical_transport = BLUEFRUIT_TRANSPORT_HWSPI;

  m_cs_pin  = csPin;
  m_irq_pin = irqPin;
  m_rst_pin = rstPin;

  m_miso_pin = m_mosi_pin = m_sck_pin = -1;
}

// Software SPI constructor removed to save space


/******************************************************************************/
/*!
    @brief Initialize the HW to enable communication with the BLE module

    @return Returns 'true' if everything initialised correctly, otherwise
            'false' if there was a problem during HW initialisation. If
            'irqPin' is not a HW interrupt pin false will be returned.
*/
/******************************************************************************/
bool Adafruit_BluefruitLE_SPI::begin(boolean v, boolean blocking)
{
  _verbose = v;

  pinMode(m_irq_pin, INPUT);

  // Set CS pin to output and de-assert by default
  pinMode(m_cs_pin, OUTPUT);
  digitalWrite(m_cs_pin, HIGH);

  if (m_sck_pin == -1) {
    // hardware SPI
    SPI.begin();
  } else {
    pinMode(m_sck_pin, OUTPUT);
    digitalWrite(m_sck_pin, LOW);
    pinMode(m_miso_pin, INPUT);
    pinMode(m_mosi_pin, OUTPUT);
  }

  bool isOK;

  // Always try to send Initialize command to reset
  // Bluefruit since user can define but not wiring RST signal
  isOK = sendInitializePattern();

  // use hardware reset if available
  if (m_rst_pin >= 0)
  {
    // pull the RST to GND for 10 ms
    pinMode(m_rst_pin, OUTPUT);
    digitalWrite(m_rst_pin, HIGH);
    digitalWrite(m_rst_pin, LOW);
    delay(10);
    digitalWrite(m_rst_pin, HIGH);

    isOK= true;
  }

  _reset_started_timestamp = millis();

  // Bluefruit takes 1 second to reboot
  if (blocking)
  {
    delay(1000);
  }

  return isOK;
}

/******************************************************************************/
/*!
    @brief  Uninitializes the SPI interface
*/
/******************************************************************************/
void Adafruit_BluefruitLE_SPI::end(void)
{
  if (m_sck_pin == -1) {
    SPI.end();
  }
}

// simulateSwitchMode() removed to save space

/******************************************************************************/
/*!
    @brief Simulate "+++" switch mode command
*/
/******************************************************************************/
bool Adafruit_BluefruitLE_SPI::setMode(uint8_t new_mode)
{
  // invalid mode
  if ( !(new_mode == BLUEFRUIT_MODE_COMMAND || new_mode == BLUEFRUIT_MODE_DATA) ) return false;

  // Already in the wanted mode
  if ( _mode == new_mode ) return true;

  // SPI use different SDEP command when in DATA/COMMAND mode.
  // --> does not switch using +++ command
  _mode = new_mode;

  // If we're entering DATA mode, flush any old response, so that it isn't
  // interpreted as incoming UART data
  if (_mode == BLUEFRUIT_MODE_DATA) flush();

  return true;
}

// enableModeSwitchCommand() removed to save space

/******************************************************************************/
/*!
    @brief Send initialize pattern to Bluefruit LE to force a reset. This pattern
    follow the SDEP command syntax with command_id = SDEP_CMDTYPE_INITIALIZE.
    The command has NO response, and is expected to complete within 1 second
*/
/******************************************************************************/
bool Adafruit_BluefruitLE_SPI::sendInitializePattern(void)
{
  return sendPacket(SDEP_CMDTYPE_INITIALIZE, NULL, 0, 0);
}

/******************************************************************************/
/*!
    @brief  Send out an packet with data in m_tx_buffer

    @param[in]  more_data
                More Data bitfield, 0 indicates this is not end of transfer yet
*/
/******************************************************************************/
bool Adafruit_BluefruitLE_SPI::sendPacket(uint16_t command, const uint8_t* buf, uint8_t count, uint8_t more_data)
{
  // flush old response before sending the new command, but only if we're *not*
  // in DATA mode, as the RX FIFO may containg incoming UART data that hasn't
  // been read yet
  if (more_data == 0 && _mode != BLUEFRUIT_MODE_DATA) flush();

  sdepMsgCommand_t msgCmd;

  msgCmd.header.msg_type    = SDEP_MSGTYPE_COMMAND;
  msgCmd.header.cmd_id_high = highByte(command);
  msgCmd.header.cmd_id_low  = lowByte(command);
  msgCmd.header.length      = count;
  msgCmd.header.more_data   = (count == SDEP_MAX_PACKETSIZE) ? more_data : 0;

  // Copy payload
  if ( buf != NULL && count > 0) memcpy(msgCmd.payload, buf, count);

  // Starting SPI transaction
  if (m_sck_pin == -1)
    SPI.beginTransaction(bluefruitSPI);

  SPI_CS_ENABLE();

  TimeoutTimer tt(_timeout);

  // Bluefruit may not be ready
  while ( ( spixfer(msgCmd.header.msg_type) == SPI_IGNORED_BYTE ) && !tt.expired() )
  {
    // Disable & Re-enable CS with a bit of delay for Bluefruit to ready itself
    SPI_CS_DISABLE();
    delayMicroseconds(SPI_DEFAULT_DELAY_US);
    SPI_CS_ENABLE();
  }

  bool result = !tt.expired();
  if ( result )
  {
    // transfer the rest of the data
    spixfer((void*) (((uint8_t*)&msgCmd) +1), sizeof(sdepMsgHeader_t)+count-1);
  }

  SPI_CS_DISABLE();
  if (m_sck_pin == -1)
    SPI.endTransaction();

  return result;
}

// write(uint8_t) removed to save space - not used by BluetoothController

// write(buffer, size) removed to save space - not used by BluetoothController

// available() removed to save space - not used by BluetoothController

// read() removed to save space - not used by BluetoothController

// peek() removed to save space - not used by BluetoothController

// flush() removed to save space - not used by BluetoothController

// getResponse() removed to save space - not used by BluetoothController

// getPacket() removed to save space - not used by BluetoothController

/******************************************************************************/
/*!

*/
/******************************************************************************/
void Adafruit_BluefruitLE_SPI::spixfer(void *buff, size_t len) {
  uint8_t *p = (uint8_t *)buff;

  while (len--) {
    p[0] = spixfer(p[0]);
    p++;
  }
}

/******************************************************************************/
/*!

*/
/******************************************************************************/
uint8_t Adafruit_BluefruitLE_SPI::spixfer(uint8_t x) {
  if (m_sck_pin == -1) {
    uint8_t reply = SPI.transfer(x);
    //SerialDebug.println(reply, HEX);
    return reply;
  }

  // software spi
  uint8_t reply = 0;
  for (int i=7; i>=0; i--) {
    reply <<= 1;
    digitalWrite(m_sck_pin, LOW);
    digitalWrite(m_mosi_pin, x & (1<<i));
    digitalWrite(m_sck_pin, HIGH);
    if (digitalRead(m_miso_pin))
      reply |= 1;
  }
  digitalWrite(m_sck_pin, LOW);
  //SerialDebug.println(reply, HEX);
  return reply;
}
