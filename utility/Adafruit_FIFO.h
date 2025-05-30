/**************************************************************************/
/*!
    @file     Adafruit_FIFO.h
    @author   Minimal FIFO implementation for space saving
    
    Simple FIFO buffer - removed to save space since BluetoothController
    doesn't use data mode or buffering functionality.
*/
/**************************************************************************/

#ifndef _ADAFRUIT_FIFO_H_
#define _ADAFRUIT_FIFO_H_

#include <Arduino.h>

// Minimal stub class - not actually used by BluetoothController
class Adafruit_FIFO {
  public:
    Adafruit_FIFO(void* buffer, uint16_t size, uint16_t item_size, bool overwritable = false) {}
    bool empty() { return true; }
    bool full() { return false; }
    uint16_t count() { return 0; }
    uint16_t remaining() { return 0; }
    bool read(void* item) { return false; }
    bool write(void* item) { return false; }
    bool peek(void* item) { return false; }
    void clear() {}
    void write_n(void* data, uint16_t count) {}
};

#endif