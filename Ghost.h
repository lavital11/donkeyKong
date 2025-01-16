#pragma once
#include "Point.h"

class Game;

class Ghost : public Point
{
	Game* g1;
	bool toRemove = false;          // Flag indicating if the ghost should be removed

public:
	Ghost(Board* board, int startX, int startY, Game* game, char symbol = 'x')
		: Point(startX, startY, symbol, board), g1(game) {}
	void move(bool noColors, const std::vector<Ghost>& ghosts);
	void setInitialDirection();
	bool shouldRemove() const { return toRemove; }
	void changeDir();
	void erase(bool noColors);
};

