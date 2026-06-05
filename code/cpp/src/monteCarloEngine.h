// monteCarloEngine.h - Monte Carlo generator.
#pragma once
#include <random>
#include <vector>

struct ModelParameters {
    double underlyingPrice;
    double interestRate;
    double dividendRate;
    double volatility;
    double maturity;
};

struct SimulationParameters {
    int nTimeSteps;
    int nPaths;
};

// Returns matrix of nPaths x nTimeSteps
std::vector<std::vector<double>> simulateGbmPath(
    const ModelParameters& modelParams, const SimulationParameters& simParams);

std::mt19937 createRng(unsigned int seed);

std::mt19937 createRng();