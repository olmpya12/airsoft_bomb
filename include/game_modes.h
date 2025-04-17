#ifndef GAME_MODES_H
#define GAME_MODES_H
#include "config.h"
#include <Arduino.h> // Add this to get millis()
#include <display_manager.h>
#include <sound_manager.h>

class DisplayManager;
class SoundManager;


class GameBase {
public:
  GameBase();
  virtual ~GameBase() = default;
  virtual void init() = 0;
  virtual void update() = 0; // Keep return type as void
  virtual void handleInput(int button) = 0;
  virtual bool isGameOver() = 0;
  virtual void reset() = 0;
  
  // Add handleButton method to base class
  virtual void handleButton(char button) = 0;
  virtual void setManagers(DisplayManager* d, SoundManager* s) {}
};

class DefuseMode : public GameBase {
private:
  DefuseState state;
  unsigned long startTime;
  int timeLimit;  // Time limit in seconds
  bool armed;
  int armingCode[4] = {1, 2, 3, 4};
  int defuseCode[4] = {5, 6, 7, 8};
  int inputCode[4];
  int codePosition;
  DisplayManager* display;
  SoundManager* sound;
  unsigned long lastBeepTime = 0;

public:
  DefuseMode();
  ~DefuseMode() override = default; 
  void init() override;
  void update() override;
  void handleInput(int button) override;
  bool isGameOver() override;
  void reset() override;
  void setTimeLimit(int seconds);
  void handleButton(char button) override;
  void setManagers(DisplayManager* d, SoundManager* s);
};


class DominationMode : public GameBase {
private:
  unsigned long gameTime;       // Total game time in seconds
  unsigned long startTime;      // When the game started
  unsigned long elapsedTime;    // How much time has passed
  
  PointOwnership currentOwner;  // Who currently owns the point
  unsigned long captureStartTime; // When capturing started
  int captureProgress;          // 0-100 percent progress
  
  PointOwnership capturingTeam; // Team currently capturing

  bool redButtonHeld;           // Red team button state
  bool greenButtonHeld;         // Green team button state

  int redScore;                 // Red team score (seconds owned)
  int greenScore;               // Green team score (seconds owned)
  unsigned long lastScoreUpdate; // Last time we updated scores
  
  bool setupComplete;           // Indicates if setup is complete

  DisplayManager* display;
  SoundManager* sound;
public:
  GameState state;              // Current game state
  
  DominationMode();
  ~DominationMode() override = default;
  void init() override;
  void update() override;
  void handleInput(int button) override;
  void handleButton(char button) override;
  bool isGameOver() override;
  void reset() override;
  
  // Domination specific methods
  void setupGameTime(bool increase);
  void startCapture(PointOwnership team);
  void updateCapture();
  void updateScores();
  void setWinThreshold(int seconds);
  
  // Getter methods
  unsigned long getGameTime() const { return gameTime; }
  unsigned long getElapsedTime() const { return elapsedTime; }
  int getRedScore() const { return redScore; }
  int getGreenScore() const { return greenScore; }
  int getCaptureProgress() const { return captureProgress; }
  PointOwnership getCurrentOwner() const { return currentOwner; }

  // Add to game_modes.h in the DominationMode class public section:
  void updateButtonStates(bool redPressed, bool greenPressed);

  int getCaptureProgress();
  PointOwnership getCurrentOwner();
  int getRedScore();
  int getGreenScore();
  int getGameTime();
  int getElapsedTime();
  PointOwnership getCapturingTeam();
  void setManagers(DisplayManager* d, SoundManager* s);
};

#endif // GAME_MODES_H