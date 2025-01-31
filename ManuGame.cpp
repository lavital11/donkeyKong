#include "ManuGame.h"

namespace fs = std::filesystem;
using namespace std::chrono;

ManuGame& ManuGame::operator=(const ManuGame& other)
{
    if (this != &other) {
        Game::operator=(other); // Call base class assignment
        // Copy specific ManuGame members here
        //this->manualMode = other.manualMode;
    }
    return *this;
}

void ManuGame::showInstructions() {
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

void ManuGame::startGame() {
    loadBoardFiles("boards/"); // Load all board files from the "boards" directory
    int choice = 0; // Variable for storing user menu choice
    random_seed = static_cast<long>(time(0));  // יצירת seed אחיד למשחק
    srand(random_seed);  // אתחול מחולל מספרים רנדומיים עם אותו seed
    steps.setRandomSeed(random_seed);  // שמירת seed בקובץ הצעדים
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
            currentBoardIndex = boardChoice - 1;
            if (boardChoice >= 1 && boardChoice <= static_cast<int>(boardFiles.size())) {
                loadBoardByIndex(boardChoice - 1); // Load the selected board
                Sleep(2000); // Pause for 2 seconds to show the loaded board
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


// Checks if Mario has reached the level's finishing point

// Handles the logic for when Mario collides with an obstacle

void ManuGame::inputAction() {

    if (_kbhit()) {
        char key = _getch();

        if (key == ESC) pauseGame();
        else if (key == 'p' || key == 'P') useHammer();
        else {
            player.keyPressed(key);
            if (getIsSave()) {
                steps.addStep(gameTime, key); // שמירת ההפרש מההתחלה במקום זמן מוחלט
            }
        }
    }
}

void ManuGame::saveStepAfterFinish() {
    steps.saveSteps(stepsFiles[currentBoardIndex]);
    steps.clear();
    gameTime = 0;
}
void ManuGame::saveResultsAfterFinish() {
    results.saveResults(resultsFiles[currentBoardIndex], player.score);
    results.clear();
}

