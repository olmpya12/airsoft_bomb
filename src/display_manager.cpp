#include "display_manager.h"
#include <Arduino.h>
#include "game_modes.h"

// Constructor for SPI OLED display
DisplayManager::DisplayManager() : 
    // Use hardware SPI for Arduino Uno (much more efficient)
    display(SCREEN_WIDTH, SCREEN_HEIGHT, 
            OLED_MOSI,OLED_CLK, OLED_DC, 
            OLED_RESET, OLED_CS),
    initialized(false) {}

// Make sure you're passing OLED_RESET to the constructor before CS
bool DisplayManager::init() {
    Serial.println("Initializing SH110X display...");
    
    // SPI SSD1306 initialization - Add debug output
    if(!display.begin(0,true)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    Serial.println("SSD1306 display initialized successfully!");
    initialized = true;
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.cp437(true); // Use full 256 char 'Code Page 437' font
    
    // Test the display with a simple pattern
    display.clearDisplay();
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);
    display.display(); // This is essential - must call display()
    delay(1000);
    
    // Using adjusted positions for 128x64 resolution
    display.clearDisplay();
    showCenteredText("AIRSOFT BOMB", 10, 2);
    showCenteredText("v2.0", 32, 1);
    display.display(); // Must call display() to update the screen
    delay(2000);
    
    return true;
}

void DisplayManager::clear() {
    if (!initialized) return;
    display.clearDisplay();
}

void DisplayManager::update() {
    if (!initialized) return;
    display.display();
}

void DisplayManager::showCenteredText(const String& text, int y, int size) {
    if (!initialized) return;
    
    int16_t x1, y1;
    uint16_t w, h;
    
    display.setTextSize(size);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    display.setCursor((SCREEN_WIDTH - w) / 2, y);
    display.print(text);
}

void DisplayManager::showWelcome() {
    if (!initialized) return;
    
    clear();
    // Adjusted positions and text sizes for 64px height
    showCenteredText("AIRSOFT BOMB", 10, 2);
    showCenteredText("READY", 35, 1);
    showCenteredText("Press any key", 50, 1);
    update();
}

void DisplayManager::showMenu(const String& title, const String items[], int numItems, int selectedIndex) {
    if (!initialized) return;
    
    clear();
    
    // Draw title with underline
    showCenteredText(title, 2, 1);
    // Underline for the title
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    // Now we can show multiple items with 64px height
    int maxItemsVisible = 4;
    
    // Calculate which items to show
    int startItem = max(0, selectedIndex - 1);
    if (startItem + maxItemsVisible > numItems) {
        startItem = max(0, numItems - maxItemsVisible);
    }
    
    int endItem = min(numItems, startItem + maxItemsVisible);
    
    // Display the menu items starting at y=16
    for (int i = startItem; i < endItem; i++) {
        int yPos = 16 + (i - startItem) * 12;
        
        if (i == selectedIndex) {
            // Selected item with inverted colors
            display.fillRect(0, yPos - 1, SCREEN_WIDTH, 10, SH110X_WHITE);
            display.setTextColor(SH110X_BLACK);
            display.setCursor(3, yPos);
            display.print("> " + items[i]);
            display.setTextColor(SH110X_WHITE);
        } else {
            // Regular item
            display.setCursor(3, yPos);
            display.print("  " + items[i]);
        }
    }
    
    update();
}

void DisplayManager::showGameMode(GameMode mode) {
    if (!initialized) return;
    
    clear();
    showCenteredText("GAME MODE", 2, 1);
    // Underline adjusted
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    String modeName = (mode == DEFUSE_MODE) ? "DEFUSE BOMB" : "DOMINATION";
    // Place mode name in the middle with larger text
    showCenteredText(modeName, 26, 2);
    
    if (mode == DEFUSE_MODE) {
        showCenteredText("Plant/defuse mission", 50, 1);
    } else {
        showCenteredText("Control points mission", 50, 1);
    }
    
    update();
}

void DisplayManager::showCountdown(int timeRemaining) {
    if (!initialized) return;
    
    clear();
    
    // Format time as MM:SS
    int minutes = timeRemaining / 60;
    int seconds = timeRemaining % 60;
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    
    showCenteredText("COUNTDOWN", 2, 1);
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    // Center the time string with large text
    showCenteredText(timeStr, 25, 3);
    
    update();
}

void DisplayManager::showDefuseScreen(int timeRemaining, bool armed, const String& code) {
    if (!initialized) return;
    
    clear();
    
    // Format time as MM:SS
    int minutes = timeRemaining / 60;
    int seconds = timeRemaining % 60;
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    
    // Show status at top
    if (armed) {
        display.fillRect(0, 0, SCREEN_WIDTH, 16, SH110X_WHITE);
        display.setTextColor(SH110X_BLACK);
        showCenteredText("ARMED", 4, 2);
        display.setTextColor(SH110X_WHITE);
    } else {
        showCenteredText("DISARMED", 4, 2);
    }
    
    // Show time with large font
    showCenteredText(timeStr, 26, 3);
    
    // Show code if available
    if (code.length() > 0) {
        showCenteredText("CODE: " + code, 52, 1);
    }
    
    update();
}

void DisplayManager::showDominationScreen(int redScore, int blueScore, int threshold) {
    if (!initialized) return;
    
    clear();
    
    showCenteredText("DOMINATION", 2, 1);
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    // Determine leading team
    String status;
    if (redScore > blueScore) {
        status = "RED LEAD";
    } else if (blueScore > redScore) {
        status = "BLUE LEAD";
    } else {
        status = "TIED";
    }
    
    showCenteredText(status, 18, 2);
    
    // Show scores
    char scoreText[20];
    sprintf(scoreText, "R:%d  B:%d", redScore, blueScore);
    showCenteredText(scoreText, 38, 1);
    
    // Draw progress bars
    int redWidth = map(redScore, 0, threshold, 0, (SCREEN_WIDTH / 2) - 5);
    int blueWidth = map(blueScore, 0, threshold, 0, (SCREEN_WIDTH / 2) - 5);
    
    // Draw red team progress bar on the left
    display.drawRect(5, 48, (SCREEN_WIDTH / 2) - 10, 8, SH110X_WHITE);
    if(redWidth > 0)
        display.fillRect(5, 48, redWidth, 8, SH110X_WHITE);
    
    // Draw blue team progress bar on the right
    display.drawRect(SCREEN_WIDTH / 2 + 5, 48, (SCREEN_WIDTH / 2) - 10, 8, SH110X_WHITE);
    if (blueWidth > 0)
        display.fillRect(SCREEN_WIDTH / 2 + 5, 48, blueWidth, 8, SH110X_WHITE);
    
    update();
}

void DisplayManager::showGameOver(bool victory) {
    if (!initialized) return;
    
    clear();
    
    showCenteredText("MISSION", 15, 2);
    
    if (victory) {
        showCenteredText("SUCCESS", 40, 2);
    } else {
        showCenteredText("FAILED", 40, 2);
    }
    
    update();
}

void DisplayManager::showSettings(const String& setting, const String& value) {
    if (!initialized) return;
    
    clear();
    
    showCenteredText("SETTINGS", 2, 1);
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    showCenteredText(setting, 24, 1);
    showCenteredText(value, 42, 2);
    
    update();
}

void DisplayManager::showPassword(const String& password, bool hidden) {
    if (!initialized) return;
    
    clear();
    
    showCenteredText("ENTER CODE", 2, 1);
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    String displayText;
    if (hidden) {
        for (unsigned int i = 0; i < password.length(); i++) {
            displayText += "*";
        }
    } else {
        displayText = password;
    }
    
    showCenteredText(displayText, 32, 2);
    
    update();
}

void DisplayManager::showBatteryStatus(float voltage) {
    if (!initialized) return;
    
    clear();
    
    showCenteredText("BATTERY", 2, 1);
    display.drawLine(0, 12, SCREEN_WIDTH, 12, SH110X_WHITE);
    
    char voltStr[10];
    sprintf(voltStr, "%.2fV", (double)voltage);
    
    showCenteredText(voltStr, 28, 2);
    
    // Draw a larger battery icon
    int batteryLevel = map(constrain((int)(voltage * 100), 320, 420), 320, 420, 0, 5);
    
    int battWidth = 50;
    int battHeight = 16;
    int battX = (SCREEN_WIDTH - battWidth) / 2;
    int battY = 44;
    
    display.drawRect(battX, battY, battWidth, battHeight, SH110X_WHITE);
    display.fillRect(battX + battWidth, battY + 4, 3, battHeight - 8, SH110X_WHITE);
    
    if (batteryLevel > 0) {
        int fillWidth = map(batteryLevel, 0, 5, 0, battWidth - 4);
        display.fillRect(battX + 2, battY + 2, fillWidth, battHeight - 4, SH110X_WHITE);
    }
    
    update();
}

void DisplayManager::showError(const String& message) {
    if (!initialized) return;
    
    clear();
    
    showCenteredText("ERROR", 10, 2);
    showCenteredText(message, 36, 1);
    
    update();
}

void DisplayManager::showDominationSetup(int minutes) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DOMINATION SETUP");
  display.println("");
  
  display.setTextSize(2);
  display.println("Time:");
  
  display.setTextSize(3);
  char timeStr[10];
  sprintf(timeStr, "%d min", minutes);
  display.println(timeStr);
  
  display.setTextSize(1);
  display.println("");
  display.println("Green: +5 min  Red: -5 min");
  display.println("# to start");
  
  display.display();
}

void DisplayManager::showDominationScreen(int redScore, int greenScore, int captureProgress, 
                                        PointOwnership currentOwner, int remainingTime) {
  display.clearDisplay();
  
  // Show timer at top
  int minutes = remainingTime / 60;
  int seconds = remainingTime % 60;
  char timeStr[10];
  sprintf(timeStr, "%02d:%02d", minutes, seconds);
  
  display.setTextSize(2);
  display.setCursor(30, 0);
  display.println(timeStr);
  
  // Show team scores
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("RED: ");
  display.print(redScore);
  
  display.setCursor(70, 20);
  display.print("GREEN: ");
  display.print(greenScore);
  
  // Show flag ownership and capture progress
  display.setCursor(0, 32);
  switch (currentOwner) {
    case RED_TEAM:
      display.println("Flag: RED TEAM");
      break;
    case GREEN_TEAM:
      display.println("Flag: GREEN TEAM");
      break;
    default:
      display.println("Flag: NEUTRAL");
      break;
  }
  
  // Draw capture progress bar
  display.drawRect(0, 44, 128, 10, SH110X_WHITE); // Changed WHITE to SH110X_WHITE
  
  // Fill progress bar based on team color and progress
  if (captureProgress > 0) {
    // Calculate width based on progress (0-100%)
    int width = (captureProgress * 126) / 100;
    display.fillRect(1, 45, width, 8, SH110X_WHITE); // Changed WHITE to SH110X_WHITE
  }
  
  display.display();
}

void DisplayManager::showDominationGameOver(PointOwnership winner, int redScore, int greenScore) {
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("GAME OVER");
  
  display.setTextSize(1);
  display.println("");
  
  // Show scores
  display.print("RED: ");
  display.println(redScore);
  
  display.print("GREEN: ");
  display.println(greenScore);
  
  display.println("");
  
  // Show winner
  display.setTextSize(2);
  switch (winner) {
    case RED_TEAM:
      display.println("RED WINS!");
      break;
    case GREEN_TEAM:
      display.println("GREEN WINS!");
      break;
    default:
      display.println("DRAW!");
      break;
  }
  
  display.setTextSize(1);
  display.println("");
  display.println("Press # to restart");
  
  display.display();
}