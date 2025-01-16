#include "Ghost.h"

void Ghost::move(bool noColors, const std::vector<Ghost>& ghosts) {
    erase(noColors); // מחיקת הרוח מהמיקום הנוכחי
    int newX = x + dir.x; // חישוב המיקום האופקי הבא
    char nextChar = pBoard->getChar(newX, y); // קבלת התו במיקום הבא
    char belowNextChar = pBoard->getChar(x + dir.x, y + 1); // קבלת התו מתחת למיקום הבא

    // טיפול בהתנגשות עם רוחות אחרות
    for (const auto& otherGhost : ghosts) {
        if (&otherGhost != this && otherGhost.getX() == newX && otherGhost.getY() == y) {
            dir.x = -dir.x; // שינוי כיוון במקרה של התנגשות
            newX = x + dir.x;
            break;
        }
    }

    // בדיקה אם יש קיר או שטח פנוי מתחת
    if (belowNextChar == ' ' || nextChar == 'Q') {
        dir.x = -dir.x;
        newX = x + dir.x;
        x = newX;
        return;
    }
    else {
        constexpr int ChangeDirChance = 5;
        if ((rand() % 100) <= ChangeDirChance) {
            dir.x = -dir.x;
            newX = x + dir.x;
            x = newX;
        }
        else {
            x = newX;
        }
    }

    draw(noColors); // צביעת הרוח במקום החדש
    lastDir = dir; // עדכון כיוון התנועה האחרון
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