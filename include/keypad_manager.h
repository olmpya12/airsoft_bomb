#ifndef KEYPAD_MANAGER_H
#define KEYPAD_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "config.h"


class KeypadManager {
private:
    // Keypad layout
    const char keypadLayout[4][3] = {
        {'1', '2', '3'},
        {'4', '5', '6'},
        {'7', '8', '9'},
        {'*', '0', '#'}
    };

    // Row and column pins (these are now PCF8574 port pins)
    const uint8_t rowPins[4] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};
    const uint8_t colPins[3] = {PIN_COL1, PIN_COL2, PIN_COL3};
    
    uint8_t i2cAddress;       // I2C address of the PCF8574
    uint8_t portState;        // Current state of the I2C expander port
    
    char lastKey = 0;         // Variable to store the last key pressed

    // I2C helper methods
    void writePort(uint8_t value);
    uint8_t readPort();
    void pinMode_I2C(uint8_t pin, uint8_t mode);
    void digitalWrite_I2C(uint8_t pin, uint8_t value);
    uint8_t digitalRead_I2C(uint8_t pin);

public:
    KeypadManager(uint8_t address = PCF8574_ADDRESS);
    void init();
    char scanKeypad();
    char getLastKey() { return lastKey; }
};

#endif // KEYPAD_MANAGER_H