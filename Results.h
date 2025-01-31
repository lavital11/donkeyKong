#pragma once
#include <list>
#include <string>
#include <fstream>

class Results {
public:
	enum ResultValue { loseLife, finishLevel, score ,noResult};// to do score

	bool operator==(const Results& other) const
	{
		return results == other.results; // שימוש באופרטור השוואה של list
	}
private:
	std::list<std::pair<size_t, ResultValue>> results; // pair: iteration, result
public:
	static Results loadResults(const std::string& filename);
	void saveResults(const std::string& filename, int ourScore) const;
	void addResult(size_t iteration, ResultValue result) {
		results.push_back({ iteration, result });
	}
	void addScore(ResultValue result, int score, const std::string& filename) {
		std::ofstream results_file(filename);
		results_file << '\n' << result << ':' << score;
	}

	std::pair<size_t, ResultValue> popResult() {
		if (results.empty()) return { 0, Results::noResult };
		auto result = results.front();
		results.pop_front();
		return result;
	}

	bool isFinishedBy(size_t iteration) const {
		return results.empty() || (results.back().first <= iteration) && (results.back().second == 1);
	}
	std::pair<size_t, ResultValue> back() {
		return results.back();
	}

	size_t getNextDeadIteration() const;

	void clear() {
		results.clear();
	}
};