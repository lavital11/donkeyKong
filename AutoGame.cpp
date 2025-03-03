﻿#include "AutoGame.h"
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
using namespace std::chrono;

// בנאי
AutoGame::AutoGame(bool silentMode, const std::vector<std::string>& stepsFiles, const std::vector<std::string>& resultsFiles)
    : Game(), isSilent(silentMode), stepsFiles(stepsFiles), resultsFiles(resultsFiles) {
}

// אופרטור השמה
AutoGame& AutoGame::operator=(const AutoGame& other) {
    if (this != &other) {
        Game::operator=(other); // קריאה לאופרטור ההשמה של מחלקת הבסיס
        isSilent = other.isSilent;
        stepsFiles = other.stepsFiles;
        resultsFiles = other.resultsFiles;
    }
    return *this;
}

// הפעלת המשחק האוטומטי
void AutoGame::startGame() {
    loadBoardFiles("boards/"); // Load all board files from the "boards" directory
    for (size_t i = 0; i <= stepsFiles.size(); ++i) {

        if (readFiles(i)) {
            random_seed = steps.getRandomSeed();             // הגדרת seed אקראי
            srand(random_seed);
            runGame(); // הרצת המסך הנוכחי
            break;
       }
        else if (!isSilent) {
            std::cout << "Screen " << i + 1 << " cannot be played. ";

            if (i == stepsFiles.size() - 1) {
                std::cout << "This is the last screen. Press any key to exit..." << std::endl;
                std::cin.ignore();
                std::cin.get();
                return;
            }
            else {
                std::cout << "Press any key to skip to the next screen..." << std::endl;
                std::cin.ignore();
                std::cin.get();
            }
        }
    }
    if(isSilentMode())
         checkTests(flagTest);

}

bool AutoGame::readFiles(int i) {
    stepsFilename = stepsFiles[i];
    resultsFilename = i < resultsFiles.size() ? resultsFiles[i] : "";
    if (!Steps::isEmptyFile(stepsFilename)) { // בדיקה אם קובץ הצעדים ריק
        currentBoardIndex = i;
        steps = Steps::loadSteps(stepsFilename);         // טעינת קובץ הצעדים
        
        results = Results::loadResults(resultsFilename); // טעינת קובץ התוצאות (אם קיים)
        return true;
    }
    else
        return false;
}


void AutoGame::inputAction() {
    if (steps.isNextStepOnIteration(gameTime)) {
        char action = steps.popStep();
        if (action == 'p' || action == 'P') useHammer();

        player.keyPressed(action);
    }
}