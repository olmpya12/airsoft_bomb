#include "game_modes.h"
#include <Arduino.h> // Add this to get millis()

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

void DefuseMode::update()
{
    // Update logic for defuse mode
}

void DefuseMode::handleInput(int button)
{
    // Handle input logic for defuse mode
}

bool DefuseMode::isGameOver()
{
    // Game over logic for defuse mode
    return false;
}

void DefuseMode::reset()
{
    startTime = 0;
    timeLimit = 300; // Default 5 minutes
    armed = false;
    codePosition = 0;

    // Initialize default defuse code (e.g., 1234)
    for (int i = 0; i < 4; i++)
    {
        defuseCode[i] = i + 1;
        inputCode[i] = 0;
    }
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

void DominationMode::update()
{
    if (state == SETUP)
    {
        return; // In setup mode, nothing to update
    }

    if (state == RUNNING)
    {
        unsigned long currentTime = millis();

        // Update elapsed time
        elapsedTime = (currentTime - startTime) / 1000; // Convert to seconds

        // CHANGE THIS SECTION: Always update capture, don't check captureStartTime here
        updateCapture(); // Always check for capture updates

        // Update team scores based on point ownership
        updateScores();

        // Check if game is over
        if (elapsedTime >= gameTime)
        {
            state = GAME_OVER;
            return;
        }
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