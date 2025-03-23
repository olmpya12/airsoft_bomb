#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "config.h"
#include <EEPROM.h>

class Settings {
private:
    byte gameMode;        // 0: Defuse, 1: Domination
    int defuseTime;       // Time in seconds for defuse mode
    int domTime;          // Score threshold for domination mode
    byte codeLength;      // Length of code for defuse mode
    char code[8];         // Code for defuse mode (max 7 chars + null terminator)
    
    // Default values - Don't use macros to avoid conflict
    static const int DEFAULT_DEFUSE_TIME_MINUTES = 5; // minutes
    static const int DEFAULT_DOM_TIME_MINUTES = 10;   // minutes
    static const byte DEFAULT_CODE_LENGTH = 4;
    
    // EEPROM read/write helpers
    void writeInt(int addr, int value);
    int readInt(int addr);
    
public:
    Settings();
    void load();
    void save();
    void reset();
    
    // Getters
    byte getGameMode() const { return gameMode; }
    int getDefuseTime() const { return defuseTime; }
    int getDomTime() const { return domTime; }
    byte getCodeLength() const { return codeLength; }
    const char* getCode() const { return code; }
    
    // Setters
    void setGameMode(byte mode);
    void setDefuseTime(int seconds);
    void setDomTime(int score);
    void setCodeLength(byte length);
    void setCode(const char* newCode);
};

#endif // SETTINGS_H