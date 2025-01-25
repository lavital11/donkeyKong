#pragma once
#include "Game.h"

class AutoGame : public Game
{
	long random_seed = 0;
	Steps steps;
	Results results;
	bool isSilent;
	std::string filename_prefix = boardFiles[currentBoardIndex].substr(0, (boardFiles[currentBoardIndex]).find_last_of('.'));
	std::string stepsFilename = filename_prefix + ".steps";
	std::string resultsFilename = filename_prefix + ".result";
	AutoGame& operator=(const AutoGame& other);

public :
	AutoGame(bool isSilent) : isSilent(isSilent){}
	void canRun(const std::vector<std::string>& stepsFiles, const std::vector<std::string>& resultsFiles);
	void runGameForCurrentScreen();
};