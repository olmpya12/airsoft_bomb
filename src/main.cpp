#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include "config.h"
#include "game_modes.h"
#include "display_manager.h"
#include "sound_manager.h"
#include "settings.h"
#include "keypad_manager.h"
#include "voltage_monitor.h"


// Global variables
GameMode currentMode = DEFUSE_MODE;
GameState currentState = SETUP;
GameBase* activeGame = nullptr;
DefuseMode defuseGame;
DominationMode dominationGame;

DisplayManager display;
SoundManager sound;
Settings settings;
KeypadManager keypad;
VoltageMonitor voltage;

// Keep track of last voltage reading time
unsigned long lastVoltageCheck = 0;
const unsigned long VOLTAGE_CHECK_INTERVAL = 10000;  // Check every 10 seconds

// Add button state variables
bool redButtonState = false;
bool greenButtonState = false;
bool lastRedButtonState = false;
bool lastGreenButtonState = false;




void setup() {
  Serial.begin(115200);
  Serial.println("\nAirsoft Bomb");
  delay(1000); // Give serial a moment to initialize
  
  // Initialize SPI for the display
  SPI.begin();
  Serial.println("SPI initialized");
  
  // Test OLED display first before anything else
  Serial.println("Testing display initialization...");
  if (!display.init()) {
    Serial.println("Display initialization failed!");
  }
  Serial.println("Display initialized successfully");
  
   
  // Initialize mode switch and team buttons
  pinMode(PIN_MODE_SWITCH, INPUT_PULLUP);
  pinMode(PIN_RED_BUTTON, INPUT_PULLUP);
  pinMode(PIN_GREEN_BUTTON, INPUT_PULLUP);
  
  // Initialize managers
  settings.load();
  // Skip sound initialization for now
  // sound.init();
  display.showWelcome();
  
  // Determine initial game mode from switch
  currentMode = digitalRead(PIN_MODE_SWITCH) ? DEFUSE_MODE : DOMINATION_MODE;
  
  // Initialize the appropriate game based on switch position
  if (currentMode == DEFUSE_MODE) {
    activeGame = &defuseGame;
    Serial.println("Starting in Defuse Mode");
  } else {
    activeGame = &dominationGame;
    Serial.println("Starting in Domination Mode");
  }
  
  activeGame->init();
  display.showGameMode(currentMode);
  delay(2000);
  activeGame->handleButton('#'); // Start the game
  Serial.println("Airsoft Bomb System Initialized");
}

void loop() {
 
  
  // Check if game mode switch has changed
  GameMode switchMode = digitalRead(PIN_MODE_SWITCH) ? DEFUSE_MODE : DOMINATION_MODE;

  
  if (switchMode != currentMode) {
    // Mode has changed, reset and initialize the new mode
    currentMode = switchMode;
    
    if (currentMode == DEFUSE_MODE) {
      activeGame = &defuseGame;
      Serial.println("Switched to Defuse Mode");
    } else {
      activeGame = &dominationGame;
      Serial.println("Switched to Domination Mode");
    }
    

    activeGame->init();

    

    display.showGameMode(currentMode);
    
    delay(1000);
  }
  
  // Handle keypad input

  char key = 0;

  
  // If a key is pressed from the keypad
  if (key != 0) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    // Pass the key to the active game
    activeGame->handleButton(key);
  }
  

  
  // Read team buttons for domination mode
  if (currentMode == DOMINATION_MODE) {
    redButtonState = !digitalRead(PIN_RED_BUTTON);    // Inverted because of pull-up
    greenButtonState = !digitalRead(PIN_GREEN_BUTTON); // Inverted because of pull-up
    
    // Track button state changes for domination game
    DominationMode* domGame = (DominationMode*)activeGame;
    
    // Debug prints for button states
    static bool printedLastState = false;
    if (redButtonState || greenButtonState) {
      if (!printedLastState) {
        Serial.print("Button held - Red: ");
        Serial.print(redButtonState);
        Serial.print(", Green: ");
        Serial.println(greenButtonState);
        printedLastState = true;
      }
    } else {
      printedLastState = false;
    }
    
    // Check for red button press
    if (redButtonState && !lastRedButtonState) {
      Serial.println("Red button pressed");
      domGame->handleButton('R');
    }
    
    // Check for red button release
    if (!redButtonState && lastRedButtonState) {
      Serial.println("Red button released");
      domGame->handleButton('r');  // Lowercase 'r' for release
    }
    
    // Check for green button press
    if (greenButtonState && !lastGreenButtonState) {
      Serial.println("Green button pressed");
      domGame->handleButton('G');
    }
    
    // Check for green button release
    if (!greenButtonState && lastGreenButtonState) {
      Serial.println("Green button released");
      domGame->handleButton('g');  // Lowercase 'g' for release
    }
    domGame->updateButtonStates(redButtonState, greenButtonState);
    
    lastRedButtonState = redButtonState;
    lastGreenButtonState = greenButtonState;
  }
  

  
  // Update the active game state

  activeGame->update();

  
  if (currentMode == DEFUSE_MODE) {
    // Update defuse mode display
    // ...
  } else {
    // Update domination mode display
    DominationMode* domGame = (DominationMode*)activeGame;
    
    if (domGame->state == SETUP) {
      // Show setup screen with time selection
      display.showDominationSetup(domGame->getGameTime() / 60);
    } else if (domGame->state == RUNNING) {
      // Show game progress
      int remainingTime = domGame->getGameTime() - domGame->getElapsedTime();
      
      // Ensure you have a getter for capture progress
      int captureProgress = domGame->getCaptureProgress();
      PointOwnership owner = domGame->getCurrentOwner();
      
      display.showDominationScreen(domGame->getRedScore(), domGame->getGreenScore(), 
                                 captureProgress, owner, remainingTime);
    } else if (domGame->state == GAME_OVER) {
      // Show game over with winner
      PointOwnership winner = (domGame->getRedScore() > domGame->getGreenScore()) ? 
                            RED_TEAM : 
                            (domGame->getGreenScore() > domGame->getRedScore()) ? GREEN_TEAM : NEUTRAL;
      display.showDominationGameOver(winner, domGame->getRedScore(), domGame->getGreenScore());
    }
  }
  
  delay(10); // Small delay to prevent CPU hogging
}