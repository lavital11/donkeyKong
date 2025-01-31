#include "Barrels.h"
#include "Game.h"

// Constant representing the bottom line of the game board
constexpr int BOTTOMLINE = 23;

// Handles the barrel's falling behavior with optional color control
void Barrels::falling(bool noColors) {
    int newY = y + 1; // Calculate the position directly below
    char belowChar = pBoard->getChar(x, newY); // Get the character at the new position below

    // Move the barrel down only if the space below is not occupied by solid characters
    if (belowChar != '=' && belowChar != '<' && belowChar != '>') {
        erase(noColors); // Erase the barrel's current position
        y = newY;        // Update the vertical position
        countFall++;     // Increment the fall counter
        draw(noColors);  // Draw the barrel at the new position
        Sleep(30);       // Add a delay to control the fall speed
    }
}

// Handles the barrel's movement logic and interactions with the environment
void Barrels::move(bool noColors) {
    erase(noColors); // Erase the barrel's current position
    int newX = x + dir.x; // Calculate the new horizontal position
    int newY = y + dir.y; // Calculate the new vertical position
    char nextChar = pBoard->getChar(newX, newY); // Get the character at the new position
    char belowChar = pBoard->getChar(x, y + 1);  // Get the character directly below the barrel

    // Check if there's empty space below for the barrel to fall
    if (belowChar == ' ') {
        x = newX; // Update horizontal position
        y = newY; // Update vertical position
        dirBefFall = dir; // Save the direction before falling
        falling(noColors); // Trigger falling logic
        return;
    }
    else {
        // If the barrel has fallen for 8 steps, it explodes
        if (countFall >= 8) {
            Point marioPos = g1->getMarioPosition();
            int distanceX = abs(marioPos.getX() - x);
            int distanceY = abs(marioPos.getY() - y);

            // If Mario is nearby, explosion should kill Mario
            if (distanceX <= 2 && distanceY <= 2) {
                explodeKillMario = true;
            }
            toRemove = true; // Mark the barrel for removal
            return;
        }
        else {
            // Check if the barrel reaches a special ground ('<' or '>')
            if (belowChar == '<' || belowChar == '>') {
                setDirection((belowChar == '<') ? -1 : 1, 0);
                newX = x + dir.x;
                newY = y + dir.y;
            }
            x = newX;
            y = newY;
            countFall = 0; // Reset the fall counter
        }
    }

    // Check if the barrel hits a wall at the next position and it's at the bottom line
    if (nextChar == 'Q' && y == BOTTOMLINE) {
        toRemove = true; // Mark the barrel for removal
        return;
    }
    else if (nextChar == 'Q') {
        dir = { 0, 0 }; // Stop the barrel if it hits a wall
        draw(noColors); // Draw the barrel with optional color support
        return;
    }

    // Update the horizontal position if there's no obstacle
    if (nextChar != 'Q') {
        x = newX;
        y = newY;
    }

    draw(noColors); // Draw the barrel at the new position with optional color support
    lastDir = dir; // Update the last movement direction
}

// Sets the initial movement direction for the barrel
void Barrels::setInitialDirection(int size) {
    setDirection(size % 2 == 0 ? -1 : 1, 0);
}

// Erases the barrel's current position, handling special cases
void Barrels::erase(bool noColors) {
    // Keep the hammer in place if the barrel is over it
    if (pBoard->getChar(x, y) == 'p') {
        draw('p', noColors); // Draw the hammer at the same position
        return;
    }

    // Keep Donkey Kong in place if the barrel is over him
    if (pBoard->getChar(x, y) == '&') {
        draw('&', noColors); // Draw Donkey Kong at the same position
        return;
    }

    // Use base Point logic for erasing the barrel
    Point::erase(noColors);
}