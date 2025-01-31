#include <cstdlib> // For srand and rand
#include <ctime>   // For time
#include <iostream>
#include "game.h"  // Include the Game class header
#include "AutoGame.h"
#include "ManuGame.h"
#include <filesystem>
#include <algorithm>

std::pair<std::vector<std::string>, std::vector<std::string>> findGameFiles();

int main(int argc, char** argv) {
    bool isLoad = argc > 1 && std::string(argv[1]) == "-load";   // טעינת קבצים
    bool isSave = argc > 1 && std::string(argv[1]) == "-save";   // משחק רגיל ושמירה
    bool isSilent = isLoad && argc > 2 && std::string(argv[2]) == "-silent"; // מצב שקט בטעינה

    //srand(static_cast<unsigned int>(time(0))); // אתחול לולאה רנדומלית

    Game* g1 = nullptr;
    auto [stepsFiles, resultsFiles] = findGameFiles();
    if (stepsFiles.empty() || resultsFiles.empty()) {
        throw std::runtime_error("No .steps or .results files found in the directory.");
    }
    try {
        // מצב טעינת קבצים
        if (isLoad) {
            g1 = new AutoGame(isSilent, stepsFiles, resultsFiles);
            g1->setIsAuto(true);
            g1->setIsSave(false);
        }
        // מצב רגיל עם שמירה
        else if (isSave) {
            g1 = new ManuGame(true, stepsFiles, resultsFiles);
            g1->setIsAuto(false);
            g1->setIsSave(true);

        }
        // מצב רגיל ללא שמירה
        else {
            g1 = new ManuGame(false);
            g1->setIsAuto(false);
            g1->setIsSave(false);
        }

        // הרצת המשחק
        g1->startGame();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    delete g1;       // ניקוי זיכרון

    return 0;
}

std::pair<std::vector<std::string>, std::vector<std::string>> findGameFiles() {
    namespace fs = std::filesystem;
    std::vector<std::string> stepsFiles;
    std::vector<std::string> resultsFiles;

    // הגדרת התיקייה שבה יתבצע החיפוש
    std::string directory = "boards";

    // בדיקה אם התיקייה קיימת
    if (!fs::exists(directory)) {
        std::cerr << "Error: Directory '" << directory << "' does not exist!" << std::endl;
        return { {}, {} };
    }

    // חיפוש קבצים בתוך התיקייה
    for (const auto& entry : fs::directory_iterator(directory)) {
        const auto& path = entry.path();
        if (path.extension() == ".steps") {
            stepsFiles.push_back(path.string());
        }
        else if (path.extension() == ".results") {
            resultsFiles.push_back(path.string());
        }
    }

    // מיון לפי שם הקובץ להבטחת סדר נכון
    std::sort(stepsFiles.begin(), stepsFiles.end());
    std::sort(resultsFiles.begin(), resultsFiles.end());

    return { stepsFiles, resultsFiles };
}