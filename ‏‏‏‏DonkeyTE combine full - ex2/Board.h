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
    // Original design of the board, stored as a 2D character array
   /* const char* originalBoard[MAX_Y] = {
            //          1         2         3         4         5         6         7
            //01234567890123456789012345678901234567890123456789012345678901234567890123456789 
             "Q                                     $                                        Q", //0
             "Q  MARIO'S LIFE:   =============================================               Q", //1
             "Q                      H                               H                       Q", //2
             "Q                      H              &                H                       Q", //3
             "Q                     <<<<<<<<<<<<<<<<=>>>>>>>>>>>>>>> H                       Q", //4
             "Q                        H                             H                       Q", //5
             "Q                        H                             <<<<<<<<<<<<<           Q", //6
             "Q         x              H                                H                    Q", //7
             "Q=======>>>>===============                               H                    Q", //8
             "Q            H         H                                  H                    Q", //9
             "Q            H         H                                  H                    Q", //10
             "Q            H     ===================<<<<         =========                   Q", //11
             "Q            H                                         H                       Q", //12
             "Q            H                                         H                       Q", //13
             "Q    >>>>>>>>>>>>>>>>>>>>>                          =====================>>>   Q", //14 
             "Q                       H                                         H            Q", //15
             "Q         x             H                        x                H            Q", //16
             "Q        ======================              ==========================        Q", //17
             "Q                  H                              H                            Q", //18
             "Q                  H                              H                            Q", //19
             "Q              =========================================                       Q", //20
             "Q                                  H                                           Q", //21
             "Q                                  H                                           Q", //22
             "Q         x                        H             x                             Q", //23                                                
             "Q=============================================================<<<<=============Q"  //24
        };*/

public:

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
    /*
    // Constructor initializes the board by copying the originalBoard
    Board() {
        reset();
    }

    // Resets the current board to match the original layout
    void reset();

    // Prints the current state of the board to the console
    void print(bool noColors) const;

    // Retrieves the character at a specific position on the board
    char getChar(int x, int y) const {
        return currentBoard[y][x];
    }*/
    void setChar(int x, int y, char c) {
        currentBoard[y][x] = c;
    }
};
