#pragma once
#include "Game.h"
class ManuGame : public Game
{
	bool isSave;
	ManuGame& operator=(const ManuGame& other);

public:
	ManuGame(bool isSave) : isSave(isSave) {}

};