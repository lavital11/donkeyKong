#include <cstdlib> // For srand and rand
#include <ctime>   // For time
#include "game.h"  // Include the Game class header

int main() {
    // Initialize the random number generator with the current time
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create a Game object
    Game g1;

    // Start the game
    g1.startGame();

    // Exit the program
    return 0;
}