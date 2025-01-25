#include "Ghost.h"

// Moves the ghost based on its current direction and interactions with other ghosts or the board
void Ghost::move(bool noColors, const std::vector<Ghost>& ghosts) {
    erase(noColors); // Erase the ghost from its current position

    // Calculate the next horizontal position and retrieve board characters at and below the new position
    int newX = x + dir.x;
    int newY = y + dir.y;
    char nextChar = pBoard->getChar(newX, y);
    char belowNextChar = pBoard->getChar(x + dir.x, y + 1);
    char underFloor = pBoard->getChar(x, y + 2);
    char belowChar = pBoard->getChar(x, y + 1);

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
    }
    else if (pBoard->getChar(x, y) == 'H' && dir.y == 0 && canClimb == true)
    {
        constexpr int chooseToClimb = 20; // Chance (percentage) for random direction change
        if ((rand() % 100) <= chooseToClimb)
        {
            climbLadder(noColors);
        }
        else
            x = newX;
    }
    else if (nextChar == 'H' && dir.y == -1 && canClimb == true)
    {
        climbLadder(noColors);
    }
    else if (underFloor == 'H' && dir.y == 0 && canClimb == true)
    {
        constexpr int chooseToClimb = 20; // Chance (percentage) for random direction change
        if ((rand() % 100) <= chooseToClimb) {
            y = y + 2;
            downOnLadder(noColors);
        }
        else
            x = newX;
    }
    
    else if (belowChar == 'H' && dir.y == 1 && canClimb == true)
    {
        downOnLadder(noColors);
    }
    else if ((belowChar == '>' || belowChar == '<' || belowChar == '=') && dir.y == 1 && canClimb == true)
    {
        dir = { 1,0 };
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

void Ghost::climbLadder(bool noColors)
{
    dir = { 0,-1 };
    int newX = x;
    int newY = dir.y + y;
    char nextChar = pBoard->getChar(x, newY);
    if ((nextChar == '>') || (nextChar == '<') || (nextChar == '=')) { // Moving past ladder symbols
        draw('H', noColors);
        dir = { 1,0 };
        y = y - 2; // Continue upward movement
    }
    else
    {
        x = newX;
        y = newY;
    }
}

void Ghost::downOnLadder(bool noColors)
{
    dir = { 0, 1 };
    int newX = x;
    int newY = y + dir.y;

    x = newX;
    y = newY;
}