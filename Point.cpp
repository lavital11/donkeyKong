#include "Point.h"

// Draws the character `c` at the current position, with optional color handling
void Point::draw(char c, bool noColors, bool isSilent) const {
    if (isSilent) return;
    gotoxy(x, y);
    if (!noColors) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        // Always keep Mario white if the character is '@'
        if (c == '@') {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // White
        }
        else {
            // Change console color based on character `c`
            switch (c) {
            case '=': SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); break; // Blue
            case 'H': SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break; // Yellow
            default: SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); break; // White
            }
        }
    }

    std::cout << c;
    if (!noColors) {
        // Reset color to default
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

// Overloaded draw method that calls the other draw method with the current character `ch`
void Point::draw(bool noColors, bool isSilent) const {
    draw(ch, noColors, isSilent);
}

// Erases the current point by drawing a blank space over it based on the current context
void Point::erase(bool noColors, bool isSilent) {
    if (isSilent) return;
    int newX = x + dir.x;
    int newY = y + dir.y;
    char nextChar = pBoard->getChar(newX, newY);
    int underY = y + 1;
    char underChar = pBoard->getChar(x, underY);

    if ((nextChar == 'H') && ((dir.y == -1) || (dir.y == 1))) {
        draw('H', noColors, isSilent);
    }
    else if ((nextChar == ' ') && (underChar == 'H') && ((dir.x == -1) || (dir.x == 1))) {
        draw('H', noColors, isSilent);
    }
    else if (pBoard->getChar(x, y) == 'Q') {
        draw('Q', noColors, isSilent);
    }
    else {
        draw(' ', noColors, isSilent);
    }

    if ((dir.x == 1) || (dir.x == -1)) {
        if (pBoard->getChar(x, y - 1) == 'H') {
            draw('H', noColors, isSilent);
        }
    }

    if (pBoard->getChar(x, y) == '&') {
        draw('&', noColors, isSilent);
    }
}

// Sets the board for this point
void Point::setBoard(Board& board) {
    pBoard = &board;
}

// Sets the position of the point
void Point::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

// Sets the symbol associated with this point
void Point::setSymbol(char symbol) {
    ch = symbol;
}

// Sets the movement direction of the point
void Point::setDirection(int dx, int dy) {
    dir = { dx, dy };
}

// Returns the current direction of the Point
Point::Direction Point::getDirection() const {
    return dir;
}

// Checks if the point is on the ground (i.e., below an obstacle)
bool Point::isOnGround() {
    int underY = y + 1; // The position directly beneath the current position
    char belowChar = pBoard->getChar(x, underY);

    return (belowChar == '=' || belowChar == '<' || belowChar == '>');
}
