#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 // Define the minimum Windows version for compatibility
#endif
#include <windows.h>
#include <string>
#include <vector>

class Board {
    static constexpr int MAX_X = 80; // Maximum width of the board
    static constexpr int MAX_Y = 25; // Maximum height of the board
    char currentBoard[MAX_Y][MAX_X + 1]; // 2D array to represent the board, each row is null-terminated

public:
    // Default constructor: Initializes the board with empty spaces and optional borders
    Board() {
        for (int y = 0; y < MAX_Y; ++y) {
            for (int x = 0; x < MAX_X; ++x) {
                currentBoard[y][x] = ' '; // Fill each cell with a space character
            }
            currentBoard[y][MAX_X] = '\0'; // Null-terminate each row for safety
        }
    }

    // Constructor: Initializes the board by loading its layout from a file
    explicit Board(const std::string& filename);

    // Loads the board layout from a text file
    // Throws a runtime error if the file cannot be opened or is invalid
    void loadBoardFromFile(const std::string& filename);

    // Resets the current board state to its initial configuration by reloading it from the file
    void reset(const std::string& filename);

    // Prints the current state of the board to the console
    // If `noColors` is true, text is displayed without any color formatting
    void print(bool noColors) const;

    // Retrieves the character at a specific position on the board
    // Returns '\0' if the specified position is out of bounds
    char getChar(int x, int y) const;

    // Sets a specific character at a given position on the board
    // Assumes the provided position is within bounds
    void setChar(int x, int y, char c) {
        currentBoard[y][x] = c;
    }
};