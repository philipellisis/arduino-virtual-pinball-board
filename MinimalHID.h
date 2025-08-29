#ifndef MINIMAL_HID_H
#define MINIMAL_HID_H

#include <Arduino.h>
#include "PluggableUSB.h"
#include "HID.h"

// Define EPTYPE_DESCRIPTOR_SIZE for different architectures
#if defined(ARDUINO_ARCH_AVR)
#define EPTYPE_DESCRIPTOR_SIZE      uint8_t
#elif defined(ARDUINO_ARCH_SAM)
#define EPTYPE_DESCRIPTOR_SIZE      uint32_t
#elif defined(ARDUINO_ARCH_SAMD)
#if defined(ARDUINO_API_VERSION)
#define EPTYPE_DESCRIPTOR_SIZE      unsigned int
#else
#define EPTYPE_DESCRIPTOR_SIZE      uint32_t
#endif
#else
#define EPTYPE_DESCRIPTOR_SIZE      uint8_t  // Default fallback
#endif

// Media key constants
#define MEDIA_VOLUME_MUTE    0xE2
#define MEDIA_VOLUME_DOWN    0xEA
#define MEDIA_VOLUME_UP      0xE9

// Keyboard key code mapping function
inline uint8_t KeyboardKeycode(uint8_t key) {
    return key;
}

// Gamepad report structure (matches HID-Project exactly)
typedef union __attribute__((packed)) {
    uint32_t buttons;
    struct __attribute__((packed)) {
        uint8_t button1 : 1;
        uint8_t button2 : 1;
        uint8_t button3 : 1;
        uint8_t button4 : 1;
        uint8_t button5 : 1;
        uint8_t button6 : 1;
        uint8_t button7 : 1;
        uint8_t button8 : 1;
        
        uint8_t button9 : 1;
        uint8_t button10 : 1;
        uint8_t button11 : 1;
        uint8_t button12 : 1;
        uint8_t button13 : 1;
        uint8_t button14 : 1;
        uint8_t button15 : 1;
        uint8_t button16 : 1;
        
        uint8_t button17 : 1;
        uint8_t button18 : 1;
        uint8_t button19 : 1;
        uint8_t button20 : 1;
        uint8_t button21 : 1;
        uint8_t button22 : 1;
        uint8_t button23 : 1;
        uint8_t button24 : 1;
        
        uint8_t button25 : 1;
        uint8_t button26 : 1;
        uint8_t button27 : 1;
        uint8_t button28 : 1;
        uint8_t button29 : 1;
        uint8_t button30 : 1;
        uint8_t button31 : 1;
        uint8_t button32 : 1;
        
        int16_t xAxis;
        int16_t yAxis;
        int16_t rxAxis;
        int16_t ryAxis;
        int8_t zAxis;
        int8_t rzAxis;
        uint8_t dPad1 : 4;
        uint8_t dPad2 : 4;
    };
} HID_GamepadReport_Data_t;

// Minimal Gamepad class using PluggableUSB
class MinimalGamepad : public PluggableUSBModule {
public:
    MinimalGamepad();
    
    void begin();
    void xAxis(int16_t value);
    void yAxis(int16_t value); 
    void zAxis(int8_t value);
    void press(uint8_t button);
    void release(uint8_t button);
    void write();
    
protected:
    // PluggableUSBModule implementation
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    
    EPTYPE_DESCRIPTOR_SIZE epType[1];
    uint8_t protocol;
    uint8_t idle;
    
    HID_GamepadReport_Data_t _report;
    
    void SendReport(void* data, int length);
};

// Minimal Boot Keyboard class
class BootKeyboardClass : public PluggableUSBModule {
public:
    BootKeyboardClass();
    void begin();
    void press(uint8_t key);
    void release(uint8_t key);
    
protected:
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    
    EPTYPE_DESCRIPTOR_SIZE epType[1];
    uint8_t protocol;
    uint8_t idle;
    
private:
    uint8_t _keyReport[8];
    void sendReport();
};

// Minimal Consumer Control class
class SingleConsumerClass : public PluggableUSBModule {
public:
    SingleConsumerClass();
    void begin();
    void press(uint16_t key);
    void release(uint16_t key);
    
protected:
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    
    EPTYPE_DESCRIPTOR_SIZE epType[1];
    uint8_t protocol;
    uint8_t idle;
    
private:
    uint16_t _consumerReport;
    void sendReport();
};

// Global instances
extern MinimalGamepad Gamepad1;
extern BootKeyboardClass BootKeyboard;
extern SingleConsumerClass SingleConsumer;

#endif