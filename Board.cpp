﻿#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstring>
#include <iostream>
#include "Board.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

// Constructor initializes the board by loading it from a file
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
    int y = 0;

    while (std::getline(inFile, line) && y < MAX_Y) {
        if (line.length() > MAX_X) {
            throw std::runtime_error("Line in file exceeds MAX_X");
        }

        // Copy line content to the board
        std::strncpy(currentBoard[y], line.c_str(), MAX_X);
        currentBoard[y][MAX_X] = '\0'; // Ensure null termination
        ++y;
    }

    if (y != MAX_Y) {
        throw std::runtime_error("File does not contain the required number of lines");
    }

    inFile.close();
}

// Resets the current board to match its initial state (reload from file)
void Board::reset(const std::string& filename) {
    loadBoardFromFile(filename);
}

// Prints the current state of the board to the console
void Board::print(bool noColors) const {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set cursor position to the top-left corner
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(hConsole, coord);

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char currentChar = currentBoard[y][x];

            // Change color based on the current character, only if noColors is false
            if (!noColors) {
                switch (currentChar) {
                case '=':
                    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Blue
                    break;
                case 'H':
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Yellow
                    break;
                case '<':
                case '>':
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // White
                    break;
                default:
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Default (White)
                    break;
                }
            }

            std::cout << currentChar;
        }

        // Print new line only if it's not the last row
        if (y < MAX_Y - 1) {
            std::cout << '\n';
        }
    }

    // Reset color, only if noColors is false
    if (!noColors) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

// Retrieves the character at a specific position on the board
char Board::getChar(int x, int y) const {
    if (x >= 0 && x < MAX_X && y >= 0 && y < MAX_Y) {
        return currentBoard[y][x];
    }
    return '\0'; // Return null character if out of bounds
}

// Retrieves the length of a row (ignoring trailing spaces)
int Board::getRowLength(int y) const {
    if (y < 0 || y >= MAX_Y) {
        return 0; // Return 0 for invalid rows
    }

    int length = MAX_X;
    while (length > 0 && currentBoard[y][length - 1] == ' ') {
        --length; // Skip trailing spaces
    }
    return length;
}