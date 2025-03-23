#include "settings.h"
#include <EEPROM.h>
#include <Arduino.h>

Settings::Settings() {
    // Set default values
    reset();
}

void Settings::load() {
    // Read game mode
    gameMode = EEPROM.read(EEPROM_SETTINGS_START);
    
    // Validate game mode
    if (gameMode > DOMINATION_MODE) {
        gameMode = DEFUSE_MODE;
    }
    
    // Read defuse time
    defuseTime = readInt(EEPROM_SETTINGS_START + 1);
    
    // Validate defuse time
    if (defuseTime < 60 || defuseTime > 3600) {
        defuseTime = DEFAULT_DEFUSE_TIME_MINUTES * 60;
    }
    
    // Read domination time
    domTime = readInt(EEPROM_SETTINGS_START + 3);
    
    // Validate domination time
    if (domTime < 60 || domTime > 3600) {
        domTime = DEFAULT_DOM_TIME_MINUTES * 60;
    }
    
    // Read code length
    codeLength = EEPROM.read(EEPROM_SETTINGS_START + 5);
    
    // Validate code length
    if (codeLength < 1 || codeLength > 7) {
        codeLength = DEFAULT_CODE_LENGTH;
    }
    
    // Read code
    for (int i = 0; i < codeLength; i++) {
        code[i] = EEPROM.read(EEPROM_SETTINGS_START + 6 + i);
        
        // Validate char is a digit
        if (code[i] < '0' || code[i] > '9') {
            code[i] = '0';
        }
    }
    
    // Null terminate
    code[codeLength] = '\0';
}

void Settings::save() {
    // Write game mode
    EEPROM.write(EEPROM_SETTINGS_START, gameMode);
    
    // Write defuse time
    writeInt(EEPROM_SETTINGS_START + 1, defuseTime);
    
    // Write domination time
    writeInt(EEPROM_SETTINGS_START + 3, domTime);
    
    // Write code length
    EEPROM.write(EEPROM_SETTINGS_START + 5, codeLength);
    
    // Write code
    for (int i = 0; i < codeLength; i++) {
        EEPROM.write(EEPROM_SETTINGS_START + 6 + i, code[i]);
    }
}

void Settings::reset() {
    gameMode = DEFUSE_MODE;
    defuseTime = DEFAULT_DEFUSE_TIME_MINUTES * 60; // Convert minutes to seconds
    domTime = DEFAULT_DOM_TIME_MINUTES * 60;       // Convert minutes to seconds  
    codeLength = DEFAULT_CODE_LENGTH;
    strcpy(code, "1234");  // Default code
}

void Settings::writeInt(int addr, int value) {
    EEPROM.write(addr, value & 0xFF);
    EEPROM.write(addr + 1, (value >> 8) & 0xFF);
}

int Settings::readInt(int addr) {
    return EEPROM.read(addr) | (EEPROM.read(addr + 1) << 8);
}

void Settings::setGameMode(byte mode) {
    if (mode <= DOMINATION_MODE) {
        gameMode = mode;
    }
}

void Settings::setDefuseTime(int seconds) {
    if (seconds >= 60 && seconds <= 3600) {
        defuseTime = seconds;
    }
}

void Settings::setDomTime(int score) {
    if (score >= 60 && score <= 3600) {
        domTime = score;
    }
}

void Settings::setCodeLength(byte length) {
    if (length >= 1 && length <= 7) {
        codeLength = length;
    }
}

void Settings::setCode(const char* newCode) {
    size_t len = strlen(newCode);
    if (len > 0 && len <= 7) {
        strncpy(code, newCode, 7);
        code[len] = '\0';
        codeLength = len;
    }
}