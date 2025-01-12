#include "Hammer.h"

void Hammer::printP() const {
    gotoxy(3, 3);
    std::cout << 'p';
}

void Hammer::deletetP() const {
    gotoxy(3, 3);
    std::cout << ' ';
}