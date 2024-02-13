#include "Common.h"

int generateRandomValue(int minValue, int maxValue) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distrib(minValue, maxValue);

	return distrib(gen);
}