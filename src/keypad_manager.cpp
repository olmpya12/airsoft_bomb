#include "keypad_manager.h"

KeypadManager::KeypadManager() {
    // Constructor
}

void KeypadManager::init() {
    // Initialize row pins as output and set to HIGH
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
  
    // Initialize column pins as input with pull-up resistors
    for (int i = 0; i < 3; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }
}

char KeypadManager::scanKeypad() {
    char key = 0;
  
    // Ensure all rows are set to HIGH before we start scanning
    for (int i = 0; i < 4; i++) {
        digitalWrite(rowPins[i], HIGH);
    }
  
    // Add a small delay to let signals stabilize
    delayMicroseconds(500);
  
    // Scan rows
    for (int r = 0; r < 4; r++) {
        // Set current row to LOW
        digitalWrite(rowPins[r], LOW);
    
        // Add a small delay to let the signal stabilize
        delayMicroseconds(500);
    
        // Check each column
        for (int c = 0; c < 3; c++) {
            // If a key is pressed, column will read LOW
            if (digitalRead(colPins[c]) == LOW) {
                // Better debounce
                delay(20);
            
                // Read multiple times to confirm
                int readCount = 0;
                for (int i = 0; i < 5; i++) {
                    if (digitalRead(colPins[c]) == LOW) {
                        readCount++;
                    }
                    delay(1);
                }
            
                // Only register if most readings were LOW
                if (readCount >= 3) {
                    key = keypadLayout[r][c];
                
                    // Wait for key release to prevent multiple detections
                    while (digitalRead(colPins[c]) == LOW) {
                        delay(10);
                    }
                }
            }
        }
    
        // Set current row back to HIGH
        digitalWrite(rowPins[r], HIGH);
    
        // If a key was pressed, break out of the loop
        if (key != 0) {
            break;
        }
    }

    // Update last key if a new key was pressed
    if (key != 0) {
        lastKey = key;
    } else {
        // Reset lastKey when no key is pressed
        lastKey = 0;
    }
  
    return key;
}