#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 // Define Windows version for compatibility
#endif
#include <windows.h>
#include <string>
#include <vector>
class Board
{
    static constexpr int MAX_X = 80; // Maximum width of the board
    static constexpr int MAX_Y = 25; // Maximum height of the board
    char currentBoard[MAX_Y][MAX_X + 1];
public:

    Board() {
        // Initialize the board to empty spaces with borders (optional customization)
        for (int y = 0; y < MAX_Y; ++y) {
            for (int x = 0; x < MAX_X; ++x) {
                currentBoard[y][x] = ' '; // Fill with spaces
            }
            currentBoard[y][MAX_X] = '\0'; // Null-terminate each row
        }
    }

    // Constructor initializes the board by loading it from a file
    explicit Board(const std::string& filename);

    // Loads the board layout from a text file
    void loadBoardFromFile(const std::string& filename);

    // Resets the current board to match its initial state (reload from file)
    void reset(const std::string& filename);

    // Prints the current state of the board to the console
    void print(bool noColors) const;

    // Retrieves the character at a specific position on the board
    char getChar(int x, int y) const;

    void setChar(int x, int y, char c) {
        currentBoard[y][x] = c;
    }

    // Retrieves the length of a row (ignoring trailing spaces)
    int getRowLength(int y) const;
};
