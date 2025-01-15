#pragma once
#include "Mario.h"
#include "Barrels.h"
#include <vector>
#include "Ghost.h"
#include "Hammer.h"
#include <string>
#include "Board.h"
#include <chrono>
#include <thread>

static constexpr int MAX_X = 80; // Maximum width of the board
static constexpr int MAX_Y = 25; // Maximum height of the board

// Class representing the game state and logic
class Game {
private:
    // Internal variables to manage game state
    bool isGameOver;            // Whether the game is over
    bool isPaused;              // Whether the game is paused
    bool noColors;              // Whether to display colors in the game
    Board board;                // The game board
    Mario player;               // The main player (Mario)
    std::vector<Barrels> barrels; // List of barrels in the game
    std::vector<Ghost> ghosts; // List of ghosts in the game
    std::vector<std::string> boardFiles; // List of board file paths
    int currentBoardIndex;               // Index of the currently loaded board
    Hammer hammer;
    int hammerOriginalX;
    int hammerOriginalY;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    double gameTime = 0;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdateTime;
    int countMario = 0;
    int countLegend = 0;
    int countPau = 0;
    int countDonkey = 0;
    int DONK_X; // X position of Donkey
    int DONK_Y; // Y position of Donkey

public:
    // Constructor
    Game();

    // Functions for managing the game
    void startGame();           // Displays the opening menu
    void runGame();             // Runs the main game loop
    void pauseGame();           // Pauses the game
    void resumeGame();          // Resumes the game from pause
    void endGame();             // Ends the game

    // Auxiliary functions
    void showInstructions();    // Displays game instructions
    void checkCollision();      // Checks if Mario collides with barrels
    void handleCollision();     // Handles collisions
    void checkLevelPass();
    void nextLevel();
    void winGame();             // Handles win logic
    void printWinMessage();     // Displays the win message
    void printNextLevelMessage() const;
    void printLoseMessage();    // Displays the lose message
    void restartLevel();        // Restarts the level after loss
    void createLegend();
    void createDonkey();
    void printLegend(int x, int y);
    void ignoreOldGhost();

    // Barrel management functions
    void updateBarrels();       // Updates the state of the barrels in the game loop
    void updateGhosts();
    void spawnBarrel();         // Spawns a new barrel in the game
    void resetBarrels();        // Resets the state of the barrels
    void resetGhosts();

    // Helper functions
    bool shouldSpawnBarrel();   // Checks if a new barrel should be spawned
    Point getMarioPosition() const; // Returns the current position of Mario

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

    void startLevel();
    void endLevel();
    double getElapsedTime() const;
};
