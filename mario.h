#pragma once
#include "Point.h"
#include "Board.h"


// Constants for life display position
static constexpr int LIFE_X = 16; // X-coordinate for displaying life
static constexpr int LIFE_Y = 1;  // Y-coordinate for displaying life

class Game;    // Forward declaration of the Game class
class Barrels; // Forward declaration of the Barrels class
class Ghost;   // Forward declaration of the Ghost class

// Class representing Mario, derived from the Point class
class Mario : public Point {
private:
    // Movement keys and their count
    static constexpr char keys[] = { 'w', 'a', 'x', 'd', 's' }; // Movement keys
    static constexpr size_t numKeys = sizeof(keys);             // Number of movement keys

    bool isAlive = true; // Mario's life status
    int countFall = 0;   // Counter for falling state
    Game* g1;            // Pointer to the Game object

public:
    int life = 3;        // Mario's starting lives
    int score = 0;       // Mario's score
    bool hasHammer = false; // Indicates if Mario has a hammer

    // Constructor: Initializes Mario's position, board, and initial state
    Mario(Game* g1, int startx, int starty, Board* board)
        : Point(startx, starty, '@', board), g1(g1), life(3), isAlive(true) {
        setSymbol('@'); // Set Mario's symbol
    }

    // Draw Mario at the start of the game
    void drawStart(bool noColors, bool isSilent);

    // Handle key press input for Mario's movement
    void keyPressed(char key);

    // Perform a jump action
    void jump(bool noColors, bool isSilent);

    // Handle Mario's falling state
    void falling(bool noColors, bool isSilent);

    // Get whether Mario is alive
    bool getIsAlive() const;

    // Move Mario based on the current direction
    void move(bool noColors,bool isSilent);

    // Display Mario's remaining life
    void printLife(int x, int y, bool isSilent) const;

    // Display Mario's score
    void printScore(int x, int y, bool isSilent) const;

    // Handle Mario's movement upward
    void caseUp(bool noColors, bool isSilent);

    // Handle Mario's movement downward
    void caseDown(bool noColors);

    // Lose a life and update the display
    void loseLife(bool isSilent) {
        life -= 1;
        printLife(LIFE_X, LIFE_Y,isSilent); // Update life display
    }

    // Check if Mario is colliding with a barrel
    bool isCollidingBarrel(const Barrels& barrel) const;

    // Check if Mario is colliding with a ghost
    bool isCollidingGhost(const Ghost& ghost) const;

    // Erase Mario from the board
    void erase(bool noColors, bool isSilent);
};