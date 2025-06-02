#include "MinimalHID.h"
#include <string.h>

// HID Report Descriptor for Gamepad (copied from HID-Project)
static const uint8_t _hidReportDescriptorGamepad[] PROGMEM = {
    /* Gamepad with 32 buttons and 6 axis*/
    0x05, 0x01,                    /* USAGE_PAGE (Generic Desktop) */
    0x09, 0x04,                    /* USAGE (Joystick) */
    0xa1, 0x01,                    /* COLLECTION (Application) */
    /* 32 Buttons */
    0x05, 0x09,                    /*   USAGE_PAGE (Button) */
    0x19, 0x01,                    /*   USAGE_MINIMUM (Button 1) */
    0x29, 0x20,                    /*   USAGE_MAXIMUM (Button 32) */
    0x15, 0x00,                    /*   LOGICAL_MINIMUM (0) */
    0x25, 0x01,                    /*   LOGICAL_MAXIMUM (1) */
    0x75, 0x01,                    /*   REPORT_SIZE (1) */
    0x95, 0x20,                    /*   REPORT_COUNT (32) */
    0x81, 0x02,                    /*   INPUT (Data,Var,Abs) */
    /* 4 16bit Axis */
    0x05, 0x01,                    /*   USAGE_PAGE (Generic Desktop) */
    0xa1, 0x00,                    /*   COLLECTION (Physical) */
    0x09, 0x30,                    /*     USAGE (X) */
    0x09, 0x31,                    /*     USAGE (Y) */
    0x09, 0x33,                    /*     USAGE (Rx) */
    0x09, 0x34,                    /*     USAGE (Ry) */
    0x16, 0x00, 0x80,              /*     LOGICAL_MINIMUM (-32768) */
    0x26, 0xFF, 0x7F,              /*     LOGICAL_MAXIMUM (32767) */
    0x75, 0x10,                    /*     REPORT_SIZE (16) */
    0x95, 0x04,                    /*     REPORT_COUNT (4) */
    0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */
    /* 2 8bit Axis */
    0x09, 0x32,                    /*     USAGE (Z) */
    0x09, 0x35,                    /*     USAGE (Rz) */
    0x15, 0x80,                    /*     LOGICAL_MINIMUM (-128) */
    0x25, 0x7F,                    /*     LOGICAL_MAXIMUM (127) */
    0x75, 0x08,                    /*     REPORT_SIZE (8) */
    0x95, 0x02,                    /*     REPORT_COUNT (2) */
    0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */
    0xc0,                          /*   END_COLLECTION */
    /* 2 Hat Switches */
    0x05, 0x01,                    /*   USAGE_PAGE (Generic Desktop) */
    0x09, 0x39,                    /*   USAGE (Hat switch) */
    0x09, 0x39,                    /*   USAGE (Hat switch) */
    0x15, 0x01,                    /*   LOGICAL_MINIMUM (1) */
    0x25, 0x08,                    /*   LOGICAL_MAXIMUM (8) */
    0x95, 0x02,                    /*   REPORT_COUNT (2) */
    0x75, 0x04,                    /*   REPORT_SIZE (4) */
    0x81, 0x02,                    /*   INPUT (Data,Var,Abs) */
    0xc0                           /* END_COLLECTION */
};

// MinimalGamepad Implementation
MinimalGamepad::MinimalGamepad() : PluggableUSBModule(1, 1, epType), protocol(HID_REPORT_PROTOCOL), idle(1) {
    epType[0] = EP_TYPE_INTERRUPT_IN;
    PluggableUSB().plug(this);
}

void MinimalGamepad::begin() {
    // Clear report
    memset(&_report, 0, sizeof(_report));
}

void MinimalGamepad::xAxis(int16_t value) {
    _report.xAxis = value;
    write();
}

void MinimalGamepad::yAxis(int16_t value) {
    _report.yAxis = value;
    write();
}

void MinimalGamepad::zAxis(int8_t value) {
    _report.zAxis = value;
    write();
}

void MinimalGamepad::press(uint8_t button) {
    if (button >= 1 && button <= 32) {
        _report.buttons |= (1UL << (button - 1));
        write();
    }
}

void MinimalGamepad::release(uint8_t button) {
    if (button >= 1 && button <= 32) {
        _report.buttons &= ~(1UL << (button - 1));
        write();
    }
}

void MinimalGamepad::write() {
    SendReport(&_report, sizeof(_report));
}

int MinimalGamepad::getInterface(uint8_t* interfaceCount) {
    *interfaceCount += 1; // uses 1
    HIDDescriptor hidInterface = {
        D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
        D_HIDREPORT(sizeof(_hidReportDescriptorGamepad)),
        D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01)
    };
    return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int MinimalGamepad::getDescriptor(USBSetup& setup) {
    // Check if this is a HID Class Descriptor request
    if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
    if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) { return 0; }

    // In a HID Class Descriptor wIndex contains the interface number
    if (setup.wIndex != pluggedInterface) { return 0; }

    // Reset the protocol on reenumeration
    protocol = HID_REPORT_PROTOCOL;

    return USB_SendControl(TRANSFER_PGM, _hidReportDescriptorGamepad, sizeof(_hidReportDescriptorGamepad));
}

bool MinimalGamepad::setup(USBSetup& setup) {
    if (pluggedInterface != setup.wIndex) {
        return false;
    }

    uint8_t request = setup.bRequest;
    uint8_t requestType = setup.bmRequestType;

    if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE) {
        if (request == HID_GET_REPORT) {
            return true;
        }
        if (request == HID_GET_PROTOCOL) {
            return true;
        }
    }

    if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
        if (request == HID_SET_PROTOCOL) {
            protocol = setup.wValueL;
            return true;
        }
        if (request == HID_SET_IDLE) {
            idle = setup.wValueH;
            return true;
        }
        if (request == HID_SET_REPORT) {
            return true;
        }
    }

    return false;
}

void MinimalGamepad::SendReport(void* data, int length) {
    USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, length);
}

// BootKeyboardClass Implementation
void BootKeyboardClass::begin() {
    memset(_keyReport, 0, sizeof(_keyReport));
}

void BootKeyboardClass::press(uint8_t key) {
    // Handle modifier keys
    if (key >= 0x80 && key <= 0x87) {
        _keyReport[0] |= (1 << (key - 0x80));
    } else {
        // Find empty slot for regular key
        for (uint8_t i = 2; i < 8; i++) {
            if (_keyReport[i] == 0) {
                _keyReport[i] = key;
                break;
            }
        }
    }
    sendReport();
}

void BootKeyboardClass::release(uint8_t key) {
    // Handle modifier keys
    if (key >= 0x80 && key <= 0x87) {
        _keyReport[0] &= ~(1 << (key - 0x80));
    } else {
        // Remove regular key
        for (uint8_t i = 2; i < 8; i++) {
            if (_keyReport[i] == key) {
                _keyReport[i] = 0;
                break;
            }
        }
    }
    sendReport();
}

void BootKeyboardClass::sendReport() {
    HID().SendReport(1, _keyReport, sizeof(_keyReport));
}

// SingleConsumerClass Implementation
void SingleConsumerClass::begin() {
    _consumerReport = 0;
}

void SingleConsumerClass::press(uint16_t key) {
    _consumerReport = key;
    sendReport();
}

void SingleConsumerClass::release(uint16_t key) {
    if (_consumerReport == key) {
        _consumerReport = 0;
        sendReport();
    }
}

void SingleConsumerClass::sendReport() {
    HID().SendReport(2, &_consumerReport, sizeof(_consumerReport));
}

// Global instances
MinimalGamepad Gamepad1;
BootKeyboardClass BootKeyboard;
SingleConsumerClass SingleConsumer;