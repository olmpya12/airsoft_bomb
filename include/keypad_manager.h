#ifndef KEYPAD_MANAGER_H
#define KEYPAD_MANAGER_H

#include <Arduino.h>
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

    // Row and column pins
    const int rowPins[4] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};
    const int colPins[3] = {PIN_COL1, PIN_COL2, PIN_COL3};
    
    char lastKey = 0;  // Variable to store the last key pressed

public:
    KeypadManager();
    void init();
    char scanKeypad();
    char getLastKey() { return lastKey; }
};

#endif // KEYPAD_MANAGER_H