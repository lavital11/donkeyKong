#pragma once
#include "Game.h"

class ManuGame : public Game
{
	std::vector<std::string> stepsFiles;
	std::vector<std::string> resultsFiles;
	ManuGame& operator=(const ManuGame& other);
protected:
public:
	ManuGame(bool isSave, std::vector<std::string> stepsFiles, std::vector<std::string> resultsFiles) :  stepsFiles(stepsFiles) , resultsFiles(resultsFiles){
	}
	ManuGame(bool isSave)  {}
	// Displays the opening menu
	void startGame();
	void showInstructions();
	
	void inputAction();
	
	void saveStepAfterFinish();
	void saveResultsAfterFinish();
	void setResFileName() {
		resultsFilename = resultsFiles[currentBoardIndex];
	}
	

};