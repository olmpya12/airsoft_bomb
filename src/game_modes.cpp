#include "game_modes.h"
#include <Arduino.h> // Add this to get millis()
#include <display_manager.h>
#include <sound_manager.h>

// GameBase implementation
GameBase::GameBase()
{
    // Base constructor implementation
}

// DefuseMode implementation
DefuseMode::DefuseMode()
{
    reset();
}

void DefuseMode::init()
{
    reset();
}

void DefuseMode::update() {
    String codeStr = "";
    for (int i = 0; i < codePosition; i++) {
        codeStr += String(inputCode[i]);
    }

    if (state == WAITING_TO_ARM) {
        // Show DISARMED screen with code input
        display->showDefuseScreen(timeLimit, false, codeStr);
        return;
    }

    // If ARMED
    unsigned long elapsed = (millis() - startTime) / 1000;
    int remaining = timeLimit - elapsed;

    if (remaining <= 0) {
        // Time's up – explosion
        sound->play(SOUND_EXPLOSION);
        display->showGameOver(false);  // Mission failed
        delay(5000);
        reset();
        return;
    }

    display->showDefuseScreen(remaining, true, codeStr);
}

void DefuseMode::setManagers(DisplayManager* d, SoundManager* s) {
    display = d;
    sound = s;
}

void DefuseMode::handleInput(int button) {
    if (button >= 0 && button <= 9) {
        if (codePosition < 4) {
            inputCode[codePosition++] = button;
            sound->play(SOUND_BUTTON_PRESS);
        }
        return;
    }

    if (button == 10) { // * = clear
        codePosition = 0;
        sound->play(SOUND_BEEP);
        return;
    }

    if (button == 11) { // # = submit
        bool correct = true;

        if (codePosition < 4) {
            sound->play(SOUND_ERROR);
            Serial.println("Code too short.");
            codePosition = 0;
            return;
        }

        // Compare code
        for (int i = 0; i < 4; i++) {
            int expected = (state == WAITING_TO_ARM) ? armingCode[i] : defuseCode[i];
            if (inputCode[i] != expected) {
                correct = false;
                break;
            }
        }

        if (correct) {
            if (state == WAITING_TO_ARM) {
                state = ARMED;
                startTime = millis();
                sound->play(SOUND_GAME_START);
                Serial.println("Bomb armed!");
            } else if (state == ARMED) {
                state = WAITING_TO_ARM;
                sound->play(SOUND_DEFUSED);
                display->showGameOver(true);  // Victory
                Serial.println("Bomb defused!");
                delay(5000);
                reset();
            }
        } else {
            sound->play(SOUND_ERROR);
            Serial.println("Incorrect code!");
        }

        codePosition = 0;
    }
}

bool DefuseMode::isGameOver()
{
    // Game over logic for defuse mode
    return false;
}

void DefuseMode::reset() {
    startTime = 0;
    timeLimit = 300; // 5 min default
    codePosition = 0;
    state = WAITING_TO_ARM;
}

void DefuseMode::setTimeLimit(int seconds)
{
    timeLimit = seconds;
}

// Add this method to implement the abstract method from base class
void DefuseMode::handleButton(char button)
{
    // Convert char to int and pass to handleInput
    if (button >= '0' && button <= '9')
    {
        handleInput(button - '0');
    }
    else
    {
        // Handle special buttons
        if (button == '*')
            handleInput(10);
        if (button == '#')
            handleInput(11);
    }
}

// DominationMode implementation
DominationMode::DominationMode()
{
    reset();
}

void DominationMode::init()
{
    reset();
    state = SETUP;
    setupComplete = false;
}
void DominationMode::setManagers(DisplayManager* d, SoundManager* s) {
    display = d;
    sound = s;
}
void DominationMode::update()
{
    if (state == SETUP)
    {
        display->showDominationSetup(getGameTime() / 60);
    }

    if (state == RUNNING)
    {
        unsigned long currentTime = millis();

        // Update elapsed time
        elapsedTime = (currentTime - startTime) / 1000; // Convert to seconds

        updateCapture();
        updateScores();

        int remainingTime = gameTime - elapsedTime;
        int captureProgress = getCaptureProgress();
        PointOwnership owner = getCurrentOwner();

        display->showDominationScreen(redScore, greenScore, captureProgress, owner, remainingTime);

        if (elapsedTime >= gameTime)
        {
            state = GAME_OVER;
            return;
        }
    }
    else if (state == GAME_OVER)
    {
        PointOwnership winner = (redScore > greenScore) ? RED_TEAM :
                                (greenScore > redScore) ? GREEN_TEAM : NEUTRAL;
        display->showDominationGameOver(winner, redScore, greenScore);
    }
}

void DominationMode::handleInput(int button)
{
    // Handle input logic for domination mode
    // This can be a simple pass-through to handleButton
    handleButton((char)(button + '0')); // Convert int to char
}

bool DominationMode::isGameOver()
{
    // Game over logic for domination mode
    return state == GAME_OVER;
}

void DominationMode::reset()
{
    gameTime = DOM_DEFAULT_TIME * 60; // Convert to seconds
    startTime = 0;
    elapsedTime = 0;
    currentOwner = NEUTRAL;
    captureStartTime = 0;
    capturingTeam = NEUTRAL;
    captureProgress = 0;
    redScore = 0;
    greenScore = 0;
    lastScoreUpdate = 0;
    setupComplete = false;
    state = SETUP;
    redButtonHeld = false;
    greenButtonHeld = false;
}

void DominationMode::setWinThreshold(int seconds)
{
    // You could implement this if needed
}

void DominationMode::updateScores()
{
    unsigned long currentTime = millis();

    // Update scores once per second
    if (currentTime - lastScoreUpdate >= 1000)
    {
        if (currentOwner == RED_TEAM)
        {
            redScore++;
        }
        else if (currentOwner == GREEN_TEAM)
        {
            greenScore++;
        }
        lastScoreUpdate = currentTime;
    }
}

void DominationMode::updateCapture()
{
    // Debug entry point

    // Only update capture if a button is being held
    if (!redButtonHeld && !greenButtonHeld)
    {
        Serial.println("No buttons held, stopping capture");
        // No button held, don't progress capture
        return;
    }

    // If someone is trying to capture a point they already own, ignore
    if ((redButtonHeld && currentOwner == RED_TEAM) ||
        (greenButtonHeld && currentOwner == GREEN_TEAM))
    {
        Serial.println("Trying to capture already owned point");
        return;
    }

    // Continue with capture progress if we have a start time
    if (captureStartTime > 0)
    {
        unsigned long currentTime = millis();
        unsigned long captureDuration = currentTime - captureStartTime;

        // Calculate progress percentage
        int oldProgress = captureProgress;
        captureProgress = (int)(captureDuration * 100 / DOM_CAPTURE_TIME);

        if (captureProgress != oldProgress)
        {

            Serial.print(captureProgress);
        }

        // Check if capture complete
        if (captureProgress >= 100)
        {
            Serial.println("Capture complete!");
            captureProgress = 100;
            currentOwner = capturingTeam;
            captureStartTime = 0; // Reset capture timer
        }
    }
}

void DominationMode::handleButton(char button)
{
    if (state == SETUP)
    {
        // In setup mode, use buttons to adjust game time
        if (button == '#')
        {
            // Start game
            startTime = millis();
            lastScoreUpdate = startTime;
            state = RUNNING;
        }
        else if (button == 'R')
        {
            // Red button decreases time
            setupGameTime(false);
        }
        else if (button == 'G')
        {
            // Green button increases time
            setupGameTime(true);
        }
    }
    else if (state == RUNNING)
    {
        // We now handle the continuous button press in updateButtonStates()
        // We still need to track initial button presses for other features
        if (button == 'R')
        {
            // Red team pressed button
            // Initial press logic if needed
        }
        else if (button == 'G')
        {
            // Green team pressed button
            // Initial press logic if needed
        }
        else if (button == 'r')
        {
            // Red team released button
            // Handle release if needed
        }
        else if (button == 'g')
        {
            // Green team released button
            // Handle release if needed
        }
    }
    else if (state == GAME_OVER)
    {
        // Any button restarts
        if (button == '#')
        {
            reset();
        }
    }
}

void DominationMode::setupGameTime(bool increase)
{
    if (increase)
    {
        gameTime += DOM_TIME_INCREMENT * 60; // Add 5 minutes
        if (gameTime > DOM_MAX_TIME * 60)
        {
            gameTime = DOM_MAX_TIME * 60;
        }
    }
    else
    {
        gameTime -= DOM_TIME_INCREMENT * 60; // Subtract 5 minutes
        if (gameTime < DOM_MIN_TIME * 60)
        {
            gameTime = DOM_MIN_TIME * 60;
        }
    }
}

void DominationMode::startCapture(PointOwnership team)
{
    // Start capturing for the indicated team
    if (currentOwner != team)
    {
        captureStartTime = millis();
        captureProgress = 0;
    }
}

void DominationMode::updateButtonStates(bool redPressed, bool greenPressed)
{
    // Only handle button states in RUNNING mode
    if (state != RUNNING)
        return;

    redButtonHeld = redPressed;
    greenButtonHeld = greenPressed;

    // Continue capture if a button is being held
    if (redButtonHeld && currentOwner != RED_TEAM)
    {
        // Continue red capture
        if (captureStartTime == 0 || capturingTeam != RED_TEAM)
        { // ← CRITICAL CHANGE: check if capture not started
            // Start new capture
            Serial.println("Starting RED capture");
            captureStartTime = millis();
            capturingTeam = RED_TEAM;
        }
        // Else continue existing capture (no change needed)
    }
    else if (greenButtonHeld && currentOwner != GREEN_TEAM)
    {
        // Continue green capture
        if (captureStartTime == 0 || capturingTeam != GREEN_TEAM)
        { // ← CRITICAL CHANGE: check if capture not started
            // Start new capture
            Serial.println("Starting GREEN capture");
            captureStartTime = millis();
            capturingTeam = GREEN_TEAM;
        }
        // Else continue existing capture (no change needed)
    }
    else
    {
        // No buttons held or trying to capture already-owned point
        if (captureProgress > 0 && captureProgress < 100)
        {
            // Reset partial capture
            Serial.println("Resetting partial capture");
            captureProgress = 0;
            captureStartTime = 0;
        }
    }
}

// Add these getter methods after the existing DominationMode methods

int DominationMode::getCaptureProgress()
{
    return captureProgress;
}

PointOwnership DominationMode::getCurrentOwner()
{
    return currentOwner;
}

int DominationMode::getRedScore()
{
    return redScore;
}

int DominationMode::getGreenScore()
{
    return greenScore;
}

int DominationMode::getGameTime()
{
    return gameTime;
}

int DominationMode::getElapsedTime()
{
    return elapsedTime;
}

PointOwnership DominationMode::getCapturingTeam()
{
    return capturingTeam;
}