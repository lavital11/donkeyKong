#pragma once
#include "Game.h"

class AutoGame : public Game {
private:
    bool silentMode;                              // מצב שקט - אם נכון, המשחק רץ ללא תצוגה
    std::vector<std::string> stepsFiles;         // רשימת קבצי צעדים
    std::vector<std::string> resultsFiles;       // רשימת קבצי תוצאות
public:
    AutoGame(bool silentMode, const std::vector<std::string>& stepsFiles, const std::vector<std::string>& resultsFiles);
    AutoGame& operator=(const AutoGame& other);  // אופרטור השמה
    void startGame();                   // הפעלת המשחק במצב אוטומטי
    void inputAction();
    
    void saveStepAfterFinish() override {}
    void saveResultsAfterFinish() override{}

};