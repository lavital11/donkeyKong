#include "Hammer.h"

// Prints the hammer symbol 'p' at a fixed position (3, 3) on the board
void Hammer::printP(bool isSilent) const {
    if (isSilent) return;
    gotoxy(3, 3); // Move the cursor to position (3, 3)
    std::cout << 'p'; // Display the hammer symbol
}

// Deletes the hammer symbol 'p' from the fixed position (3, 3) on the board
void Hammer::deletetP(bool isSilent) const {
    if (isSilent) return;
    gotoxy(3, 3); // Move the cursor to position (3, 3)
    std::cout << ' '; // Replace the hammer symbol with a blank space
}