#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <Arduino.h>
#include "config.h"
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define planted_sound '00.wav';
#define defused_sound '01.wav';
#define bep_keypad_sound '02.wav';
#define bep_bomb_sound '03.wav';



// Sound effect definitions are already in config.h

class SoundManager {
private:
    SoftwareSerial dfPlayerSerial;
    DFRobotDFPlayerMini dfPlayer;
    bool initialized;
    uint8_t volume;

public:
    SoundManager();
    bool init();
    void play(uint8_t sound);
    void playWithVolume(uint8_t sound, uint8_t volume);
    void setVolume(uint8_t volume);
    uint8_t getVolume();
    void playBeepAd(uint8_t track);
    void stop();
};

#endif // SOUND_MANAGER_H