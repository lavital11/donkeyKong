#include "ManuGame.h"

ManuGame& ManuGame::operator=(const ManuGame& other)
{
    if (this != &other) {
        Game::operator=(other); // Call base class assignment
        // Copy specific ManuGame members here
        //this->manualMode = other.manualMode;
    }
    return *this;
}