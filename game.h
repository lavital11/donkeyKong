#pragma once
#include "Steps.h"
#include "Results.h"
#include "Mario.h"
#include "Barrels.h"
#include <vector>
#include "Ghost.h"
#include "Hammer.h"
#include <string>
#include "Board.h"
#include <chrono>
#include <thread>
#include <conio.h>
#include <time.h>
#include <filesystem>

// Constants for key codes, scores, and menu options
static constexpr int MAX_X = 80; // Maximum width of the board
static constexpr int MAX_Y = 25; // Maximum height of the board
static constexpr int SCORE_X = 9;
static constexpr int SCORE_Y = 2;
constexpr int ESC = 27; // ESC key code
static constexpr int KILL_BARREL_SCORE = 50;
static constexpr int KILL_GHOST_SCORE = 30;
static constexpr int LESS_THAN_A_MINUITE_SCORE = 100;
static constexpr int FINISH_LEVEL = 300;
static constexpr int DIE = -50;
static constexpr int WITH_COLORS = 1;
static constexpr int NO_COLORS = 2;
static constexpr int SPEC_BOARD = 3;
static constexpr int INSTRUCTIONS = 8;
static constexpr int EXIT = 9;

// Class representing the game state and logic
class Game {
private:
    // Internal variables to manage game state
    std::vector<Barrels> barrels; // List of barrels in the game
    std::vector<Ghost> ghosts;  // List of ghosts in the game
    Hammer hammer;
    int hammerOriginalX;
    int hammerOriginalY;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdateTime;
    int countMario = 0;
    int countLegend = 0;
    int countPau = 0;
    int countDonkey = 0;
    int DONK_X; // X position of Donkey
    int DONK_Y; // Y position of Donkey

protected:
    int nextDeadIteration;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool isGameOver;            // Whether the game is over
    bool isPaused;              // Whether the game is paused
    std::vector<std::string> boardFiles; // List of board file paths
    int currentBoardIndex;               // Index of the currently loaded board
    Board board;                 // לוח המשחק
    Mario player;               // השחקן (מריו)
    size_t gameTime;             // מונה זמן המשחק
    std::string stepsFilename;   // שם קובץ הצעדים
    std::string resultsFilename; // שם קובץ התוצאות
    Steps steps;                 // אובייקט ניהול צעדים
    Results results;
    long random_seed;
    bool noColors;              // Whether to display colors in the game
    int PAU_X;  // X position of Pau
    int PAU_Y;  // Y position of Pau
    
    bool isAuto;
    bool isSave;

public:
    // Constructor
    Game();

    virtual Game& operator=(const Game& other);
    

    // Functions for managing the game
    virtual void startGame() = 0 ;                  // Displays the opening menu
    void runGame();                    // Runs the main game loop
    void pauseGame();                  // Pauses the game
    void resumeGame();                 // Resumes the game from pause
    void endGame();                    // Ends the game

    // Auxiliary functions
    void checkCollision();             // Checks if Mario collides with barrels
    void handleCollision();            // Handles collisions
    void checkLevelPass();
    void nextLevel();
    void winGame();                    // Handles win logic
    void printWinMessage() const;      // Displays the win message
    void printNextLevelMessage() const;
    void printLoseMessage();           // Displays the lose message
    void restartLevel();               // Restarts the level after loss
    void createLegend();
    void createDonkey();
    void createPau();
    void printLegend(int x, int y);
    void ignoreOldGhost();

    // Barrel management functions
    virtual void updateBarrels();              // Updates the state of the barrels in the game loop
    virtual void updateGhosts();
    void spawnBarrel();                // Spawns a new barrel in the game
    void resetBarrels();               // Resets the state of the barrels
    void resetGhosts();

    // Helper functions
    bool shouldSpawnBarrel();          // Checks if a new barrel should be spawned
    Point getMarioPosition() const;    // Returns the current position of Mario

    void createGhost();
    void createHammer();
    void collectHammer();
    void resetHammer();
    void useHammer();
    void checkAndRemoveEntities(int checkX, int checkY, bool& success);
    void createMario();

    // New function to load board files
    void loadBoardFiles(const std::string& directory);

    // New function to load a specific board by index
    void loadBoardByIndex(int index);

    bool validateBoard();
    void drawBorders();
    bool checkMario();
    bool checkPau();
    bool checkDonkey();
    bool checkLegend();
    bool checkGhost();
    bool checkInvalidChar();
    bool checkLadder();
    bool canMarioClimb() const;

    void startLevel();
    void endLevel();
    double getElapsedTime() const;
    
    virtual void inputAction() = 0;
    bool getIsAuto() {
        return isAuto;
    }
    void setIsAuto(bool is) {
        isAuto = is;
    }
    void setIsSave(bool is) {
        isSave = is;
    }
    bool getIsSave() {
        return isSave;
    }
    virtual void saveStepAfterFinish() = 0;
    virtual void saveResultsAfterFinish() = 0;
    void reportResultError(const std::string& message, const std::string& filename, size_t iteration);

};