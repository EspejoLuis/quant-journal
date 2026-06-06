// monteCarloEngine.h - Monte Carlo generator.
#pragma once
#include <optional>
#include <random>
#include <vector>

struct ModelParameters {
    double underlyingPrice;
    double interestRate;
    double dividendRate;
    double volatility;
    double maturityInYears;
};

struct SimulationParameters {
    int nPaths;
    int nSteps;
    std::optional<unsigned int> seed;
};

// Returns matrix of nPaths x nSteps
std::vector<std::vector<double>> simulateGbmPath(
    const ModelParameters& modelParams, const SimulationParameters& simParams);

std::mt19937 createRng(unsigned int seed);

std::mt19937 createRng();