#ifndef CONFIG_H
#define CONFIG_H

#ifndef D0
#define D0 16  // GPIO16
#define D1 5   // GPIO5
#define D2 4   // GPIO4
#define D3 0   // GPIO0
#define D4 2   // GPIO2
#define D5 14  // GPIO14
#define D6 12  // GPIO12
#define D7 13  // GPIO13
#define D8 15  // GPIO15
#define D9 3   // GPIO3 (RX)
#define D10 1  // GPIO1 (TX)
#define RX 3   // GPIO3
#define TX 1   // GPIO1
#define SD3 10 // GPIO10
#endif

// Pin definitions for ESP8266
// Using NodeMCU/ESP8266 GPIO numbers instead of Arduino pin numbers
#define PIN_BUTTON_MENU D3     // Button for menu navigation 
#define PIN_BUTTON_CONFIRM D4  // Button for confirming selections
#define PIN_BUZZER D0          // Buzzer for alarms and feedback
#define PIN_LED_RED D1         // Red LED
#define PIN_LED_GREEN D2       // Green LED
#define PIN_VOLTAGE_SENSOR A0  // Only analog pin on ESP8266

// Keypad pin connections for ESP8266
#define PIN_ROW1 D5   
#define PIN_ROW2 D6   
#define PIN_ROW3 D7   
#define PIN_ROW4 D8   
#define PIN_COL1 RX   // GPIO3
#define PIN_COL2 TX   // GPIO1
#define PIN_COL3 D9   // GPIO3/RX (if available) or another GPIO

// DFPlayer Mini pins
// Using software serial with ESP8266
#define DFPLAYER_RX_PIN D10  // Connect to TX pin on DFPlayer Mini
#define DFPLAYER_TX_PIN SD3  // Connect to RX pin on DFPlayer Mini

// Hardware SPI pins for Arduino UNO are fixed:
// MOSI - Pin 11 (fixed)
// SCK  - Pin 13 (fixed)
// SS   - Pin 10 (fixed, but can be ignored if not using multiple devices)

#define OLED_MOSI  13   //D7
#define OLED_CLK   14   //D5
#define OLED_DC    2  // D2  
#define OLED_CS    15  //D8
#define OLED_RESET 16  //D0

// Display settings
#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels

// Game mode definitions
enum GameMode {
  DEFUSE_MODE = 0,     // Defuse the bomb before time runs out
  DOMINATION_MODE = 1  // Capture and hold the point
};

// Game state definitions
enum GameState {
  SETUP,     // Initial setup/configuration
  READY,     // Ready to start game
  RUNNING,   // Game in progress
  PAUSED,    // Game paused
  FINISHED,  // Game ended
  SETTINGS,   // Settings menu
  GAME_OVER
};

// Sound types
enum SoundType {
  SOUND_STARTUP = 0,
  SOUND_GAME_START = 1,
  SOUND_EXPLOSION = 2,
  SOUND_DEFUSED = 3,
  SOUND_BEEP = 4,
  SOUND_BUTTON_PRESS = 5,
  SOUND_ERROR = 6,
  SOUND_WARNING = 7,
  // Add more as needed
};

// Game settings defaults - Avoid redefinition conflict with settings.h
// Use different names to avoid redefinition
#define CONFIG_DEFUSE_TIME_DEFAULT 300  // 5 minutes for defuse mode
#define CONFIG_DOM_TIME_DEFAULT 100     // Score threshold for domination mode

// EEPROM addresses
#define EEPROM_SETTINGS_START 0   // Start address for settings in EEPROM

// Game mode selection switch
#define PIN_MODE_SWITCH 27  // GPIO pin for game mode selection

// Team buttons for domination mode
#define PIN_RED_BUTTON D1   // Red team button
#define PIN_GREEN_BUTTON D2 // Green team button

// Domination game constants
#define DOM_DEFAULT_TIME 1       // Default time in minutes
#define DOM_TIME_INCREMENT 5      // Time increment/decrement in minutes
#define DOM_MIN_TIME 5            // Minimum time in minutes
#define DOM_MAX_TIME 60           // Maximum time in minutes
#define DOM_CAPTURE_TIME 1000     // Time in ms to capture a point (fill slider)

#endif // CONFIG_H