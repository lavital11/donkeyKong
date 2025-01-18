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
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    int y = 0; // Tracks the current row being processed in the board

    // Read each line from the file up to the maximum number of rows (25)
    while (std::getline(inFile, line) && y < MAX_Y) {
        if (line.size() > static_cast<std::string::size_type>(MAX_X)) {
            // Truncate the line to MAX_X characters
            line = line.substr(0, static_cast<std::string::size_type>(MAX_X));
            if (line[static_cast<std::string::size_type>(MAX_X - 1)] != 'Q') {
                line[static_cast<std::string::size_type>(MAX_X - 1)] = 'Q';
            }
        }

        if (line.size() < static_cast<std::string::size_type>(MAX_X)) {
            // Pad the line with spaces
            line.append(static_cast<std::string::size_type>(MAX_X - line.size()), ' ');
            line[static_cast<std::string::size_type>(MAX_X - 1)] = 'Q';
        }

        // Copy the line to the board array
        std::copy(line.begin(), line.end(), currentBoard[y]);
        currentBoard[y][MAX_X] = '\0';
        ++y;
    }

    // Ignore extra lines beyond MAX_Y
    while (std::getline(inFile, line)) {
        // Skip extra lines
    }

    // Validate or fix the 25th row (if it exists)
    if (y == MAX_Y) {
        // Check and fix the 25th row
        currentBoard[MAX_Y - 1][0] = 'Q'; // Ensure the first character is Q
        currentBoard[MAX_Y - 1][MAX_X - 1] = 'Q'; // Ensure the last character is Q

        // Replace invalid characters between the first and last 'Q'
        for (int i = 1; i < MAX_X - 1; ++i) {
            char c = currentBoard[MAX_Y - 1][i];
            if (c != '=' && c != '<' && c != '>') {
                currentBoard[MAX_Y - 1][i] = '='; // Replace invalid characters with '='
            }
        }
    }

    // Fill the remaining rows if fewer than MAX_Y
    while (y < MAX_Y) {
        std::string emptyRow(static_cast<std::string::size_type>(MAX_X), '=');
        emptyRow[0] = 'Q';
        emptyRow[static_cast<std::string::size_type>(MAX_X - 1)] = 'Q';

        std::copy(emptyRow.begin(), emptyRow.end(), currentBoard[y]);
        currentBoard[y][MAX_X] = '\0';
        ++y;
    }

    inFile.close();
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