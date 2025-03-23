#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "config.h"
#include "game_modes.h"

class DisplayManager {
private:
    Adafruit_SH1106G display;
    bool initialized;

public:
    DisplayManager();
    bool init();
    
    // Basic display functions
    void clear();
    void update();
    void showCenteredText(const String& text, int y, int size = 1);
    
    // Game-specific screens
    void showWelcome();
    void showMenu(const String& title, const String items[], int numItems, int selectedIndex);
    void showGameMode(GameMode mode);
    void showCountdown(int timeRemaining);
    void showDefuseScreen(int timeRemaining, bool armed, const String& code = "");
    void showDominationScreen(int redScore, int blueScore, int threshold);
    void showGameOver(bool victory);
    void showSettings(const String& setting, const String& value);
    void showPassword(const String& password, bool hidden = true);
    void showBatteryStatus(float voltage);
    void showError(const String& message);
    
    // Domination mode specific screens
    void showDominationSetup(int minutes);
    void showDominationScreen(int redScore, int greenScore, int captureProgress, 
                              PointOwnership currentOwner, int remainingTime);
    void showDominationGameOver(PointOwnership winner, int redScore, int greenScore);
};

#endif // DISPLAY_MANAGER_H