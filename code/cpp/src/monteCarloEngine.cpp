#include "monteCarloEngine.h"

// Returns matrix of nPaths x nTimeSteps
std::vector<std::vector<double>> simulateGbmPath(
    const ModelParameters& modelParams, const SimulationParameters& simParams) {
};

std::mt19937 createRng() {
    std::random_device randomDevice;
    return std::mt19937(randomDevice());
};

std::mt19937 createRng(unsigned int seed) { return std::mt19937(seed); };