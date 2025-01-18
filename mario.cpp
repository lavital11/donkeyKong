#include "Mario.h"
#include "Barrels.h"
#include "Game.h"
#include "Ghost.h"

// Draw Mario at the starting position
void Mario::drawStart(bool noColors) {
    dir = { 0, 0 };          // Initialize direction to stationary
    lastDir = { 0, 0 };      // Initialize the last direction
    isAlive = true;          // Set Mario as alive
    draw(noColors);          // Draw Mario (supports noColors)
}

// Handle key presses for movement
void Mario::keyPressed(char key) {
    char thisChar = pBoard->getChar(x, y); // Get the current character at Mario's position
    for (size_t i = 0; i < numKeys; i++) { // Iterate through all movement keys
        if (std::tolower(key) == keys[i]) {
            if (keys[i] == 'w' && (!isOnGround() && thisChar != 'H')) // Prevent jumping in the air
                return;
            dir = directions[i]; // Update direction based on the key pressed
            return;
        }
    }
}

// Move Mario up
void Mario::caseUp(bool noColors) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    int aboveNewY = newY - 1;
    char aboveNextChar = pBoard->getChar(newX, aboveNewY);
    char nextChar = pBoard->getChar(newX, newY);

    if (pBoard->getChar(newX, newY) == 'H') { // Moving on a ladder
        x = newX;
        y = newY;
    }
    else if ((nextChar == '>') || (nextChar == '<') || (nextChar == '=')) { // Moving past ladder symbols
        draw('H', noColors);
        y = y - 2; // Continue upward movement
        dir = { 0, 0 };
    }
    else {
        jump(noColors); // Handle jumping
        draw(noColors);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Pause for animation
        erase(noColors);
        falling(noColors); // Handle falling if needed
    }
}

// Move Mario down
void Mario::caseDown(bool noColors) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    int underNewY = newY + 1;
    char nextChar = pBoard->getChar(newX, newY);
    char underNextChar = pBoard->getChar(newX, underNewY);

    if (nextChar == '>' || nextChar == '<' || nextChar == '=') {
        if (underNextChar == 'H') { // Handle moving down a ladder
            y = y + 2; // Skip the floor sign
            dir = { 0, 1 }; // Continue moving down
        }
        else {
            dir = { 0, 0 }; // Stop movement
        }
    }
    else {
        x = newX;
        y = newY; // Move normally
    }
}

// General movement function for Mario
void Mario::move(bool noColors) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    char nextChar = pBoard->getChar(newX, newY);
    char underChar = pBoard->getChar(x, y + 1);

    if (dir.y == -1) // If 'w' is pressed
        caseUp(noColors);
    else if (dir.y == -1 && (pBoard->getChar(x, y) == 'H')) // Climbing up a ladder
        caseUp(noColors);
    else if (dir.y == 1) // If 'x' is pressed
        caseDown(noColors);
    else { // If 'a', 'd', or 's' is pressed
        if (nextChar == 'Q') { // Wall collision
            dir = { 0, 0 }; // Stop movement
        }
        else if (!isOnGround() && underChar != 'H') { // Falling between floors
            falling(noColors);
        }
        else {
            x = newX;
            y = newY; // Move to new position
        }
    }
    lastDir = dir; // Update last movement direction
}

// Handle Mario's jump
void Mario::jump(bool noColors) {
    int newX = x + lastDir.x;
    int newY = y + (2 * dir.y);

    if (pBoard->getChar(newX, y) == 'Q') { // Wall collision during jump
        dir = { 0, 0 };
        falling(noColors); // Start falling
        return;
    }

    if (pBoard->getChar(newX, newY) == 'H') { // Land on a ladder
        x = newX;
        y = newY;
        draw('H', noColors);
        dir = lastDir;
        return;
    }
    if ((pBoard->getChar(newX, newY) == '=') || (pBoard->getChar(newX, newY) == '<') || (pBoard->getChar(newX, newY) == '>')) {
        x = newX;
        y = y + dir.y;
        draw('=', noColors);
        dir = lastDir;
        return;
    }
    if (pBoard->getChar(newX, newY) == ' ') { // Continue moving in space
        x = newX;
        y = newY;
        dir = lastDir;
        return;
    }
}

// Handle Mario's falling
void Mario::falling(bool noColors) {
    int newX = x + lastDir.x;
    int newY = y + 1;
    int underNewY = newY + 1;

    if (newX < 0 || newX >= MAX_X || newY < 0 || newY >= MAX_Y - 1) { // Check board boundaries
        dir = { 0, 0 }; // Stop movement
        return;
    }

    for (int stepY = y + 1; stepY <= newY; ++stepY) {
        char stepChar = pBoard->getChar(newX, stepY);
        if (stepChar == 'Q') { // Wall collision
            dir = { 0, 0 };
            return;
        }
    }

    char underNextChar = pBoard->getChar(newX, underNewY);
    char nextChar = pBoard->getChar(newX, newY);
    y = newY;
    x = newX;

    if (isOnGround()) { // If Mario lands on the ground
        dir = lastDir;
        if (countFall >= 5) { // Collision handling after a long fall
            g1->handleCollision();
            countFall = 0;
            return;
        }
        countFall = 0;
        draw(noColors);
        erase(noColors);
    }
    else if ((nextChar == ' ') || (nextChar == 'H')) { // Continue falling
        countFall++;
        y = newY;
        x = newX;
        draw(noColors);
        erase(noColors);
    }
}

// Returns whether Mario is alive
bool Mario::getIsAlive() const {
    return life > 0;
}

// Display Mario's remaining lives
void Mario::printLife(int x, int y) const {
    char lifeChar = life + '0'; // Convert life count to character
    gotoxy(x, y);
    std::cout << lifeChar;
}

// Display Mario's score
void Mario::printScore(int x, int y) const {
    gotoxy(x, y);
    std::cout << score;
}

// Check if Mario is colliding with a barrel
bool Mario::isCollidingBarrel(const Barrels& barrel) const {
    return (getX() == barrel.getX() && getY() == barrel.getY());
}

// Check if Mario is colliding with a ghost
bool Mario::isCollidingGhost(const Ghost& ghost) const {
    return (getX() == ghost.getX() && getY() == ghost.getY());
}

// Erase Mario from the board
void Mario::erase(bool noColors) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    char nextChar = pBoard->getChar(newX, newY);

    Point::erase(noColors); // Call the base class logic for erasing
}