#pragma once
#include "Point.h"
#include "Board.h"

// Forward declaration of the Game class
class Game;

class Barrels : public Point {
private:
    bool explodeKillMario = false; // Flag to determine if the barrel will explode and kill Mario
    bool toRemove = false;          // Flag indicating if the barrel should be removed
    Game* g1;                      // Pointer to the game instance
    int countFall = 0;              // Counter for how many times the barrel has fallen

public:
    // Constructor to initialize a Barrels object with starting position, game instance, and optional symbol
    Barrels(Board* board, int startX, int startY, Game* game, char symbol = 'O')
        : Point(startX, startY, symbol, board), g1(game) {
    }

    // Manages the falling behavior of the barrel
    void falling(bool noColors);

    // Moves the barrel according to its current direction
    void move(bool noColors);

    // Sets the initial direction of the barrel when it's spawned
    void setInitialDirection();

    // Checks if the barrel should be removed from the game
    bool shouldRemove() const { return toRemove; }

    // Checks if the barrel will explode and kill Mario upon collision
    bool isExplodeAndKillMario() const { return explodeKillMario; }

    void erase(bool noColors);
};
