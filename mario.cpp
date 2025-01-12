﻿#include "Mario.h"
#include "Barrels.h"
#include "Game.h"
#include "Ghost.h"

// Draw Mario at the starting position
void Mario::drawStart(bool noColors) {
    dir = { 0, 0 };
    lastDir = { 0, 0 };
    isAlive = true;
    draw(noColors); // Supports noColors
}

// Handle key presses for movement
void Mario::keyPressed(char key) {
    char thisChar = pBoard->getChar(x,y);
    for (size_t i = 0; i < numKeys; i++) {
        if (std::tolower(key) == keys[i]) {
            if (keys[i] == 'w' && (!isOnGround() && thisChar != 'H')) // Check if Mario is already in the air
                return;
            dir = directions[i];
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

    if (pBoard->getChar(newX, newY) == 'H') {
        x = newX;
        y = newY;
    }
    else if ((nextChar == '>') || (nextChar == '<') || (nextChar == '=')) {
        draw('H', noColors); // Supports noColors
        y = y - 2; // Continue upward movement
        dir = { 0, 0 };
    }
    else {
        jump(noColors); // Supports noColors
        draw(noColors); // Supports noColors
        Sleep(200);
        erase(noColors); // Supports noColors
        falling(noColors); // Supports noColors
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
        if (underNextChar == 'H') { // handle the case Mario trying to get down on the ladder
            y = y + 2; // skip the floor sign
            dir = { 0, 1 }; // continue move down
        }
        else
            dir = { 0, 0 }; // Stop movement
    }
    else {
        x = newX;
        y = newY;
    }
}

// General movement function for Mario
void Mario::move(bool noColors) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    char nextChar = pBoard->getChar(newX, newY);
    char underChar = pBoard->getChar(x, y+1);
    if (dir.y == -1) // press 'w'
        caseUp(noColors); // Supports noColors
    else if (dir.y == -1 && (pBoard->getChar(x, y) == 'H')) // לעלות על הסולם
        caseUp(noColors);
    else if (dir.y == 1) // press 'x'
        caseDown(noColors); // Supports noColors
    else { // press 'a' or 'd' or 's'
        if (nextChar == 'Q') { // facing a wall
            dir = { 0, 0 }; // stop movement
        }
        else if (!isOnGround() && underChar!='H') { // falling between floors
            falling(noColors); // Supports noColors
        }
        else {
            x = newX;
            y = newY;
        }
    }
    lastDir = dir;
}

// Handle Mario's jump
void Mario::jump(bool noColors) {
    int newX = x + lastDir.x;
    int newY = y + (2 * dir.y);
    if (pBoard->getChar(newX, newY) == 'H') {
        x = newX;
        y = newY;
        draw('H', noColors); // Supports noColors
        dir = lastDir;
        return;
    }
    if ((pBoard->getChar(newX, newY) == '=') || (pBoard->getChar(newX, newY) == '<') || (pBoard->getChar(newX, newY) == '>')) {
        x = newX;
        y = y + dir.y;
        draw('=', noColors); // Supports noColors
        dir = lastDir;
        return;
    }
    if (pBoard->getChar(newX, newY) == ' ') {
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
    static int countFall = 0;
    int underNewY = newY + 1;
    char underNextChar = pBoard->getChar(newX, underNewY);
    char nextChar = pBoard->getChar(newX, newY);
    y = newY;
    x = newX;
    if (isOnGround()) {
        dir = lastDir; // Stop falling if obstacle below
        if (countFall >= 5) {
            g1->handleCollision();
            countFall = 0;
            return;
        }
        countFall = 0;
        draw(noColors); // Supports noColors
        erase(noColors); // Supports noColors
    }
    else if ((nextChar == ' ') || (nextChar == 'H')) {
        countFall++;
        y = newY; // Move down
        x = newX;
        draw(noColors); // Supports noColors
        erase(noColors); // Supports noColors
    }
}

// Returns whether Mario is alive
bool Mario::getIsAlive() {
    return life > 0;
}

// Display Mario's remaining lives
void Mario::printLife(int x, int y) const {
    char lifeChar = life + '0';
    gotoxy(x,y);
    std::cout << lifeChar;
}

void Mario::printScore(int x, int y) const {
    char scoreChar = score + '0';
    gotoxy(x,y);
    std::cout << scoreChar;
}

// Check if Mario is colliding with a given barrel
bool Mario::isCollidingBarrel(const Barrels& barrel) const {
    return (getX() == barrel.getX() && getY() == barrel.getY());
}

bool Mario::isCollidingGhost(const Ghost& ghost) const {
    return (getX() == ghost.getX() && getY() == ghost.getY());
}

void Mario::erase(bool noColors) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    char nextChar = pBoard->getChar(newX, newY);

    // קריאה ללוגיקה הבסיסית של Point למחיקת הדמות
    Point::erase(noColors);
}