﻿#include "Ghost.h"

// Moves the ghost based on its current direction and interactions with other ghosts or the board
void Ghost::move(bool noColors, const std::vector<Ghost>& ghosts) {
    erase(noColors); // Erase the ghost from its current position

    // Calculate the next horizontal position and retrieve board characters at and below the new position
    int newX = x + dir.x;
    char nextChar = pBoard->getChar(newX, y);
    char belowNextChar = pBoard->getChar(x + dir.x, y + 1);

    // Handle collisions with other ghosts
    for (const auto& otherGhost : ghosts) {
        if (&otherGhost != this && otherGhost.getX() == newX && otherGhost.getY() == y) {
            dir.x = -dir.x; // Reverse direction on collision
            newX = x + dir.x;
            break;
        }
    }

    // Check for walls or open space below the next position
    if (belowNextChar == ' ' || nextChar == 'Q') {
        dir.x = -dir.x; // Reverse direction if the ghost can't continue moving
        newX = x + dir.x;
        x = newX;
        return;
    }
    else {
        constexpr int ChangeDirChance = 5; // Chance (percentage) for random direction change
        if ((rand() % 100) <= ChangeDirChance) {
            dir.x = -dir.x; // Randomly reverse direction
            newX = x + dir.x;
            x = newX;
        }
        else {
            x = newX; // Move to the next position
        }
    }

    draw(noColors); // Draw the ghost in the new position
    lastDir = dir;  // Update the last movement direction
}

// Sets the initial movement direction of the ghost
void Ghost::setInitialDirection() {
    setDirection((rand() % 2 == 0) ? -1 : 1, 0); // Randomly choose left or right
}

// Reverses the ghost's horizontal direction
void Ghost::changeDir() {
    dir.x = -dir.x;
}

// Erases the ghost from the board
void Ghost::erase(bool noColors) {
    // Check if there's a hammer below the ghost
    if (pBoard->getChar(x, y) == 'p') {
        return; // Do not erase the hammer
    }

    // Call the base logic from Point to erase the ghost
    Point::erase(noColors);
}