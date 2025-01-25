#include <cstdlib> // For srand and rand
#include <ctime>   // For time
#include <iostream>
#include "game.h"  // Include the Game class header
#include "AutoGame.h"
#include "ManuGame.h"


int main(int argc, char** argv) {
    bool isLoad = argc > 1 && std::string(argv[1]) == "-load";   // טעינת קבצים
    bool isSave = argc > 1 && std::string(argv[1]) == "-save";   // משחק רגיל ושמירה
    bool isSilent = isLoad && argc > 2 && std::string(argv[2]) == "-silent"; // מצב שקט בטעינה

    // Initialize the random number generator with the current time
    srand(static_cast<unsigned int>(time(0)));

    // Create a Game object pointer
    Game* g1 = nullptr;

    if (isLoad)
        g1 = new AutoGame(isSilent);
    else if (isSave)
        g1 = new ManuGame(true); // משחק רגיל ושמירה לקבצים
    else
        g1 = new ManuGame(false); // משחק ידני בלבד

    // Start the game
    g1->startGame();

    // Clean up memory
    delete g1;

    // Exit the program
    return 0;
}