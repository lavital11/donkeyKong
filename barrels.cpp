#include "Barrels.h"
#include "Game.h"

// Constant for the bottom line on the game board
constexpr int BOTTOMLINE = 23;

// Causes the barrel to fall, with optional color control
void Barrels::falling(bool noColors) {
    int newY = y + 1; // Move to the position directly below
    char belowChar = pBoard->getChar(x, newY); // Character at the position below

    // Only move down if the space below is not occupied by solid characters
    if (belowChar != '=' && belowChar != '<' && belowChar != '>') {
        erase(noColors); // Erase the barrel from its current position with noColors support
        y = newY;        // Update the position to below
        countFall++;     // Increment the fall step counter
        draw(noColors);  // Draw the barrel at the new position with noColors support
        Sleep(30);       // Slow down the fall display
    }
}

// Moves the barrel according to its direction, with optional color control
void Barrels::move(bool noColors) {
    erase(noColors); // Erase the barrel from its current position with noColors support
    int newX = x + dir.x; // Calculate new horizontal position
    int newY = y + dir.y; // Calculate new vertical position
    char nextChar = pBoard->getChar(newX, newY); // Character at the next position
    char belowChar = pBoard->getChar(x, y + 1);  // Character directly below the barrel

    // If there's space below, the barrel falls
    if (belowChar == ' ') {
        x = newX;
        y = newY;
        dirBefFall = dir;
        falling(noColors); // Barrel falls with optional color control
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
void Barrels::setInitialDirection() {
    setDirection((rand() % 2 == 0) ? -1 : 1, 0);
}

void Barrels::erase(bool noColors) {
    // בדוק אם יש פטיש מתחת לחבית
    if (pBoard->getChar(x, y) == 'p') {
        draw('p', noColors); // השאר את הפטיש במקום
        return;
    }

    // קריאה ללוגיקה הבסיסית של Point למחיקת החבית
    Point::erase(noColors);
}