#pragma once
#include "Point.h"
#include "Board.h"

// Forward declaration of the Game class to avoid circular dependencies
class Game;

class Barrels : public Point {
private:
    bool explodeKillMario = false; // Flag to indicate if the barrel will explode and kill Mario
    bool toRemove = false;         // Flag to indicate if the barrel should be removed from the game
    Game* g1;                      // Pointer to the Game instance for accessing game-related data
    int countFall = 0;             // Counter to track the number of consecutive falls of the barrel

public:
    // Constructor: Initializes a Barrels object with the starting position, game instance, 
    // optional symbol, and the board it interacts with
    Barrels(Board* board, int startX, int startY, Game* game, char symbol = 'O')
        : Point(startX, startY, symbol, board), g1(game) {
    }

    // Handles the falling behavior of the barrel, including updating its position 
    // and managing interactions during the fall
    void falling(bool noColors);

    // Moves the barrel in its current direction, handles collisions with the environment, 
    // and determines if the barrel needs to change direction or explode
    void move(bool noColors);

    // Sets the initial movement direction of the barrel when it is spawned, 
    void setInitialDirection(int size);

    // Returns whether the barrel should be removed from the game
    bool shouldRemove() const { return toRemove; }

    // Returns whether the barrel's explosion will kill Mario upon collision
    bool isExplodeAndKillMario() const { return explodeKillMario; }

    // Erases the barrel's current position on the board, ensuring special elements 
    // like the hammer or Donkey Kong remain in place if they overlap with the barrel
    void erase(bool noColors);
};