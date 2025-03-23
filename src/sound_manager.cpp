#include "sound_manager.h"
#include <Arduino.h>

// Use the renamed pins from config.h
SoundManager::SoundManager() : dfPlayerSerial(DFPLAYER_RX_PIN, DFPLAYER_TX_PIN), initialized(false), volume(20) {}

void SoundManager::init() {
    dfPlayerSerial.begin(9600);
    
    Serial.println("Initializing DFPlayer...");
    
    if (!dfPlayer.begin(dfPlayerSerial)) {
        Serial.println("DFPlayer initialization failed!");
        pinMode(PIN_BUZZER, OUTPUT);
    } else {
        Serial.println("DFPlayer online!");
        initialized = true;
        dfPlayer.volume(volume);
        delay(100);
    }
}

void SoundManager::play(uint8_t sound) {
    if (initialized) {
        dfPlayer.play(sound);
    } else {
        switch (sound) {
            case SOUND_STARTUP:
                tone(PIN_BUZZER, 1000, 100);
                delay(150);
                tone(PIN_BUZZER, 1500, 100);
                delay(150);
                tone(PIN_BUZZER, 2000, 100);
                break;
            case SOUND_BEEP:
                tone(PIN_BUZZER, 1000, 100);
                break;
            case SOUND_ERROR:
                tone(PIN_BUZZER, 400, 300);
                break;
            default:
                tone(PIN_BUZZER, 1000, 100);
                break;
        }
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
    } else {
        noTone(PIN_BUZZER);
    }
}