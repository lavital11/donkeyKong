#pragma once
#include <iostream>
#include "Board.h"
#include "utils.h"

// Constants
constexpr int DONK_X = 38;
constexpr int DONK_Y = 3;

class Point {
public:
    struct Direction { int x, y; };
    static constexpr Direction directions[] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0}, {0, 0} };

protected:
    int x = 0, y = 0;            // The current position of the point on the board
    Direction dir{ 0, 0 };       // Current direction of movement
    Direction lastDir{ 0, 0 };   // Last direction before falling
    Direction dirBefFall{ 0,0 }; // Direction before falling
    char ch = ' ';               // Character representing the point
    Board* pBoard = nullptr;     // Pointer to the game board

public:
    // Default constructor initializes point at (0, 0) with empty symbol and no board
    Point() : x(0), y(0), ch(' '), pBoard(nullptr) {}

    // Constructor initializing point at specified coordinates with no symbol
    Point(int startX , int startY ) : x(startX), y(startY) {}

    // Constructor initializing point at specified coordinates with specified symbol and board
    Point(int startX, int startY, char symbol, Board* board)
        : x(startX), y(startY), ch(symbol), pBoard(board) {}

    // Returns the current position as a Point object
    Point getPosition() const { return Point(x, y); }

    // Draws the point's character `c` at the current position, with optional color handling
    void draw(char c, bool noColors) const;

    // Overloaded draw method that uses the current character
    void draw(bool noColors) const;

    // Erases the point by drawing a space over it, with optional color handling
    virtual void erase(bool noColors);

    // Sets the board for this point
    void setBoard(Board& board);

    // Sets the position of the point
    void setPosition(int newX, int newY);

    // Sets the symbol associated with this point
    void setSymbol(char symbol);

    // Sets the movement direction of the point
    void setDirection(int dx, int dy);

    Direction getDirection() const;

    // Checks if the point is on the ground (i.e., below an obstacle)
    bool isOnGround();

    // Gets the current x-coordinate
    int getX() const { return x; }

    // Gets the current y-coordinate
    int getY() const { return y; }
};
