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
  
  Wire.begin(D6,D7);

  if (!sound.init())
  {  
    ESP.restart();
  }
  // Test OLED display first before anything else
  Serial.println("Testing display initialization...");
  if (!display.init()) {
    Serial.println("Display initialization failed!");
  }
  else{
    Serial.println("Display initialized successfully");
  }
  // Initialize I2C for keypad
  keypad.init();
  Serial.println("Keypad initialized");
   
  // Initialize mode switch and team buttons
  pinMode(PIN_MODE_SWITCH, INPUT_PULLUP);
  Serial.println("Display 2 initialized successfully");
  
  // Initialize managers
  settings.load();
  // Skip sound initialization for now
  display.showWelcome();
  
  // Determine initial game mode from switch
  // currentMode = digitalRead(PIN_MODE_SWITCH) ? DEFUSE_MODE : DOMINATION_MODE;
  currentMode = DEFUSE_MODE;
  
  // Initialize the appropriate game based on switch position
  if (currentMode == DEFUSE_MODE) {
    activeGame = &defuseGame;
    Serial.println("Starting in Defuse Mode");
  } else {
    activeGame = &dominationGame;
    Serial.println("Starting in Domination Mode");
  }
  
  activeGame->init();
  activeGame->setManagers(&display, &sound);
  
  display.showGameMode(currentMode);
  delay(2000);
  Serial.println("Airsoft Bomb System Initialized");
  pinMode(PIN_RED_BUTTON, INPUT_PULLUP);
  pinMode(PIN_GREEN_BUTTON, INPUT_PULLUP);
}

void loop() {
 
  // Handle keypad input - FIXED: Actually call the scanKeypad function
  char key = keypad.scanKeypad();

  
  // If a key is pressed from the keypad
  if (key != 0) {
    sound.play(SOUND_BEEP);
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
  delay(10); // Small delay to prevent CPU hogging
}