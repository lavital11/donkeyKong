#pragma once
#include "Point.h"
#include "Board.h"

// Constants
static constexpr int LIFE_X = 16; // Maximum width of the board
static constexpr int LIFE_Y = 1; // Maximum height of the board

class Game;
class Barrels;
class Ghost;

class Mario : public Point {
private:
    static constexpr char keys[] = { 'w', 'a', 'x', 'd', 's' }; // Movement keys
    static constexpr size_t numKeys = sizeof(keys); // Number of movement keys
    bool isAlive = true; // Mario's life status
    int countFall = 0;
    Game* g1;

public:
    int life = 3; // Mario's starting lives
    int score = 0;
    bool hasHammer = false;

    // Constructor
    Mario(Game* g1, int startx, int starty, Board* board)
        : Point(startx, starty, '@', board), g1(g1), life(3), isAlive(true) {
        setSymbol('@');
    }

    // Game initialization
    void drawStart(bool noColors);

    // Handle key press
    void keyPressed(char key);

    // Mario's jump action
    void jump(bool noColors);

    // Manage falling state
    void falling(bool noColors);

    // Get whether Mario is alive
    bool getIsAlive();

    // Move Mario based on input
    void move(bool noColors);

    // Display Mario's remaining life
    void printLife(int x, int y) const;

    void printScore(int x, int y) const;

    // Handle Mario's movement upward
    void caseUp(bool noColors);

    // Handle Mario's movement downward
    void caseDown(bool noColors);

    // Lose a life
    void loseLife() {
        life -= 1;
        printLife(LIFE_X, LIFE_Y);
    }

    // Check collision with a barrel
    bool isCollidingBarrel(const Barrels& barrel) const;

    bool isCollidingGhost(const Ghost& ghost) const;

    void erase(bool noColors);
};
