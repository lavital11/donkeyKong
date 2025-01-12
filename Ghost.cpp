#include "Ghost.h"

void Ghost::move(bool noColors) {
    erase(noColors); // Erase the barrel from its current position with noColors support
    int newX = x + dir.x; // Calculate new horizontal position
    char nextChar = pBoard->getChar(newX, y); // Character at the next position
    char belowNextChar = pBoard->getChar(x+dir.x, y + 1);  // Character directly below the barrel

    // If there's space below or face a wall, changing direction
    if (belowNextChar == ' ' || nextChar == 'Q') {
        dir.x = -dir.x;
        newX = x + dir.x;
        x = newX;
        return;
    }
    else {
        constexpr int ChangeDirChance = 5; // 5% chance to change direction
        if ((rand() % 100) <= ChangeDirChance) {
            dir.x = -dir.x;
            newX = x + dir.x;
            x = newX;
        }
        else
            x = newX;
    }
    draw(noColors); // Draw the barrel at the new position with optional color support
    lastDir = dir; // Update the last movement direction   
}

void Ghost::setInitialDirection() {
    setDirection((rand() % 2 == 0) ? -1 : 1, 0);
}
void Ghost::changeDir() {
    dir.x = -dir.x;
}

void Ghost::erase(bool noColors) {
    // בדוק אם יש פטיש מתחת לרוח
    if (pBoard->getChar(x, y) == 'p') {
        return; // אל תמחק את הפטיש
    }

    // קריאה ללוגיקה הבסיסית של Point למחיקת הרוח
    Point::erase(noColors);
}