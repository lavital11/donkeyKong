#include "AutoGame.h"

AutoGame& AutoGame::operator=(const AutoGame& other)
{
    if (this != &other) {
        Game::operator=(other); // Call base class assignment
        // Copy specific AutoGame members here
        //this->autoMode = other.autoMode;
    }
    return *this;
}

void AutoGame::canRun(const std::vector<std::string>& stepsFiles, const std::vector<std::string>& resultsFiles) {
    for (size_t i = 0; i < stepsFiles.size(); ++i) {
        stepsFilename = stepsFiles[i];
        resultsFilename = resultsFiles[i];

        if (!Steps::isEmptyFile(stepsFilename)) {
            steps = Steps::loadSteps(stepsFilename);
            random_seed = steps.getRandomSeed();
            results = Results::loadResults(resultsFilename);

            // Run the game for the current screen (optional logic here)
            runGameForCurrentScreen(); // Logic for running one screen
        }
        else {
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
}

void AutoGame::runGameForCurrentScreen()
{

}