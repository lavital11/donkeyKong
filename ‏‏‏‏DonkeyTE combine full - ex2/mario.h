#pragma once
#include "Point.h"
#include "Board.h"

// Constants
#define START_X 76
#define START_Y 23

class Game;
class Barrels;
class Ghost;

class Mario : public Point {
private:
    static constexpr char keys[] = { 'w', 'a', 'x', 'd', 's' }; // Movement keys
    static constexpr size_t numKeys = sizeof(keys); // Number of movement keys
    bool isAlive = true; // Mario's life status
    Game* g1;

public:
    int life = 3; // Mario's starting lives

    // Constructor
    Mario(Game* g1)
        : Point(START_X, START_Y, '@', nullptr), g1(g1), life(3), isAlive(true) {
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
    void printLife() const;

    // Handle Mario's movement upward
    void caseUp(bool noColors);

    // Handle Mario's movement downward
    void caseDown(bool noColors);

    // Lose a life
    void loseLife() {
        life -= 1;
        printLife();
    }

    // Check collision with a barrel
    bool isCollidingBarrel(const Barrels& barrel) const;

    bool isCollidingGhost(const Ghost& ghost) const;

};
