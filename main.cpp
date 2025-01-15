#include <cstdlib> // For srand and rand
#include <ctime>   // For time
#include "game.h"

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Initialize random number generator
    Game g1;
    g1.startGame(); // Start the game
    return 0;
}
