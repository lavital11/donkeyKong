#include <windows.h>
#include "Game.h"

// Using namespace for filesystem and time utilities
namespace fs = std::filesystem;
using namespace std::chrono;

// Constructor: Initializes the game state and player
Game::Game() : isGameOver(false), isPaused(false), noColors(false), currentBoardIndex(0), hammer(nullptr, -1, -1, nullptr), player(this, -1, -1, nullptr), hammerOriginalX(0), hammerOriginalY(0), DONK_X(0), DONK_Y(0), PAU_X(0), PAU_Y(0), gameTime(gameTime), random_seed(0) {}

Game& Game::operator=(const Game& other) {
    if (this != &other) { // Prevent self-assignment
        isGameOver = other.isGameOver;
        isPaused = other.isPaused;
        noColors = other.noColors;
        board = other.board; // Assuming 'Board' has a valid operator=
        player = other.player; // Assuming 'Mario' has a valid operator=
        barrels = other.barrels; // Assuming vector supports copy assignment
        ghosts = other.ghosts;
        boardFiles = other.boardFiles;
        currentBoardIndex = other.currentBoardIndex;
        hammer = other.hammer;
        hammerOriginalX = other.hammerOriginalX;
        hammerOriginalY = other.hammerOriginalY;
        startTime = other.startTime;
        endTime = other.endTime;
        gameTime = other.gameTime;
        lastUpdateTime = other.lastUpdateTime;
        countMario = other.countMario;
        countLegend = other.countLegend;
        countPau = other.countPau;
        countDonkey = other.countDonkey;
        DONK_X = other.DONK_X;
        DONK_Y = other.DONK_Y;
        PAU_X = other.PAU_X;
        PAU_Y = other.PAU_Y;
    }
    return *this;
}


void Game::runGame() {
startTime = std::chrono::steady_clock::now();
isGameOver = false;
isPaused = false;
player.life = 3; // Reset player's lives
player.score = 0; // Reset player's score
ShowConsoleCursor(false); // Hide the cursor for a cleaner game interface
board.reset(boardFiles[currentBoardIndex]); // Load the current board
if (!validateBoard()) { // Validate the board for proper setup
    Sleep(2000); // Pause for 2 seconds to let the user read the error
    return; // Return to the main menu if the board is invalid
}

// Initialize the game elements
board.print(noColors, this->isSilentMode());
player.setBoard(board);
createMario();
createHammer();
createGhost();
createLegend();
createPau();
startLevel();
    
while (!isGameOver) {
    gameTime++;
    auto currentTime = steady_clock::now(); // Get the current time
    inputAction();
     if (getIsAuto()) {
        if (results.isFinishedBy(gameTime)) {
            whyFailed = "Results file reached finish while game hadn't!";
            if(!isSilentMode())
                reportResultError(whyFailed, resultsFilename, gameTime);
            flagTest = false;
            break;
        }
        else
             nextDeadIteration = results.getNextDeadIteration();
     }
    

        updateBarrels();

     //Spawn barrels at regular intervals
        if(shouldSpawnBarrel())
            spawnBarrel();
   

    updateGhosts(); // Update ghost positions
    player.erase(noColors, this->isSilentMode()); // Erase Mario's current position
    player.move(noColors, this->isSilentMode()); // Update Mario's position
    player.draw(noColors, this->isSilentMode()); // Draw Mario in the new position
    
    if (!isSilentMode())
       Sleep(40);
    
    player.printScore(SCORE_X, SCORE_Y, isSilentMode()); // Update score display
    collectHammer(); // Check if Mario collects the hammer
    checkCollision(); // Handle collisions with barrels or ghosts
     if (isGameOver && getIsAuto()) // becuse maybe hanldeCullosion change isGameOver to true
        break;
     if (nextDeadIteration == gameTime && getIsAuto()) {
         whyFailed = "Results file has a mario death event that didn't happen!";
         if (!isSilentMode())
              reportResultError(whyFailed, resultsFilename, gameTime);
        flagTest = false;
        break;
    }


    checkLevelPass(); // Check if Mario passes the level

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
    board.print(noColors, isSilent); // Reprint the board
    createLegend(); // Redraw the legend
}

// Ends the game and displays the "Game Over" message
void Game::endGame() {
    if (isSave)
    {
        saveStepAfterFinish();
    }
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

    if (!isAuto) {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
        while (!_kbhit()) { // Wait for any key press
            Sleep(100); // Reduce CPU usage while waiting
        }
        char key = _getch(); // Capture the key press to continue
    }
}

// Returns Mario's current position as a Point object
Point Game::getMarioPosition() const {
    return player.getPosition(); // Retrieve the player's position
}

// Updates all barrels' positions and handles their removal
void Game::updateBarrels() {
    //auto currentTime = steady_clock::now();
    //auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();

    //if (elapsedTime >= 100) { // Update barrels every 100 milliseconds
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

       // lastUpdateTime = currentTime; // Reset the timer for barrel updates
    }
//}

// Updates all ghosts' positions and handles their removal
void Game::updateGhosts() {
   // static auto lastUpdateTime = steady_clock::now();
   // auto currentTime = steady_clock::now();
   // auto elapsedTime = duration_cast<milliseconds>(currentTime - lastUpdateTime).count();

   // if (elapsedTime >= 100) { // Update ghosts every 100 milliseconds
        // Iterate through the ghosts list
        for (auto it = ghosts.begin(); it != ghosts.end(); ) {
            if (it->shouldRemove()) { // Check if the ghost should be removed
                it->erase(noColors, isSilentMode()); // Erase the ghost from the screen
                it = ghosts.erase(it); // Remove the ghost from the list
            }
            else {
                ++it; // Move to the next ghost
            }
        }

        for (auto& ghost : ghosts) {
            ghost.move(noColors, ghosts, isSilentMode()); // Update the ghost's position
        }
       // lastUpdateTime = currentTime; // Reset the timer for ghost updates
    }
//}

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

void Game::handleCollision() {
    if(getIsSave())
        results.addResult(gameTime, Results::loseLife);
     else if (getIsAuto())
    {
        if (results.popResult() != std::pair{ gameTime, 0 }) {
            whyFailed = "Results file doesn't match mario death!";
            if (!isSilentMode())
                reportResultError(whyFailed, resultsFilename, gameTime);
            flagTest = false;
            isGameOver = true;
        }
        
            nextDeadIteration = results.getNextDeadIteration();

    }
    
    player.erase(noColors, isSilentMode()); // Erase Mario from his current position
    player.loseLife(isSilentMode()); // Decrease Mario's life count
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

void Game::checkLevelPass() {
    if (player.getX() == PAU_X && player.getY() == PAU_Y) { // Check if Mario reached the finish
        if (getIsSave())
            results.addResult(gameTime, Results::finishLevel);
        endLevel(); // Stop the level timer

        if (gameTime < 230) { // Award bonus points for completing the level quickly
            player.score += LESS_THAN_A_MINUITE_SCORE;
        }

        player.score += FINISH_LEVEL; // Award points for finishing the level
        saveStepAfterFinish();

        if (getIsSave()) {
            saveResultsAfterFinish();
        }
        if (getIsAuto()) {
            if (results.back() != std::pair{ player.score, 2 }) {
                whyFailed = "Results file doesn't match mario score!";
                if (!isSilentMode())
                     reportResultError(whyFailed, resultsFilename, gameTime);
                flagTest = false;
                isGameOver = true;
                return;
            }
        }

        if (currentBoardIndex + 1 < static_cast<int>(boardFiles.size())) {
            nextLevel(); // Move to the next level if available
            gameTime = 0;

        }
        else {
            winGame(); // End the game with a victory if all levels are completed
        }
    }
}


// Determines if a barrel should be spawned based on random chance
bool Game::shouldSpawnBarrel() {
    constexpr int spawnChance = 5; // 5% chance to spawn a barrel
    int randomal = rand();
    return (randomal% 100) < spawnChance;
}

// Spawns a new barrel at Donkey Kong's position
void Game::spawnBarrel() {
    createDonkey(); // Ensure Donkey Kong's position is set
    Barrels newBarrel(&board, DONK_X, DONK_Y, this); // Create a new barrel
    newBarrel.setInitialDirection(barrels.size()); // Set the barrel's initial direction
    barrels.push_back(newBarrel); // Add the barrel to the list
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

    if (!isAuto) {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
        while (!_kbhit()) { // Wait for user input
            Sleep(100);
        }
        char key = _getch(); // Capture key press to proceed
    }
    system("cls"); // Clear the screen after the message

    ++currentBoardIndex; // Increment the board index to move to the next level
    readFiles(currentBoardIndex);
    board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Load the next board
    restartLevel(); // Reset the level for the new board
    board.print(noColors,isSilentMode()); // Display the new board
    createLegend(); // Display the legend
    player.printLife(LIFE_X, LIFE_Y,isSilentMode()); // Show Mario's remaining lives
    player.printScore(SCORE_X, SCORE_Y, isSilentMode()); // Show Mario's score

}

// Ends the game with a win message and resets the game state
void Game::winGame() {
    //results.addResult(gameTime, score);
    isGameOver = true; // Mark the game as over
    currentBoardIndex = 0; // Reset to the first board
    if(!getIsAuto())
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

    if (!isAuto) {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // Clear input buffer
        while (!_kbhit()) { // Wait for user input
            Sleep(100);
        }
        char key = _getch(); // Capture key press to continue
    }
    

}

// Displays the win message with formatted output
void Game::printWinMessage() const {
    if (isSilentMode()) return;
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
    if (isSilentMode()) return;
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
    if (isSilentMode()) return;

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
        ghost.erase(noColors, isSilentMode()); // Erase each ghost from the screen
    }
    ghosts.clear(); // Clear the ghost list
}

// Restarts the current level, resetting all game elements
void Game::restartLevel() {
    countMario = 0;      // Reset Mario counter
    countLegend = 0;     // Reset legend counter
    resetBarrels();      // Clear all barrels
    resetGhosts();       // Clear all ghosts
    resetHammer();       // Reset hammer state

    try {
        board.loadBoardFromFile(boardFiles[currentBoardIndex]); // Reload the board from file
    }
    catch (const std::exception& e) {
        std::cerr << "Error reloading board: " << e.what() << std::endl;
        isGameOver = true; // End the game if the board cannot be loaded
        return;
    }

    board.print(noColors, isSilentMode());       // Print the reset board
    createGhost();               // Recreate ghosts on the board
    createHammer();              // Recreate hammer on the board
    createMario();               // Recreate Mario on the board
    player.drawStart(noColors, isSilentMode());  // Draw Mario at the starting position
    createLegend();              // Recreate the legend
}

// Creates all ghosts on the board by scanning for 'x' characters
void Game::createGhost() {
    ghosts.clear(); // Clear existing ghosts

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (board.getChar(x, y) == 'x') { // Check for ghost markers
                Ghost newGhost(&board, x, y, 'x', this, false); // Create a new ghost
                newGhost.setInitialDirection(); // Set its initial direction
                ghosts.push_back(newGhost); // Add it to the ghost list
            }
            else if (board.getChar(x, y) == 'X')
            {
                Ghost newGhost(&board, x, y, 'X', this, true); // Create a new ghost
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
                player.drawStart(noColors, isSilentMode()); // Draw Mario on the board
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
    if (isSilentMode()) return;
    gotoxy(x, y);
    std::cout << "MARIO'S LIFE:"; // Display Mario's life
    player.printLife(LIFE_X, LIFE_Y,isSilentMode()); // Print life details
    gotoxy(x, y + 1);
    std::cout << "SCORE:"; // Display score
    player.printScore(SCORE_X, SCORE_Y, isSilentMode()); // Print score details
}

// Handles collecting the hammer when Mario reaches its position
void Game::collectHammer() {
    if (player.getX() == hammer.getX() && player.getY() == hammer.getY()) { // Check if Mario collects the hammer
        player.hasHammer = true; // Update Mario's hammer state
        hammerOriginalX = hammer.getX(); // Save hammer's original X position
        hammerOriginalY = hammer.getY(); // Save hammer's original Y position
        hammer.setPosition(-1, -1); // Remove hammer from the board
        board.setChar(player.getX(), player.getY(), ' '); // Clear hammer marker
        hammer.printP(isSilentMode()); // Indicate hammer collection
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
        for (int dx = 1; dx <= 2; ++dx) { // Check up to 2 spaces ahead
            checkAndRemoveEntities(marioX + dx, marioY, success);
        }
    }
    else if (direction.x == -1 && direction.y == 0) { // Mario moving left
        for (int dx = -1; dx >= -2; --dx) { // Check up to 2 spaces behind
            checkAndRemoveEntities(marioX + dx, marioY, success);
        }
    }
    else { // Mario is stationary, check all directions
        for (const auto& d : Point::directions) { // Iterate over all possible directions
            for (int i = 1; i <= 2; ++i) { // Check up to 2 spaces in each direction
                checkAndRemoveEntities(marioX + d.x * i, marioY + d.y * i, success);
            }
        }
    }

    // Handle the hammer's status after use
    if (success) {
        hammer.printP(isSilentMode()); // Indicate the hammer is still available
    }
    else {
        player.hasHammer = false; // Remove the hammer from Mario
        hammer.deletetP(isSilentMode()); // Clear the hammer from the board
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
            it->erase(noColors, isSilentMode()); // Erase the ghost from the screen
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
    if (!checkPau() || !checkLegend() || !checkDonkey() || !checkGhost() || !checkInvalidChar() || !canMarioClimb() || !checkLadder() || !checkMario()) {
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
            if ((board.getChar(x, y) == 'x' || board.getChar(x, y) == 'X') && (board.getChar(x, y + 1) != '=' && board.getChar(x, y + 1) != '<' && board.getChar(x, y + 1) != '>')) {
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
                currentChar != 'p' && currentChar != 'H' && currentChar != ' ' && currentChar != 'x' && currentChar != 'X') {
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

bool Game::canMarioClimb() const {
    // Check if there is a ladder ('H') in the 24th row
    for (int x = 0; x < MAX_X; ++x) {
        if (board.getChar(x, 23) == 'H') { // Row 24 is indexed as 23 (0-based indexing)
            return true; // Valid: A ladder exists in the 24th row
        }
    }

    // If no ladder exists in the 24th row
    std::cout << "Invalid board: No ladder ('H') in row 24. Mario cannot climb." << std::endl;
    return false;
}

void Game::reportResultError(const std::string& message, const std::string& filename, size_t iteration) {
    system("cls");
    std::cout << "Screen " << filename << " - " << message << '\n';
    std::cout << "Iteration: " << iteration << '\n';
    std::cout << "Press any key to continue to next screens (if any)" << std::endl;
    _getch();
}

void Game::checkTests(bool flag) {
    system("cls");
    if (flag == true) 
        std::cout << "test passed!" << std::endl;
    else
        std::cout << "test failed! - "<< whyFailed << std::endl;
}