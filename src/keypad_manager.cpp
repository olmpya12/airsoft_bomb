#include "keypad_manager.h"

KeypadManager::KeypadManager(uint8_t address) : i2cAddress(address), portState(0xFF) {
    // Constructor - initialize port state to all HIGH (inputs with pull-ups)
}

void KeypadManager::writePort(uint8_t value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(value);
    Wire.endTransmission();
    portState = value;
}

uint8_t KeypadManager::readPort() {
    Wire.requestFrom(i2cAddress, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0xFF;  // Default to all HIGH if read fails
}

void KeypadManager::pinMode_I2C(uint8_t pin, uint8_t mode) {
    // For PCF8574, pins are inputs when HIGH and outputs when LOW
    if (mode == INPUT || mode == INPUT_PULLUP) {
        // Set pin HIGH for input with pull-up
        portState |= (1 << pin);  // Set bit to 1
    } else {
        // No specific action needed for OUTPUT as we'll control the pin state
        // when we write to it
    }
    writePort(portState);
}

void KeypadManager::digitalWrite_I2C(uint8_t pin, uint8_t value) {
    if (value == HIGH) {
        portState |= (1 << pin);  // Set bit to 1
    } else {
        portState &= ~(1 << pin); // Clear bit to 0
    }
    writePort(portState);
}

uint8_t KeypadManager::digitalRead_I2C(uint8_t pin) {
    uint8_t portValue = readPort();
    uint8_t result = (portValue & (1 << pin)) ? HIGH : LOW;

    return result;
}

void KeypadManager::init() {
 
    
    // Initial state - all pins HIGH (inputs with pull-ups)
    writePort(0xFF);
   
    
    // Initialize row pins as output and set to HIGH
    for (int i = 0; i < 4; i++) {
        pinMode_I2C(rowPins[i], OUTPUT);
        digitalWrite_I2C(rowPins[i], HIGH);
    }
  
    // Initialize column pins as input with pull-up resistors
    for (int i = 0; i < 3; i++) {
        pinMode_I2C(colPins[i], INPUT_PULLUP);


    }

}

char KeypadManager::scanKeypad() {
    char key = 0;
    static unsigned long lastDebugTime = 0;
    bool debugThisScan = (millis() - lastDebugTime) > 5000; // Debug output every 5 seconds
  
    if (debugThisScan) {

        lastDebugTime = millis();
    }
  
    // Ensure all rows are set to HIGH before we start scanning
    for (int i = 0; i < 4; i++) {
        digitalWrite_I2C(rowPins[i], HIGH);
    }
  
    // Add a small delay to let signals stabilize
    delayMicroseconds(500);
  
    // Scan rows
    for (int r = 0; r < 4; r++) {
        // Set current row to LOW
        digitalWrite_I2C(rowPins[r], LOW);
    
        // Add a small delay to let the signal stabilize
        delayMicroseconds(500);
    
        // Check each column
        for (int c = 0; c < 3; c++) {
            // If a key is pressed, column will read LOW
            uint8_t colState = digitalRead_I2C(colPins[c]);
            if (colState == LOW) {
                // Better debounce
                delay(20);
            
                // Read multiple times to confirm
                int readCount = 0;
                for (int i = 0; i < 5; i++) {
                    if (digitalRead_I2C(colPins[c]) == LOW) {
                        readCount++;
                    }
                    delay(1);
                }
            
                
                // Only register if most readings were LOW
                if (readCount >= 3) {
                    key = keypadLayout[r][c];

                    unsigned long keyPressStartTime = millis();
                    while (digitalRead_I2C(colPins[c]) == LOW) {
                        delay(10);
                        // Add timeout to prevent infinite loop
                        if (millis() - keyPressStartTime > 3000) {
                            
                            break;
                        }
                    }

                }
            }
        }
    
        // Set current row back to HIGH
        digitalWrite_I2C(rowPins[r], HIGH);
        if (debugThisScan) {

        }
    
        // If a key was pressed, break out of the loop
        if (key != 0) {
            break;
        }
    }

    // Update last key if a new key was pressed
    if (key != 0) {
        lastKey = key;
    } else if (lastKey != 0) {
        // Key was released

        lastKey = 0;
    }
    
    if (debugThisScan && key == 0) {
    }
    
    return key;
}
