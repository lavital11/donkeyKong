﻿#include <windows.h>
#include <conio.h>
#include <time.h>
#include <filesystem>
#include "Game.h"

// Constants for key codes, scores, and menu options
constexpr int ESC = 27; // ESC key code
static constexpr int SCORE_X = 9;
static constexpr int SCORE_Y = 2;
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

// Using namespace for filesystem and time utilities
namespace fs = std::filesystem;
using namespace std::chrono;

// Constructor: Initializes the game state and player
Game::Game() : isGameOver(false), isPaused(false), noColors(false), currentBoardIndex(0), hammer(nullptr, -1, -1, nullptr), player(this,-1,-1,nullptr), hammerOriginalX(0), hammerOriginalY(0), DONK_X(0), DONK_Y(0), PAU_X(0), PAU_Y(0) {}

// Main game loop: Displays the menu and starts the game based on user choice
void Game::startGame() {
    loadBoardFiles("boards/"); // Load all board files from the "boards" directory
    int choice = 0; // Variable for storing user menu choice
    do {
        system("cls"); // Clear the console screen
        std::cout << "(1) Start a new game with colors" << std::endl;
        std::cout << "(2) Start a new game without colors" << std::endl;
        std::cout << "(3) Select a specific board" << std::endl;
        std::cout << "(8) Present instructions and keys" << std::endl;
        std::cout << "(9) EXIT" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case WITH_COLORS: // Start a game with colors enabled
            noColors = false;
            system("cls");
            runGame();
            break;

        case NO_COLORS: // Start a game without colors
            noColors = true;
            system("cls");
            runGame();
            break;

        case SPEC_BOARD: { // Allow the user to select a specific board
            system("cls");
            std::cout << "Available boards:\n";
            for (size_t i = 0; i < boardFiles.size(); ++i) {
                std::cout << "(" << i + 1 << ") " << boardFiles[i] << "\n";
            }
            std::cout << "Enter the board number to load: ";
            int boardChoice;
            std::cin >> boardChoice;
            if (boardChoice >= 1 && boardChoice <= static_cast<int>(boardFiles.size())) {
                loadBoardByIndex(boardChoice - 1); // Load the selected board
                Sleep(3000); // Pause for 3 seconds to show the loaded board
            }
            else {
                std::cout << "Invalid board number. Returning to main menu...\n";
                Sleep(1500); // Pause for 1.5 seconds before returning to the menu
            }
            break;
        }

        case INSTRUCTIONS: // Display game instructions
            system("cls");
            showInstructions();
            break;

        case EXIT: // Exit the game
            std::cout << "Exiting the game. Goodbye!" << std::endl;
            break;

        default: // Handle invalid menu choices
            std::cout << "Invalid choice. Please try again." << std::endl;
            std::cin.clear();             // Clear the error flag on cin
            std::cin.ignore(256, '\n');   // Ignore the invalid input
            Sleep(1500); // Pause for 1.5 seconds for user feedback
            break;
        }
    } while (choice != EXIT); // Exit the loop when the user chooses to quit
}

// Displays instructions for the game
void Game::showInstructions() {
    system("cls");
    std::cout << "Welcome to the game!" << std::endl;
    std::cout << "Instructions:" << std::endl;
    std::cout << "Use the following keys to control Mario:" << std::endl;
    std::cout << "'W' - Jump or climb up the ladder" << std::endl;
    std::cout << "'A' - Move left" << std::endl;
    std::cout << "'D' - Move right" << std::endl;
    std::cout << "'X' - Climb down the ladder" << std::endl;
    std::cout << "'ESC' - Pause the game" << std::endl;
    std::cout << "Press any key to return to the main menu..." << std::endl;
    std::cin.ignore(); // Handle leftover input
    std::cin.get(); // Wait for the user to press a key
}

// Runs the main game logic after starting or resuming
void Game::runGame() {
    isGameOver = false;
    isPaused = false;
    player.life = 3; // Reset player's lives
    player.score = 0; // Reset player's score
    ShowConsoleCursor(false); // Hide the cursor for a cleaner game interface
    board.reset(boardFiles[currentBoardIndex]); // Load the current board
    if (!validateBoard()) { // Validate the board for proper setup
        Sleep(3000); // Pause for 3 seconds to let the user read the error
        return; // Return to the main menu if the board is invalid
    }

    // Initialize the game elements
    board.print(noColors);
    player.setBoard(board);
    createMario();
    createHammer();
    createGhost();
    createLegend();
    createPau();
    startLevel();

    // Timers for controlling game updates
    auto lastBarrelSpawnTime = steady_clock::now();
    auto lastBarrelUpdateTime = steady_clock::now();
    auto lastMarioUpdateTime = steady_clock::now();

    constexpr int barrelSpawnInterval = 1500; // Milliseconds between barrel spawns
    constexpr int barrelUpdateInterval = 100; // Milliseconds between barrel updates
    constexpr int marioUpdateInterval = 80;   // Milliseconds between Mario updates

    // Game loop
    while (!isGameOver) {
        auto currentTime = steady_clock::now(); // Get the current time

        // Check for keyboard input
        if (_kbhit()) {
            char key = _getch();
            if (key == ESC) pauseGame(); // Pause game if ESC is pressed
            else if (key == 'p' || key == 'P') useHammer(); // Use hammer if 'P' is pressed
            else player.keyPressed(key); // Handle other key inputs
        }

        // Update barrels at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastBarrelUpdateTime).count() >= barrelUpdateInterval) {
            updateBarrels();
            lastBarrelUpdateTime = currentTime;
        }

        // Spawn barrels at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastBarrelSpawnTime).count() >= barrelSpawnInterval) {
            spawnBarrel();
            lastBarrelSpawnTime = currentTime;
        }

        // Update Mario at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastMarioUpdateTime).count() >= marioUpdateInterval) {
            player.erase(noColors); // Erase Mario's current position
            player.move(noColors); // Update Mario's position
            player.draw(noColors); // Draw Mario in the new position
            lastMarioUpdateTime = currentTime;
        }

        player.printScore(SCORE_X, SCORE_Y); // Update score display
        updateGhosts(); // Update ghost positions
        collectHammer(); // Check if Mario collects the hammer
        checkCollision(); // Handle collisions with barrels or ghosts
        checkLevelPass(); // Check if Mario passes the level
        Sleep(10); // Add a short delay to reduce CPU usage
    }
}

// Pauses the game and displays a message
void Game::pauseGame() {
    isPaused = true; // Set the paused state
    system("cls"); // Clear the screen
    std::cout << "Game paused. Press ESC again to continue..." << std::endl;

    while (true) {
        if (_kbhit()) { // Wait for a key press
            char key = _getch();
            if (key == ESC) { // Resume the game if ESC is pressed again
                resumeGame();
                break;
            }
        }
        Sleep(100); // Reduce CPU usage while waiting
    }
}

// Resumes the game from the paused state
void Game::resumeGame() {
    isPaused = false; // Reset the paused state
    system("cls"); // Clear the screen
    ignoreOldGhost(); // Clear old ghost positions
    board.print(noColors); // Reprint the board
    createLegend(); // Redraw the legend
}

// Ends the game and displays the "Game Over" message
void Game::endGame() {
    isGameOver = true; // Set the game-over state
    restartLevel(); // Reset the level for a fresh start
    system("cls"); // Clear the screen

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); // Set text color to red
    }

    printLoseMessage(); // Show the "Game Over" message

    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset text color
    }

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
    while (!_kbhit()) { // Wait for any key press
        Sleep(100); // Reduce CPU usage while waiting
    }
    char key = _getch(); // Capture the key press to continue
}

// Returns Mario's current position as a Point object
Point Game::getMarioPosition() const {
    return player.getPosition(); // Retrieve the player's position
}

// Updates all barrels' positions and handles their removal
void Game::updateBarrels() {
    auto currentTime = steady_clock::now();
    auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();

    if (elapsedTime >= 100) { // Update barrels every 100 milliseconds
        // Iterate through the barrels list
        for (auto it = barrels.begin(); it != barrels.end(); ) {
            if (it->shouldRemove()) { // Check if the barrel should be removed
                it->erase(noColors); // Erase the barrel from the screen
                it = barrels.erase(it); // Remove the barrel from the list
            }
            else {
                ++it; // Move to the next barrel
            }
        }

        for (auto& barrel : barrels) {
            barrel.move(noColors); // Update the barrel's position
        }

        lastUpdateTime = currentTime; // Reset the timer for barrel updates
    }
}

// Updates all ghosts' positions and handles their removal
void Game::updateGhosts() {
    static auto lastUpdateTime = steady_clock::now();
    auto currentTime = steady_clock::now();
    auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();

    if (elapsedTime >= 100) { // Update ghosts every 100 milliseconds
        // Iterate through the ghosts list
        for (auto it = ghosts.begin(); it != ghosts.end(); ) {
            if (it->shouldRemove()) { // Check if the ghost should be removed
                it->erase(noColors); // Erase the ghost from the screen
                it = ghosts.erase(it); // Remove the ghost from the list
            }
            else {
                ++it; // Move to the next ghost
            }
        }

        for (auto& ghost : ghosts) {
            ghost.move(noColors, ghosts); // Update the ghost's position
        }
        lastUpdateTime = currentTime; // Reset the timer for ghost updates
    }
}

// Checks collisions between Mario, barrels, and ghosts
void Game::checkCollision() {
    for (auto& barrel : barrels) {
        if (player.isCollidingBarrel(barrel)) { // Check if Mario collides with a barrel
            handleCollision(); // Handle collision logic
            break;
        }
        if (barrel.isExplodeAndKillMario()) { // Check if a barrel explodes near Mario
            handleCollision(); // Handle explosion logic
            break;
        }
    }

    for (auto& ghost : ghosts) {
        if (player.isCollidingGhost(ghost)) { // Check if Mario collides with a ghost
            handleCollision(); // Handle collision logic
            break;
        }
    }
}

// Handles the logic for when Mario collides with an obstacle
void Game::handleCollision() {
    player.erase(noColors); // Erase Mario from his current position
    player.loseLife(); // Decrease Mario's life count
    resetBarrels(); // Clear all barrels from the game
    resetGhosts(); // Clear all ghosts
    player.score += DIE; // Deduct points for losing a life

    if (player.life <= 0) { // If Mario has no lives left
        endGame(); // End the game
    }
    else {
        restartLevel(); // Restart the current level
    }
}

// Determines if a barrel should be spawned based on random chance
bool Game::shouldSpawnBarrel() {
    constexpr int spawnChance = 5; // 5% chance to spawn a barrel
    return (rand() % 100) < spawnChance;
}

// Spawns a new barrel at Donkey Kong's position
void Game::spawnBarrel() {
    createDonkey(); // Ensure Donkey Kong's position is set
    Barrels newBarrel(&board, DONK_X, DONK_Y, this); // Create a new barrel
    newBarrel.setInitialDirection(); // Set the barrel's initial direction
    barrels.push_back(newBarrel); // Add the barrel to the list
}

// Checks if Mario has reached the level's finishing point
void Game::checkLevelPass() {
    if (player.getX() == PAU_X && player.getY() == PAU_Y) { // Check if Mario reached the finish
        endLevel(); // Stop the level timer
        gameTime = getElapsedTime(); // Calculate elapsed time for the level

        if (gameTime < 60) { // Award bonus points for completing the level quickly
            player.score += LESS_THAN_A_MINUITE_SCORE;
        }

        player.score += FINISH_LEVEL; // Award points for finishing the level

        if (currentBoardIndex + 1 < static_cast<int>(boardFiles.size())) {
            nextLevel(); // Move to the next level if available
        }
        else {
            winGame(); // End the game with a victory if all levels are completed
        }
    }
}

// Moves to the next level in the game
void Game::nextLevel() {
    resetBarrels(); // Clear all barrels from the game
    resetGhosts();  // Clear all ghosts from the game
    system("cls");  // Clear the screen

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Set blue color for the message
    }
    printNextLevelMessage(); // Display the "Next Level" message

    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset colors
    }

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
    while (!_kbhit()) { // Wait for user input
        Sleep(100);
    }
    char key = _getch(); // Capture key press to proceed

    system("cls"); // Clear the screen after the message

    ++currentBoardIndex; // Increment the board index to move to the next level
    board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Load the next board
    restartLevel(); // Reset the level for the new board
    board.print(noColors); // Display the new board
    createLegend(); // Display the legend
    player.printLife(LIFE_X, LIFE_Y); // Show Mario's remaining lives
    player.printScore(SCORE_X, SCORE_Y); // Show Mario's score
}

// Ends the game with a win message and resets the game state
void Game::winGame() {
    isGameOver = true; // Mark the game as over
    currentBoardIndex = 0; // Reset to the first board
    restartLevel(); // Reset the level state
    system("cls"); // Clear the screen

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Set text color to green
    }

    printWinMessage(); // Display the win message

    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset text color
    }

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
    while (!_kbhit()) { // Wait for user input
        Sleep(100);
    }
    char key = _getch(); // Capture key press to continue
}

// Displays the win message with formatted output
void Game::printWinMessage() const {
    int screenWidth = 80; // Console screen width
    int screenHeight = 25; // Console screen height

    std::string line1 = "*****************************************";
    std::string line2 = "*                                       *";
    std::string line3;
    std::string line4 = "*     YOU PASSED THE FINAL LEVEL!       *";
    std::string line5 = "*          EARNED 300 POINTS!           *";
    std::string line6 = "*               YOU WIN!                *";
    std::string line7 = "*            Game Completed!            *";
    std::string line8 = "            Total Score:";

    // Check if the player gets a bonus for finishing quickly
    if (gameTime < 60) {
        line3 = "*   100 POINTS BONUS FOR BEING FAST!    *";
    }
    else {
        line3 = "*                                       *";
    }

    const int messageHeight = 8;
    const int startRow = (screenHeight - messageHeight) / 2; // Center vertically
    const int centerColumn = (screenWidth - static_cast<int>(line1.length())) / 2; // Center horizontally
    for (int i = 0; i < startRow; ++i) {
        std::cout << std::endl; // Add padding above the message
    }

    // Print the formatted message box
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line3 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line4 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line5 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line6 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line7 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line8 << " " << player.score << std::endl;
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl;

    std::cout << std::endl; // Add padding below the message
    std::cout << std::string(centerColumn, ' ') << "Press any key to return to the main menu..." << std::endl;
}

// Displays the next level message with formatted output
void Game::printNextLevelMessage() const {
    const int screenWidth = 80;
    const int screenHeight = 25;

    std::string line1 = "*****************************************";
    std::string line2 = "*                                       *";
    std::string line3;
    std::string line4 = "*        YOU PASSED THIS LEVEL!         *";
    std::string line5 = "*          EARNED 300 POINTS!           *";
    std::string line6 = "*     GOOD LUCK IN THE NEXT LEVEL!      *";

    // Check if the player gets a bonus for finishing quickly
    if (gameTime < 60) {
        line3 = "*   100 POINTS BONUS FOR BEING FAST!    *";
    }
    else {
        line3 = "*                                       *";
    }

    const int messageHeight = 6;
    const size_t startRow = (screenHeight - messageHeight) / 2;
    const size_t centerColumn = (screenWidth - line1.length()) / 2;

    for (int i = 0; i < startRow; ++i) {
        std::cout << std::endl; // Add padding above the message
    }

    // Print the formatted message box
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line3 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line4 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line5 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line6 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl;

    std::cout << std::endl;
    std::cout << std::string(centerColumn, ' ') << "Press any key to start the next level..." << std::endl;
}

// Displays the game over message with formatted output
void Game::printLoseMessage() {
    const int screenWidth = 80;
    const int screenHeight = 25;

    std::string line1 = "*****************************************";
    std::string line2 = "*                                       *";
    std::string line3 = "*              GAME OVER!               *";
    std::string line4 = "*     Mario has lost all his lives.     *";

    const int messageHeight = 6;
    const int startRow = (screenHeight - messageHeight) / 2;
    const int centerColumn = (screenWidth - static_cast<int>(line1.length())) / 2; // Center horizontally
    for (int i = 0; i < startRow; ++i) {
        std::cout << std::endl; // Add padding above the message
    }

    // Print the formatted message box
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line3 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line4 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl;
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl;

    std::cout << std::endl;
    std::cout << std::string(centerColumn, ' ') << "Press any key to return to the main menu..." << std::endl;
}

// Resets all barrels in the game
void Game::resetBarrels() {
    for (auto& barrel : barrels) {
        barrel.erase(noColors); // Erase each barrel from the screen
    }
    barrels.clear(); // Clear the barrel list

    gotoxy(DONK_X, DONK_Y); // Move to Donkey Kong's position
    std::cout << ' '; // Clear Donkey Kong's spot
}

// Resets all ghosts in the game
void Game::resetGhosts() {
    for (auto& ghost : ghosts) {
        ghost.erase(noColors); // Erase each ghost from the screen
    }
    ghosts.clear(); // Clear the ghost list
}

// Restarts the current level, resetting all game elements
void Game::restartLevel() {
    countMario = 0; // Reset Mario counter
    countLegend = 0; // Reset legend counter
    resetBarrels(); // Clear all barrels
    resetGhosts(); // Clear all ghosts
    resetHammer(); // Reset hammer state

    board.reset(boardFiles[currentBoardIndex]); // Reload the board
    board.print(noColors); // Print the reset board
    createGhost(); // Recreate ghosts on the board
    createHammer(); // Recreate hammer on the board
    createMario(); // Recreate Mario on the board
    player.drawStart(noColors); // Draw Mario at the starting position
    createLegend(); // Recreate the legend
}

// Creates all ghosts on the board by scanning for 'x' characters
void Game::createGhost() {
    ghosts.clear(); // Clear existing ghosts

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'x') { // Check for ghost markers
                Ghost newGhost(&board, x, y, this); // Create a new ghost
                newGhost.setInitialDirection(); // Set its initial direction
                ghosts.push_back(newGhost); // Add it to the ghost list
            }
        }
    }
}

// Removes old ghost markers ('x') from the board
void Game::ignoreOldGhost() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'x') { // Check for ghost markers
                board.setChar(x, y, ' '); // Replace them with spaces
            }
        }
    }
}

// Creates the hammer by finding the 'p' character on the board
void Game::createHammer() {
    hammer = Hammer(&board, -1, -1, this); // Reset hammer position

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'p') { // Find the hammer marker
                hammer = Hammer(&board, x, y, this); // Set hammer position
                board.setChar(x, y, 'p'); // Ensure marker remains on the board
                return;
            }
        }
    }
}

// Creates the legend by finding the 'L' character on the board
void Game::createLegend() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'L') { // Find the legend marker
                printLegend(x, y); // Print the legend details
                return;
            }
        }
    }
}

// Creates Mario by finding the '@' character on the board
void Game::createMario() {
    int currentLife = player.life; // Save Mario's current life count
    int currentScore = player.score; // Save Mario's current score

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '@') { // Find Mario's marker
                player = Mario(this, x, y, &board); // Set Mario's position
                player.life = currentLife; // Restore life count
                player.score = currentScore; // Restore score
                player.drawStart(noColors); // Draw Mario on the board
                return;
            }
        }
    }
}

// Creates Donkey Kong by finding the '&' character on the board
void Game::createDonkey() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '&') { // Find Donkey Kong's marker
                DONK_X = x; // Save X-coordinate
                DONK_Y = y; // Save Y-coordinate
                return;
            }
        }
    }
}

// Creates Pauline by finding the '$' character on the board
void Game::createPau() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '$') { // Find Pauline's marker
                PAU_X = x; // Save X-coordinate
                PAU_Y = y; // Save Y-coordinate
                return;
            }
        }
    }
}

// Displays the legend on the board at the specified coordinates
void Game::printLegend(int x, int y) {
    gotoxy(x, y);
    std::cout << "MARIO'S LIFE:"; // Display Mario's life
    player.printLife(LIFE_X, LIFE_Y); // Print life details
    gotoxy(x, y + 1);
    std::cout << "SCORE:"; // Display score
    player.printScore(SCORE_X, SCORE_Y); // Print score details
}

// Handles collecting the hammer when Mario reaches its position
void Game::collectHammer() {
    if (player.getX() == hammer.getX() && player.getY() == hammer.getY()) { // Check if Mario collects the hammer
        player.hasHammer = true; // Update Mario's hammer state
        hammerOriginalX = hammer.getX(); // Save hammer's original X position
        hammerOriginalY = hammer.getY(); // Save hammer's original Y position
        hammer.setPosition(-1, -1); // Remove hammer from the board
        board.setChar(player.getX(), player.getY(), ' '); // Clear hammer marker
        hammer.printP(); // Indicate hammer collection
    }
}

// Resets the hammer to its original position
void Game::resetHammer() {
    if (hammerOriginalX != -1 && hammerOriginalY != -1) { // Check if hammer has an original position
        board.setChar(hammerOriginalX, hammerOriginalY, 'p'); // Restore hammer marker
        hammer.setPosition(hammerOriginalX, hammerOriginalY); // Update hammer position
    }
}

// Handles the use of the hammer by Mario
void Game::useHammer() {
    if (!player.hasHammer) return; // Do nothing if Mario doesn't have the hammer

    bool success = false;
    int marioX = player.getX();
    int marioY = player.getY();

    // Use Mario's current movement direction
    Point::Direction direction = player.getDirection();

    // Check the hammer's effect based on Mario's direction
    if (direction.x == 1 && direction.y == 0) { // Mario moving right
        for (int dx = 1; dx <= 3; ++dx) { // Check up to 3 spaces ahead
            checkAndRemoveEntities(marioX + dx, marioY, success);
        }
    }
    else if (direction.x == -1 && direction.y == 0) { // Mario moving left
        for (int dx = -1; dx >= -3; --dx) { // Check up to 3 spaces behind
            checkAndRemoveEntities(marioX + dx, marioY, success);
        }
    }
    else { // Mario is stationary, check all directions
        for (const auto& d : Point::directions) { // Iterate over all possible directions
            for (int i = 1; i <= 3; ++i) { // Check up to 3 spaces in each direction
                checkAndRemoveEntities(marioX + d.x * i, marioY + d.y * i, success);
            }
        }
    }

    // Handle the hammer's status after use
    if (success) {
        hammer.printP(); // Indicate the hammer is still available
    }
    else {
        player.hasHammer = false; // Remove the hammer from Mario
        hammer.deletetP(); // Clear the hammer from the board
    }
}

// Checks and removes entities (barrels or ghosts) at a given position
void Game::checkAndRemoveEntities(int checkX, int checkY, bool& success) {
    if (checkX < 0 || checkX >= MAX_X || checkY < 0 || checkY >= MAX_Y) return; // Ensure position is within bounds

    // Check for barrels
    for (auto it = barrels.begin(); it != barrels.end(); ) {
        if (it->getX() == checkX && it->getY() == checkY) { // Barrel found
            it->erase(noColors); // Erase the barrel from the screen
            it = barrels.erase(it); // Remove it from the list
            success = true; // Mark success
            player.score += KILL_BARREL_SCORE; // Add points for destroying a barrel
            return; // Stop further checks for this position
        }
        else {
            ++it; // Move to the next barrel
        }
    }

    // Check for ghosts
    for (auto it = ghosts.begin(); it != ghosts.end(); ) {
        if (it->getX() == checkX && it->getY() == checkY) { // Ghost found
            it->erase(noColors); // Erase the ghost from the screen
            it = ghosts.erase(it); // Remove it from the list
            success = true; // Mark success
            player.score += KILL_GHOST_SCORE; // Add points for destroying a ghost
            return; // Stop further checks for this position
        }
        else {
            ++it; // Move to the next ghost
        }
    }
}

// Loads all board files from the specified directory
void Game::loadBoardFiles(const std::string& directory) {
    boardFiles.clear(); // Clear any existing board files
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".screen") { // Check for .screen files
            boardFiles.push_back(entry.path().string()); // Add the board file to the list
        }
    }
    std::sort(boardFiles.begin(), boardFiles.end()); // Sort the board files alphabetically
    currentBoardIndex = 0; // Start with the first board
}

// Loads a specific board by index
void Game::loadBoardByIndex(int index) {
    if (index < 0 || index >= static_cast<int>(boardFiles.size())) {
        throw std::out_of_range("Board index out of range"); // Throw an error if the index is invalid
    }
    currentBoardIndex = index; // Set the current board index

    try {
        board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Load the selected board

        if (!validateBoard()) { // Validate the board configuration
            return; // Return to the main menu if the board is invalid
        }

        std::cout << "Board loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading board: " << e.what() << std::endl; // Handle file loading errors
        throw;
    }
}

// Starts the level timer
void Game::startLevel() {
    startTime = std::chrono::high_resolution_clock::now(); // Record the start time
}

// Ends the level timer
void Game::endLevel() {
    endTime = std::chrono::high_resolution_clock::now(); // Record the end time
}

// Calculates the elapsed time for the level
double Game::getElapsedTime() const {
    std::chrono::duration<double> elapsedTime = endTime - startTime; // Calculate elapsed time
    return elapsedTime.count(); // Return the time in seconds
}

// Validates the current board to ensure it meets the game requirements
bool Game::validateBoard() {
    drawBorders(); // Draw the board's borders

    // Validate the presence and configuration of key elements on the board
    if (!checkMario() || !checkPau() || !checkLegend() || !checkDonkey() || !checkGhost() || !checkInvalidChar()) {
        return false;
    }
    return true;
}

// Draws the borders of the board, ensuring they are properly formatted
void Game::drawBorders() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            // Draw side borders
            if (x == 0 || x == MAX_X - 1) {
                if (board.getChar(x, y) != 'Q') {
                    board.setChar(x, y, 'Q'); // Set the side border character
                }
            }
            // Replace invalid 'Q' characters inside the board
            else if (board.getChar(x, y) == 'Q') {
                board.setChar(x, y, ' '); // Replace with a space
            }

            // Replace null characters ('\0') with spaces
            if (board.getChar(x, y) == '\0') {
                board.setChar(x, y, ' ');
            }

            // Draw the bottom border
            if (y == MAX_Y - 1) {
                if (x != 0 && x != MAX_X - 1 &&
                    board.getChar(x, y) != '=' && board.getChar(x, y) != '>' && board.getChar(x, y) != '<') {
                    board.setChar(x, y, '=');
                }
            }
        }
    }
}

// Ensures there is exactly one Mario on the board
bool Game::checkMario() {
    countMario = 0; // Reset the Mario counter

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '@') { // Check for Mario's marker
                countMario++;
            }
        }
    }

    if (countMario == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no Mario or more than one. Choose another board..." << std::endl;
        return false;
    }
}

// Ensures there is exactly one Pauline on the board
bool Game::checkPau() {
    countPau = 0; // Reset the Pauline counter

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '$') { // Check for Pauline's marker
                countPau++;
            }
        }
    }

    if (countPau == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no Pauline or more than one. Choose another board..." << std::endl;
        return false;
    }
}

// Ensures there is exactly one Donkey Kong on the board
bool Game::checkDonkey() {
    countDonkey = 0; // Reset the Donkey Kong counter

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '&') { // Check for Donkey Kong's marker
                countDonkey++;
            }
        }
    }

    if (countDonkey == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no Donkey Kong or more than one. Choose another board..." << std::endl;
        return false;
    }
}

// Ensures there is exactly one Legend marker on the board
bool Game::checkLegend() {
    countLegend = 0; // Reset the Legend counter

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'L') { // Check for Legend marker
                countLegend++;
            }
        }
    }

    if (countLegend == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no Legend or more than one. Choose another board..." << std::endl;
        return false;
    }
}

// Ensures all ghosts ('x') on the board are properly placed
bool Game::checkGhost() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'x' && (board.getChar(x, y + 1) != '=' && board.getChar(x, y + 1) != '<' && board.getChar(x, y + 1) != '>')) {
                std::cout << "Invalid board, Ghost can not be on air. Choose another board..." << std::endl;
                return false;
            }
        }
    }
    return true;
}

// Ensures there are no invalid characters on the board
bool Game::checkInvalidChar() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char currentChar = board.getChar(x, y);
            // Check if the character is not part of the valid set
            if (currentChar != '@' && currentChar != '&' && currentChar != '$' && currentChar != 'L' &&
                currentChar != '=' && currentChar != '>' && currentChar != '<' && currentChar != 'Q' &&
                currentChar != 'p' && currentChar != 'H' && currentChar != ' ' && currentChar != 'x') {
                std::cout << "Invalid board, there is an invalid char. Choose another board..." << std::endl;
                return false; // Return false if an invalid character is found
            }
        }
    }
    return true; // Return true if all characters are valid
}

// Ensures all ladders ('H') on the board are properly placed
bool Game::checkLadder() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'H') { // Check for ladder markers
                // Check if the ladder has empty space below or above it
                if ((y + 1 < MAX_Y && board.getChar(x, y + 1) == ' ') ||
                    (y - 1 >= 0 && board.getChar(x, y - 1) == ' ')) {
                    std::cout << "Invalid board: There is a problem with the ladder. Choose another board..." << std::endl;
                    return false;
                }
            }
        }
    }
    return true; // Return true if all ladders are valid
}