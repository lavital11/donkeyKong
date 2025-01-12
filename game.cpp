#include <windows.h>
#include <conio.h>
#include "Game.h"
#include <chrono> // Usage for handling time
#include <time.h>
#include <filesystem> // For directory iteration and sorting


constexpr int  PAU_X = 38;
constexpr int  PAU_Y = 0;
constexpr int  NUM_HAM_LOC = 4;

constexpr int ESC = 27; // ESC key code
using namespace std::chrono;
namespace fs = std::filesystem;

// Constructor initializes the game state and player
Game::Game() : isGameOver(false), isPaused(false), noColors(false), player(this) {}

void Game::startGame() {
    loadBoardFiles("boards/"); // Load all board files from the "boards" directory
    int choice = 0;            // Variable for user choice
    do {
        system("cls"); // Clear the screen
        std::cout << "(1) Start a new game with colors" << std::endl;
        std::cout << "(2) Start a new game without colors" << std::endl;
        std::cout << "(3) Select a specific board" << std::endl;
        std::cout << "(8) Present instructions and keys" << std::endl;
        std::cout << "(9) EXIT" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: // Start game with colors
            noColors = false;
            system("cls");
            runGame();
            break;

        case 2: // Start game without colors
            noColors = true;
            system("cls");
            runGame();
            break;

        case 3: { // Select a specific board
            system("cls");
            std::cout << "Available boards:\n";
            for (size_t i = 0; i < boardFiles.size(); ++i) {
                std::cout << "(" << i + 1 << ") " << boardFiles[i] << "\n";
            }
            std::cout << "Enter the board number to load: ";
            int boardChoice;
            std::cin >> boardChoice;
            if (boardChoice >= 1 && boardChoice <= static_cast<int>(boardFiles.size())) {
                loadBoardByIndex(boardChoice - 1);
                std::cout << "Board loaded successfully.\n";
                Sleep(1500);
            }
            else {
                std::cout << "Invalid board number. Returning to main menu...\n";
                Sleep(1500);
            }
            break;
        }

        case 8: // Show instructions
            system("cls");
            showInstructions();
            break;

        case 9: // Exit game
            std::cout << "Exiting the game. Goodbye!" << std::endl;
            break;

        default: // Invalid choice handling
            std::cout << "Invalid choice. Please try again." << std::endl;
            std::cin.clear();             // Clear the error flag on cin
            std::cin.ignore(256, '\n');   // Ignore the invalid input
            Sleep(1500); // Small delay for better user experience
            break;
        }
    } while (choice != 9); // Exit loop when the user chooses to quit
}
/*void Game::startGame() {
    int choice = 0; // Variable for user choice
    do {
        system("cls"); // Clear the screen
        std::cout << "(1) Start a new game with colors" << std::endl;
        std::cout << "(2) Start a new game without colors" << std::endl;
        std::cout << "(8) Present instructions and keys" << std::endl;
        std::cout << "(9) EXIT" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: // Start game with colors
            noColors = false;
            system("cls");
            runGame();
            break;
        case 2: // Start game without colors
            noColors = true;
            system("cls");
            runGame();
            break;

        case 8: // Show instructions
            system("cls");
            showInstructions();
            break;

        case 9: // Exit game
            std::cout << "Exiting the game. Goodbye!" << std::endl;
            break;

        default: // Invalid choice handling
            std::cout << "Invalid choice. Please try again." << std::endl;
            std::cin.clear(); // Clear the error flag on cin
            std::cin.ignore(256, '\n'); // Ignore the invalid input
            Sleep(1500); // Small delay for better user experience
            break;
        }
    } while (choice != 9); // Exit loop when the user chooses to quit
}*/

void Game::showInstructions() {
    system("cls");
    // Print game instructions
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

void Game::runGame() {
    isGameOver = false;
    isPaused = false;
    player.life = 3; // Reset player's lives
    ShowConsoleCursor(false); // Hide cursor for game aesthetics
    board.reset(); // Initialize the board state
    board.print(noColors); // Print the board based on color mode
    player.setBoard(board); // Associate player with the board
    player.printLife(); // Display player's lives
    createHammer();

    // Timers for game mechanics
    auto lastBarrelSpawnTime = steady_clock::now();
    auto lastBarrelUpdateTime = steady_clock::now();
    auto lastMarioUpdateTime = steady_clock::now();

    constexpr int barrelSpawnInterval = 1500; // Interval for spawning barrels (milliseconds)
    constexpr int barrelUpdateInterval = 100; // Interval for updating barrels (milliseconds)
    constexpr int marioUpdateInterval = 80; // Interval for updating Mario (milliseconds)

    while (!isGameOver) {
        auto currentTime = steady_clock::now(); // Get current time
        

        if (_kbhit()) { // Check for keyboard input
            char key = _getch();
            if (key == ESC) // Handle pause
                pauseGame();
            else
                player.keyPressed(key); // Pass the key press to the player
        }

        // Update barrels at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastBarrelUpdateTime).count() >= barrelUpdateInterval) {
            updateBarrels();
            updateGhosts();
            lastBarrelUpdateTime = currentTime;
        }
        if(ghosts.size() < 5)
             createGhost();

        // Spawn barrels at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastBarrelSpawnTime).count() >= barrelSpawnInterval) {
            spawnBarrel();
            lastBarrelSpawnTime = currentTime;
        }

        // Update Mario's movement at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastMarioUpdateTime).count() >= marioUpdateInterval) {
            player.erase(noColors); // Erase Mario's previous position
            player.move(noColors); // Update Mario's position
            player.draw(noColors); // Draw Mario at the new position
            lastMarioUpdateTime = currentTime;
        }

        checkCollision(); // Check if Mario collides with barrels
        checkWin(); // Check if Mario reached the win condition

        Sleep(10); // Short sleep to reduce CPU usage
    }
}

void Game::pauseGame() {
    isPaused = true;

    system("cls");
    // Display pause message
    std::cout << "Game paused. Press ESC again to continue..." << std::endl;

    while (true) {
        if (_kbhit()) { // Wait for key press to resume
            char key = _getch();
            if (key == ESC) {
                resumeGame();
                break;
            }
        }
        Sleep(100); // Reduce CPU usage while waiting
    }
}

void Game::resumeGame() {
    isPaused = false;
    system("cls");
    board.print(noColors); // Redraw the board
    player.printLife(); // Redisplay player's lives
}

void Game::endGame() {
    isGameOver = true;
    restartLevel(); // Reset the level state
    resetBarrels(); // Clear barrels from the game
    system("cls"); // Clear the screen

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY); // Set red color for message
    }

    printLoseMessage(); // Display "Game Over" message

    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset colors
    }

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
    while (!_kbhit()) { // Wait for user input
        Sleep(100);
    }
    char key = _getch(); // Capture key press to continue
}
Point Game::getMarioPosition() const {
    return player.getPosition(); // Retrieve the current position of Mario
}

void Game::updateBarrels() {
    static auto lastUpdateTime = steady_clock::now();
    auto currentTime = steady_clock::now();
    auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();

    if (elapsedTime >= 100) { // Update barrels every 100 milliseconds
        std::vector<decltype(barrels.begin())> toRemove;

        for (auto it = barrels.begin(); it != barrels.end(); ++it) {
            if (it->shouldRemove()) { // Identify barrels to be removed
                toRemove.push_back(it);
            }
        }

        for (auto& it : toRemove) {
            it->erase(noColors); // Erase barrel from screen
            barrels.erase(it); // Remove barrel from the list
        }

        for (auto& barrel : barrels) {
            barrel.move(noColors); // Update barrel position
        }

        lastUpdateTime = currentTime; // Reset the timer for next update
    }
}
void Game::updateGhosts() {
    static auto lastUpdateTime = steady_clock::now();
    auto currentTime = steady_clock::now();
    auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();

    if (elapsedTime >= 100) { // Update barrels every 100 milliseconds
        std::vector<decltype(ghosts.begin())> toRemove;

        for (auto it = ghosts.begin(); it != ghosts.end(); ++it) {
            if (it->shouldRemove()) { // Identify ghost to be removed
                toRemove.push_back(it);
            }
        }

        for (auto& it : toRemove) {
            it->erase(noColors); // Erase ghost from screen
            ghosts.erase(it); // Remove ghost from the list
        }
        for (size_t i = 0; i < ghosts.size(); ++i) {
            for (size_t j = i + 1; j < ghosts.size(); ++j) {
                if (ghosts[i].getX() == ghosts[j].getX() && ghosts[i].getY() == ghosts[j].getY()) {
                    ghosts[i].changeDir();
                    ghosts[j].changeDir();
                }
            }
        }
        for (auto& ghost : ghosts) {
            ghost.move(noColors); // Update ghost position
        }

        lastUpdateTime = currentTime; // Reset the timer for next update
    }
}

void Game::checkCollision() {
    for (auto& barrel: barrels) {
        if (player.isCollidingBarrel(barrel)) { // Check if Mario collides with a barrel
            handleCollision(); // Handle collision logic
            break;
        }
        if (barrel.isExplodeAndKillMario()) { // Check if a barrel explodes near Mario
            handleCollision(); // Handle explosion logic
            break;
        }
    }
    for (auto& ghost1: ghosts) {
        if (player.isCollidingGhost(ghost1)) { // Check if Mario collides with a ghost
            handleCollision(); // Handle collision logic
            break;
        }
    }
}

void Game::handleCollision() {
    player.erase(noColors); // Erase Mario from his current position
    player.loseLife(); // Decrement Mario's life count
    resetBarrels(); // Clear barrels from the board
    resetGhosts();
    if (player.life <= 0) { // Check if Mario has no lives left
        endGame(); // End the game
    }
    else {
        restartLevel(); // Restart the current level
    }
}

bool Game::shouldSpawnBarrel() {
    constexpr int spawnChance = 10; // 10% chance to spawn a barrel
    return (rand() % 100) < spawnChance;
}

void Game::spawnBarrel() {
    int startX = DONK_X; // Set initial X position of the barrel
    int startY = DONK_Y; // Set initial Y position of the barrel

    Barrels newBarrel(&board, startX, startY, this); // Create a new barrel instance

    newBarrel.setInitialDirection(); // Set the initial movement direction of the barrel

    barrels.push_back(newBarrel); // Add the new barrel to the list
}

void Game::checkWin() {
    if (player.getX() == PAU_X && player.getY() == PAU_Y) // Check if Mario reached the winning position
        winGame(); // Handle win scenario
}

void Game::winGame() {
    isGameOver = true; // Mark the game as over
    restartLevel(); // Reset the level
    resetBarrels(); // Clear barrels
    system("cls"); // Clear the screen

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Set green color for win message
    }

    printWinMessage(); // Display the win message

    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset text color
    }

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
    while (!_kbhit()) { // Wait for user input
        Sleep(100);
    }
    char key = _getch(); // Wait for any key to continue
}

void Game::printWinMessage() {
    const int screenWidth = 80; // Total width of the console screen
    const int screenHeight = 25; // Total height of the console screen
    const std::string line1 = "*****************************************"; // Top and bottom border of the message box
    const std::string line2 = "*                                       *"; // Empty line for padding within the box
    const std::string line3 = "*               YOU WIN!                *"; // Main win message
    const std::string line4 = "*            Level Complete!            *"; // Submessage indicating level completion

    const int messageHeight = 6; // Total height of the message box
    const size_t startRow = (screenHeight - messageHeight) / 2; // Vertical center of the screen
    const size_t centerColumn = (screenWidth - line1.length()) / 2; // Horizontal center of the screen

    for (int i = 0; i < startRow; ++i) {
        std::cout << std::endl; // Print empty lines to vertically center the message
    }

    // Print each line of the message box centered horizontally
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl; // Top border
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl; // Padding
    std::cout << std::string(centerColumn, ' ') << line3 << std::endl; // Main win message
    std::cout << std::string(centerColumn, ' ') << line4 << std::endl; // Submessage
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl; // Padding
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl; // Bottom border

    std::cout << std::endl; // Add an extra empty line after the box

    // Print the instruction to return to the main menu centered horizontally
    std::cout << std::string(centerColumn, ' ') << "Press any key to return to the main menu..." << std::endl;
}


void Game::printLoseMessage() {
    const int screenWidth = 80; // Total width of the console screen
    const int screenHeight = 25; // Total height of the console screen
    const std::string line1 = "*****************************************"; // Top and bottom border of the message box
    const std::string line2 = "*                                       *"; // Empty line for padding within the box
    const std::string line3 = "*              GAME OVER!               *"; // Main lose message
    const std::string line4 = "*     Mario has lost all his lives.     *"; // Submessage providing details

    const int messageHeight = 6; // Total height of the message box
    const size_t startRow = (screenHeight - messageHeight) / 2; // Vertical center of the screen
    const size_t centerColumn = (screenWidth - line1.length()) / 2; // Horizontal center of the screen

    for (int i = 0; i < startRow; ++i) {
        std::cout << std::endl; // Print empty lines to vertically center the message
    }

    // Print each line of the message box centered horizontally
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl; // Top border
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl; // Padding
    std::cout << std::string(centerColumn, ' ') << line3 << std::endl; // Main lose message
    std::cout << std::string(centerColumn, ' ') << line4 << std::endl; // Submessage
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl; // Padding
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl; // Bottom border

    std::cout << std::endl; // Add an extra empty line after the box

    // Print the instruction to return to the main menu centered horizontally
    std::cout << std::string(centerColumn, ' ') << "Press any key to return to the main menu..." << std::endl;
}


void Game::resetBarrels() {
    for (auto& barrel : barrels) {
        barrel.erase(noColors); // Erase each barrel from the screen
    }
    barrels.clear(); // Clear the barrel list

    gotoxy(DONK_X, DONK_Y); // Move to Donkey Kong's position
    std::cout << ' '; // Clear Donkey Kong's spot
}
void Game::resetGhosts() {
    for (auto& ghost : ghosts) {
        ghost.erase(noColors); // Erase each barrel from the screen
    }
    ghosts.clear(); // Clear the barrel list

   // gotoxy(DONK_X, DONK_Y); // Move to Donkey Kong's position
    //std::cout << ' '; // Clear Donkey Kong's spot*/
}

void Game::restartLevel() {
    resetBarrels(); // Clear all existing barrels from the game
    player.drawStart(noColors); // Reset Mario's position and draw him at the starting point
    board.reset(); // Reset the game board to its initial state
}

void Game::createGhost() {
    Point startGhost[10]; 
    int startx1 = 10, startx2 = 50, starty1 = 7, starty2 = 16, starty3 = 23;

    Ghost newGhost1(&board, startx1, starty1, this);
    newGhost1.setInitialDirection();
    ghosts.push_back(newGhost1);

    Ghost newGhost2(&board, startx1, starty2, this);
    newGhost2.setInitialDirection();
    ghosts.push_back(newGhost2);

    Ghost newGhost3(&board, startx1, starty3, this);
    newGhost3.setInitialDirection();
    ghosts.push_back(newGhost3);

    Ghost newGhost4(&board, startx2, starty2, this);
    newGhost4.setInitialDirection();
    ghosts.push_back(newGhost4); 

    Ghost newGhost5(&board, startx2, starty3, this);
    newGhost5.setInitialDirection();
    ghosts.push_back(newGhost5);
    
}
void Game::createHammer() {
    Point hammerLoc[4] = { {70,13},{8,13},{5,7},{35,10} };
    srand(time(NULL));
    int randomIndex = rand() % NUM_HAM_LOC;
    Point hammerCurLoc = hammerLoc[3];
    Hammer newHammer(&board, hammerCurLoc.getX(), hammerCurLoc.getY(), this);
    newHammer.draw(noColors);
}



void Game::loadBoardFiles(const std::string& directory) {
    boardFiles.clear();
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            boardFiles.push_back(entry.path().string());
        }
    }
    std::sort(boardFiles.begin(), boardFiles.end());
    currentBoardIndex = 0; // Start with the first board
}

void Game::loadBoardByIndex(int index) {
    if (index < 0 || index >= static_cast<int>(boardFiles.size())) {
        throw std::out_of_range("Board index out of range");
    }
    currentBoardIndex = index;
    board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Load the selected board
}