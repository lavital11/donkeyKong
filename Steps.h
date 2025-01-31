#pragma once
#include <list>
#include <string>
#include <utility>

class Steps {
	long random_seed = 0;
	std::list<std::pair<size_t, char>> steps; // pair: iteration, step
public:
	// Load steps from a file
	static Steps loadSteps(const std::string& filename);

	// Save steps to a file
	void saveSteps(const std::string& filename) const;

	// Getters and setters for randomSeed
	long getRandomSeed() const {
		return random_seed;
	}
	void setRandomSeed(long seed) {
		random_seed = seed;
	}

	// Add a new step
	void addStep(size_t iteration, char step) {
		steps.push_back({ iteration, step });
	}

	void clear() {
		steps.clear();
	}

	bool isEmpty() {
		return steps.empty();
	}

	// Check if the next step occurs on the given iteration
	bool isNextStepOnIteration(size_t iteration) const {
		return !steps.empty() && steps.front().first == iteration;
	}

	// Remove and return the next step
	char popStep() {
		char step = steps.front().second;
		steps.pop_front();
		return step;
	}

	// Check if a file is empty
	static bool isEmptyFile(const std::string& filename);
	auto front() { return steps.front(); }
	// Provide iterators for range-based for loops
	auto begin() { return steps.begin(); }
	auto end() { return steps.end(); }
	auto begin() const { return steps.begin(); }
	auto end() const { return steps.end(); }
};