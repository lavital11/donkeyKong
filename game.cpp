#include <windows.h>
#include <conio.h>
#include <time.h>
#include <filesystem>
#include "Game.h"

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

namespace fs = std::filesystem;
using namespace std::chrono;

// Constructor initializes the game state and player
Game::Game() : isGameOver(false), isPaused(false), noColors(false), currentBoardIndex(0), hammer(nullptr, -1, -1, nullptr), player(this,-1,-1,nullptr), hammerOriginalX(0), hammerOriginalY(0), DONK_X(0), DONK_Y(0), PAU_X(0), PAU_Y(0) {}

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
        case WITH_COLORS: // Start game with colors
            noColors = false;
            system("cls");
            runGame();
            break;

        case NO_COLORS: // Start game without colors
            noColors = true;
            system("cls");
            runGame();
            break;

        case SPEC_BOARD: { // Select a specific board
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
                Sleep(3000);
            }
            else {
                std::cout << "Invalid board number. Returning to main menu...\n";
                Sleep(1500);
            }
            break;
        }

        case INSTRUCTIONS: // Show instructions
            system("cls");
            showInstructions();
            break;

        case EXIT: // Exit game
            std::cout << "Exiting the game. Goodbye!" << std::endl;
            break;

        default: // Invalid choice handling
            std::cout << "Invalid choice. Please try again." << std::endl;
            std::cin.clear();             // Clear the error flag on cin
            std::cin.ignore(256, '\n');   // Ignore the invalid input
            Sleep(1500); // Small delay for better user experience
            break;
        }
    } while (choice != EXIT); // Exit loop when the user chooses to quit
}

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
    player.score = 0; // Reset player's score
    ShowConsoleCursor(false); // Hide cursor for game aesthetics
    board.reset(boardFiles[currentBoardIndex]); // Initialize the board state
    // Validate the default board before starting the game
    if (!validateBoard()) {
        Sleep(3000); // Wait for 2 seconds to let the user read the message
        return; // Return to the main menu
    }
    board.print(noColors); // Print the board based on color mode
    player.setBoard(board); // Associate player with the board
    createMario();
    createHammer();
    createGhost();
    createLegend();
    startLevel();

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
            else if (key == 'p' || key == 'P') // שימוש בפטיש
                useHammer();
            else
                player.keyPressed(key); // Pass the key press to the player
        }

        // Update barrels at regular intervals
        if (duration_cast<milliseconds>(currentTime - lastBarrelUpdateTime).count() >= barrelUpdateInterval) {
            updateBarrels();
            updateGhosts();
            lastBarrelUpdateTime = currentTime;
        }

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

        player.printScore(SCORE_X, SCORE_Y);
        collectHammer();
        checkCollision(); // Check if Mario collides with barrels
        checkLevelPass();

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
    ignoreOldGhost();
    board.print(noColors); // Redraw the board
    createLegend();
}

void Game::endGame() {
    isGameOver = true;
    restartLevel(); // Reset the level state
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
    auto currentTime = steady_clock::now();
    auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();
    
    if (elapsedTime >= 100) { // Update barrels every 100 milliseconds
        // Use a loop with erase to safely remove elements
        for (auto it = barrels.begin(); it != barrels.end(); ) {
            if (it->shouldRemove()) { // Identify barrels to be removed
                it->erase(noColors); // Erase barrel from screen
                it = barrels.erase(it); // Remove barrel from the list and get the next iterator
            }
            else {
                ++it; // Move to the next barrel
            }
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

    if (elapsedTime >= 100) { // Update ghosts every 100 milliseconds
        // Use a loop with erase to safely remove elements
        for (auto it = ghosts.begin(); it != ghosts.end(); ) {
            if (it->shouldRemove()) { // Identify ghosts to be removed
                it->erase(noColors); // Erase ghost from screen
                it = ghosts.erase(it); // Remove ghost from the list and get the next iterator
            }
            else {
                ++it; // Move to the next ghost
            }
        }

        // Check for collisions between ghosts
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
    player.score += DIE;
    if (player.life <= 0) { // Check if Mario has no lives left
        endGame(); // End the game
    }
    else {
        restartLevel(); // Restart the current level
    }
}

bool Game::shouldSpawnBarrel() {
    constexpr int spawnChance = 5; // 5% chance to spawn a barrel
    return (rand() % 100) < spawnChance;
}

void Game::spawnBarrel() {
    createDonkey();

    Barrels newBarrel(&board, DONK_X, DONK_Y, this); // Create a new barrel instance

    newBarrel.setInitialDirection(); // Set the initial movement direction of the barrel

    barrels.push_back(newBarrel); // Add the new barrel to the list

}

void Game::checkLevelPass() {
    if (player.getX() == PAU_X && player.getY() == PAU_Y) // Check if Mario reached the winning position
    {
        endLevel();
        gameTime = getElapsedTime();
        if (gameTime < 60)
        {
            player.score += LESS_THAN_A_MINUITE_SCORE;
        }
        player.score += FINISH_LEVEL;
        if (currentBoardIndex + 1 < static_cast<int>(boardFiles.size()))
            nextLevel();
        else
            winGame();
    }  
}

void Game::nextLevel() {
    // Reset the level and game objects
    resetBarrels(); // Clear barrels from the game
    resetGhosts();  // Clear ghosts from the game
    system("cls");  // Clear the screen

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Set blue color for the next level message
    }
    printNextLevelMessage(); // Display the message for the next level

    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset colors
    }

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
    while (!_kbhit()) { // Wait for user input
        Sleep(100);
    }
    char key = _getch(); // Capture key press to continue

    // Clear the screen after the message
    system("cls");

    ++currentBoardIndex; // Move to the next board
    board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Load the next board
    restartLevel(); // Reset the level for the new board
    board.print(noColors); // Display the new board
    createLegend();
    player.printLife(LIFE_X, LIFE_Y); // Display Mario's remaining lives
    player.printScore(SCORE_X, SCORE_Y);
}

void Game::winGame() {
    isGameOver = true; // Mark the game as over
    currentBoardIndex = 0; // Reset to the first board (default board)
    restartLevel(); // Reset the level
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
    const std::string line4 = "*            game Completed!            *"; // Submessage indicating level completion

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

void Game::printNextLevelMessage() const {
    const int screenWidth = 80; // Total width of the console screen
    const int screenHeight = 25; // Total height of the console screen

    std::string line1 = "*****************************************"; // Top and bottom border of the message box
    std::string line2 = "*                                       *";
    std::string line3;
    std::string line4 = "*        YOU PASSED THIS LEVEL!         *"; // Main win message
    std::string line5 = "*          EARNED 300 POINTS!           *"; // Main win message
    std::string line6 = "*     GOOD LUCK IN THE NEXT LEVEL!      *"; // Submessage indicating level completion

    // Check if the player gets a bonus for finishing quickly
    if (gameTime < 60) {
        line3 = "*   100 POINTS BONUS FOR BEING FAST!    *"; // Bonus message
    }
    else {
        line3 = "*                                       *"; // Empty padding
    }

    const int messageHeight = 6; // Total height of the message box
    const size_t startRow = (screenHeight - messageHeight) / 2; // Vertical center of the screen
    const size_t centerColumn = (screenWidth - line1.length()) / 2; // Horizontal center of the screen

    for (int i = 0; i < startRow; ++i) {
        std::cout << std::endl; // Print empty lines to vertically center the message
    }

    // Print each line of the message box centered horizontally
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl; // Top border
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl; // empty line
    std::cout << std::string(centerColumn, ' ') << line3 << std::endl; // Bonus orempty line
    std::cout << std::string(centerColumn, ' ') << line4 << std::endl; // Main win message
    std::cout << std::string(centerColumn, ' ') << line5 << std::endl; // Submessage
    std::cout << std::string(centerColumn, ' ') << line6 << std::endl; // Submessage
    std::cout << std::string(centerColumn, ' ') << line2 << std::endl; // Padding
    std::cout << std::string(centerColumn, ' ') << line1 << std::endl; // Bottom border

    std::cout << std::endl; // Add an extra empty line after the box

    // Print the instruction to return to the main menu centered horizontally
    std::cout << std::string(centerColumn, ' ') << "Press any key to start the next level..." << std::endl;
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
    countMario = 0;
    countLegend = 0;
    resetBarrels(); // Clear all existing barrels from the game
    resetGhosts();  // Clear all existing ghosts
    resetHammer();
     // Reset Mario's position and draw him at the starting point
    board.reset(boardFiles[currentBoardIndex]); // Reset the game board to its initial state
    board.print(noColors);
    createGhost();
    createHammer();
    createMario();
    player.drawStart(noColors);
    createLegend();
}

void Game::createGhost() {
    // Clear the existing list of ghosts
    ghosts.clear();

    // Iterate over the board to find all 'x' characters
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'x') { // Check if the character is 'x'
                // Create a new Ghost object and add it to the list of ghosts
                Ghost newGhost(&board, x, y, this);
                newGhost.setInitialDirection();
                ghosts.push_back(newGhost);
            }
        }
    }
}

void Game::ignoreOldGhost() {
    // Iterate over the board to find all 'x' characters
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'x') { // Check if the character is 'x'
                board.setChar(x, y, ' ');
            }
        }
    }
}

void Game::createHammer() {
    if (hammer.getX() == -1 && hammer.getY() == -1) {
        // פטיש כבר לא קיים, חפש מחדש את ה- p בלוח
        for (int y = 0; y < MAX_Y; ++y) {
            for (int x = 0; x < MAX_X; ++x) {
                if (board.getChar(x, y) == 'p') { // אם מצאנו את ה-p בלוח
                    hammer = Hammer(&board, x, y, this); // צור את הפטיש מחדש
                    board.setChar(x, y, 'p'); // ודא שהפטיש מופיע בלוח
                    return;
                }
            }
        }
    }
    else {
        // אם לפטיש יש מיקום תקין, ודא שהוא מודפס בלוח
        board.setChar(hammer.getX(), hammer.getY(), 'p');
    }
}

void Game::createLegend() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'L') { // אם מצאנו את ה-Legend
                printLegend(x, y); // הדפס את ה-Legend
                return; // סיימנו - לא צריך להמשיך
            }
        }
    }
}

void Game::createMario() {
    // שמור את מספר החיים והניקוד הנוכחיים של מריו
    int currentLife = player.life;
    int currentScore = player.score;

    // סרוק את הלוח כדי למצוא את מריו
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '@') { // אם מצאנו את מריו בלוח
                // צור את מריו במיקום החדש ושחזר את הפרטים הקודמים
                player = Mario(this, x, y, &board);
                player.life = currentLife;
                player.score = currentScore;
                player.drawStart(noColors); // צייר את מריו בלוח
                return; // סיימנו
            }
        }
    }
}

void Game::createDonkey() {
    // Iterate through the board to find the '&' character
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            if (board.getChar(x, y) == '&') {
                DONK_X = x; // Set the X position of Donkey
                DONK_Y = y; // Set the Y position of Donkey
                return; // Exit inner loop when found
            }
        }
    }
}

void Game::createPau() {
    // Iterate through the board to find the '&' character
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            if (board.getChar(x, y) == '$') {
                PAU_X = x; // Set the X position of Donkey
                PAU_Y = y; // Set the Y position of Donkey
                return; // Exit inner loop when found
            }
        }
    }
}

void Game::printLegend(int x, int y) {
    gotoxy(x, y);
    std::cout << "MARIO'S LIFE:"; // Display player's lives;
    player.printLife(LIFE_X, LIFE_Y);
    gotoxy(x, y+1);
    std::cout << "SCORE:";
    player.printScore(SCORE_X, SCORE_Y);
}

void Game::collectHammer() {
    if (player.getX() == hammer.getX() && player.getY() == hammer.getY()) {
        player.hasHammer = true; // עדכן שלמריו יש פטיש
        // שמירת המיקום המקורי של הפטיש
        hammerOriginalX = hammer.getX();
        hammerOriginalY = hammer.getY();
        hammer.setPosition(-1, -1); // איפוס המיקום של הפטיש
        board.setChar(player.getX(), player.getY(), ' '); // הסרת ה-'p' מהלוח
        hammer.printP(); // הצגת אינדיקציה שפטיש נאסף
    }
}

void Game::resetHammer() {
    if (hammerOriginalX != -1 && hammerOriginalY != -1) {
        board.setChar(hammerOriginalX, hammerOriginalY, 'p'); // החזרת ה-'p' ללוח
        hammer.setPosition(hammerOriginalX, hammerOriginalY); // עדכון מיקום הפטיש
    }
}

void Game::useHammer() {
    if (!player.hasHammer) return; // אם למריו אין פטיש, אל תעשה כלום

    bool success = false;
    int marioX = player.getX();
    int marioY = player.getY();

    // שימוש בכיוון התנועה הנוכחי של מריו באמצעות getDirection
    Point::Direction direction = player.getDirection();

    // בדיקה לפי כיוון התנועה
    if (direction.x == 1 && direction.y == 0) {
        // אם מריו נע ימינה
        for (int dx = 1; dx <= 3; ++dx) { // עד מרחק של 3 תווים
            int checkX = marioX + dx;
            int checkY = marioY;

            checkAndRemoveEntities(checkX, checkY, success);
        }
    }
    else if (direction.x == -1 && direction.y == 0) {
        // אם מריו נע שמאלה
        for (int dx = -1; dx >= -3; --dx) { // עד מרחק של 3 תווים
            int checkX = marioX + dx;
            int checkY = marioY;

            checkAndRemoveEntities(checkX, checkY, success);
        }
    }
    else {
        // אם מריו סטטי, בדוק לכל הכיוונים
        for (const auto& d : Point::directions) {
            for (int i = 1; i <= 3; ++i) { // עד מרחק של 3 תווים
                int checkX = marioX + d.x * i;
                int checkY = marioY + d.y * i;

                checkAndRemoveEntities(checkX, checkY, success);
            }
        }
    }

    // אם הפטיש פגע במשהו, הוא נשאר אצל מריו
    if (success) {
        hammer.printP(); // עדכן שהפטיש עדיין זמין
    }
    else {
        // אם הפטיש לא פגע בכלום, מריו מאבד אותו
        player.hasHammer = false;
        hammer.deletetP();
    }
}

// פונקציה לבדיקת הסרה של חביות או רוחות מהמיקום הנתון
void Game::checkAndRemoveEntities(int checkX, int checkY, bool& success) {
    if (checkX < 0 || checkX >= MAX_X || checkY < 0 || checkY >= MAX_Y) return; // בדוק אם בגבולות

    // בדיקה על חביות
    for (auto it = barrels.begin(); it != barrels.end(); ) {
        if (it->getX() == checkX && it->getY() == checkY) {
            it->erase(noColors); // מחק את החבית מהמסך
            it = barrels.erase(it); // הסר את החבית מהרשימה
            success = true;
            player.score += KILL_BARREL_SCORE;
            return; // אין צורך לבדוק יותר
        }
        else {
            ++it;
        }
    }

    // בדיקה על רוחות
    for (auto it = ghosts.begin(); it != ghosts.end(); ) {
        if (it->getX() == checkX && it->getY() == checkY) {
            it->erase(noColors); // מחק את הרוח מהמסך
            it = ghosts.erase(it); // הסר את הרוח מהרשימה
            success = true;
            player.score += KILL_GHOST_SCORE;
            return; // אין צורך לבדוק יותר
        }
        else {
            ++it;
        }
    }
}

void Game::loadBoardFiles(const std::string& directory) {
    boardFiles.clear();
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".screen") {
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

    try {
        board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Load the selected board

        // Validate the board
        if (!validateBoard()) {
            return;
        }

        std::cout << "Board loaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading board: " << e.what() << std::endl;
        throw;
    }
}

void Game::startLevel()
{
    startTime = std::chrono::high_resolution_clock::now();
}

void Game::endLevel()
{
    endTime = std::chrono::high_resolution_clock::now();
}

double Game::getElapsedTime() const
{
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    return elapsedTime.count(); // החזרת הזמן בשניות
}

bool Game::validateBoard() {
    drawBorders(); // צייר גבולות
    if (!checkMario() || !checkPau() || !checkLegend() || !checkDonkey() || !checkGhost() || !checkInvalidChar()) {
        return false;
    }
    else
        return true;
}

void Game::drawBorders() {
    for (int i = 0; i < MAX_X; i++) {
        for (int j = 0; j < MAX_Y; j++) {
            // Draw top and bottom borders
            if (i == 0 || i == 79) {
                if (board.getChar(i, j) != 'Q') {
                    board.setChar(i, j, 'Q');
                }
            }
            else if (board.getChar(i, j) == 'Q') {
                // Replace any 'Q' not in the borders with a space
                board.setChar(i, j, ' ');
            }

            // Replace '\0' with space
            if (board.getChar(i, j) == '\0') {
                board.setChar(i, j, ' ');
            }

            // Draw bottom row border
            if (j == 24) {
                if ((i != 0 && i != 79) &&
                    (board.getChar(i, j) != '=' && board.getChar(i, j) != '>' && board.getChar(i, j) != '<')) {
                    board.setChar(i, j, '=');
                }
            }
        }
    }
}

bool Game::checkMario() {
    countMario = 0; // אפס את הספירה של מריו

    // סרוק את הלוח כדי למצוא מופעים של '@'
    for (int y = 0; y < MAX_Y; y++) { // הלולאה החיצונית סורקת שורות
        for (int x = 0; x < MAX_X; x++) { // הלולאה הפנימית סורקת עמודות
            if (board.getChar(x, y) == '@') {
                countMario++; // עדכון הספירה
            }
        }
    }

    // בדוק אם יש בדיוק מופע אחד של '@'
    if (countMario == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no Mario or more than one. Choose another board..." << std::endl;
        return false;
    }
}

bool Game::checkPau() {
    countPau = 0; 

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '$') {
                countPau++; // עדכון הספירה
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

bool Game::checkDonkey() {
    countDonkey = 0;

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == '&') {
                countDonkey++;
            }
        }
    }

    if (countDonkey == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no DonkeyKong or more than one. Choose another board..." << std::endl;
        return false;
    }
}

bool Game::checkLegend() {
    countLegend = 0;

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'L') {
                countLegend++;
            }
        }
    }

    if (countLegend == 1) {
        return true;
    }
    else {
        std::cout << "Invalid board, no L for Legend or more than one. Choose another board..." << std::endl;
        return false;
    }
}

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

bool Game::checkInvalidChar() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) != '@' && board.getChar(x, y) != '&' && board.getChar(x, y) != '$' && board.getChar(x, y) != 'L' && board.getChar(x, y) != '=' && board.getChar(x, y) != '>' && board.getChar(x, y) != '<' && board.getChar(x, y) != 'Q' && board.getChar(x, y) != 'p' && board.getChar(x, y) != 'H' && board.getChar(x, y) != ' ' && board.getChar(x, y) != 'x') {
                std::cout << "Invalid board,There is an invalid char. Choose another board..." << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool Game::checkLadder() {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'H') {
                // Check below the ladder
                if (y + 1 < MAX_Y && board.getChar(x, y + 1) == ' ') {
                    std::cout << "Invalid board: There is a problem with the ladder. Choose another board..." << std::endl;
                    return false;
                }
                // Check above the ladder
                if (y - 1 >= 0 && board.getChar(x, y - 1) == ' ') {
                    std::cout << "Invalid board: There is a problem with the ladder. Choose another board..." << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
}