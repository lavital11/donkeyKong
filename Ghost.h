#pragma once
#include "Point.h"

class Game; // Forward declaration of the Game class

// Class representing a Ghost, which is derived from the Point class
class Ghost : public Point {
private:
    Game* g1;                     // Pointer to the Game instance
    bool toRemove = false;        // Flag indicating if the ghost should be removed
    bool canClimb;


public:
    // Constructor: Initializes the ghost with its starting position, game reference, and symbol
    Ghost(Board* board, int startX, int startY,char symbol, Game* game, bool canClimb)
        : Point(startX, startY,symbol, board), g1(game), canClimb(canClimb) {
    }

    // Moves the ghost on the board, taking into account collisions and game logic
    void move(bool noColors, const std::vector<Ghost>& ghosts);
    // Sets the initial movement direction of the ghost
    void setInitialDirection();

    // Checks if the ghost is marked for removal
    bool shouldRemove() const { return toRemove; }

    // Changes the ghost's movement direction
    void changeDir();

    // Erases the ghost from its current position on the board
    void erase(bool noColors);
    // 
    void climbLadder(bool noColors);

    void downOnLadder(bool noColors);
};