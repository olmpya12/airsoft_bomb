#include "sound_manager.h"
#include <Arduino.h>

// Use the renamed pins from config.h
SoundManager::SoundManager() : dfPlayerSerial(5, 4), initialized(false), volume(20) {}

bool SoundManager::init() {
    delay(2000);
    dfPlayerSerial.begin(9600);
    delay(1500);
    Serial.println("Initializing DFPlayer...");
    if (!dfPlayer.begin(dfPlayerSerial)) {
        Serial.println("DFPlayer initialization failed!");
        return false;
    } else {
        Serial.println("DFPlayer online!");
        initialized = true;
        dfPlayer.volume(volume);
        delay(100);
        return true;
    }
}

void SoundManager::play(uint8_t sound) {
    if (initialized) {
        dfPlayer.play(sound);
    }
}


void SoundManager::playWithVolume(uint8_t sound, uint8_t vol) {
    if (initialized) {
        uint8_t originalVolume = dfPlayer.readVolume();
        dfPlayer.volume(vol);
        dfPlayer.play(sound);
        dfPlayer.volume(originalVolume);
    } else {
        play(sound);
    }
}

void SoundManager::setVolume(uint8_t vol) {
    volume = constrain(vol, 0, 30);
    if (initialized) {
        dfPlayer.volume(volume);
    }
}

uint8_t SoundManager::getVolume() {
    return volume;
}

void SoundManager::stop() {
    if (initialized) {
        dfPlayer.stop();
    }
}