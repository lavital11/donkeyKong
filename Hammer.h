#pragma once
#include "Point.h"

class Game;

class Hammer : public Point
{
	Game* g1;

public:
	Hammer(Board* board, int startX, int startY, Game* game, char symbol = 'p')
		: Point(startX, startY, symbol, board), g1(game) {}
	void printP() const;
	void deletetP() const;
};

