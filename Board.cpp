#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstring>
#include <iostream>
#include "Board.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

// Constructor: Initializes the board by loading its layout from a file
Board::Board(const std::string& filename) {
    loadBoardFromFile(filename);
}

// Loads the board layout from a text file
void Board::loadBoardFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        // If the file cannot be opened, throw an error
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    int y = 0; // Tracks the current row being processed

    // Read each line of the file until MAX_Y rows are reached
    while (std::getline(inFile, line) && y < MAX_Y) {
        if (line.length() > MAX_X) {
            // If the line exceeds the maximum width, throw an error
            throw std::runtime_error("Line in file exceeds MAX_X");
        }

        // Copy the line content into the board array
        std::strncpy(currentBoard[y], line.c_str(), MAX_X);
        currentBoard[y][MAX_X] = '\0'; // Ensure null termination for each row
        ++y;
    }

    // Verify the file contains the required number of rows
    if (y != MAX_Y) {
        throw std::runtime_error("File does not contain the required number of lines");
    }

    inFile.close(); // Close the file
}

// Resets the current board state to its initial configuration by reloading from the file
void Board::reset(const std::string& filename) {
    loadBoardFromFile(filename);
}

// Prints the current state of the board to the console
void Board::print(bool noColors) const {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Get console handle for coloring

    // Set the cursor position to the top-left corner of the console
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(hConsole, coord);

    // Iterate over each row and column of the board
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char currentChar = currentBoard[y][x];

            // Change console text color based on the character, if noColors is false
            if (!noColors) {
                switch (currentChar) {
                case '=': // Blue for '='
                    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                    break;
                case 'H': // Yellow for 'H'
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    break;
                case '<':
                case '>': // White for '<' and '>'
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    break;
                default: // Default (white) for other characters
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    break;
                }
            }

            std::cout << currentChar; // Print the current character
        }

        // Print a newline after each row except the last one
        if (y < MAX_Y - 1) {
            std::cout << '\n';
        }
    }

    // Reset console text color to default, if noColors is false
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

// Retrieves the character at a specified position on the board
char Board::getChar(int x, int y) const {
    // Ensure the position is within board boundaries
    if (x >= 0 && x < MAX_X && y >= 0 && y < MAX_Y) {
        return currentBoard[y][x];
    }
    return '\0'; // Return null character for out-of-bounds positions
}