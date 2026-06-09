#include "monteCarloEngine.h"
#include <cmath>
#include <stdexcept>

// Returns matrix of nPaths x nSteps
std::vector<std::vector<double>> simulateGbmPath(
    const ModelParameters& modelParams, const SimulationParameters& simParams) {

    validateGbmInputs(modelParams, simParams);

    const double s0 = modelParams.underlyingPrice;
    const double vol = modelParams.volatility;
    const double q = modelParams.dividendRate;
    const double r = modelParams.interestRate;
    const double T = modelParams.maturityInYears;

    const int nSteps = simParams.nSteps;
    const int nPaths = simParams.nPaths;
    const std::optional<unsigned int> seed = simParams.seed;

    std::vector<std::vector<double>> simulatedPrices(
        nPaths, std::vector<double>(nSteps + 1, s0));

    // Standard Normal Distribution
    std::normal_distribution<double> distr(0.0, 1.0);
    std::mt19937 rng = seed ? createRng(*seed) : createRng();

    const double dt = T / nSteps;

    for (int i = 0; i < nPaths; i++) {
        for (int j = 0; j < nSteps; j++) {

            double z = distr(rng);

            simulatedPrices[i][j + 1] =
                simulatedPrices[i][j] *
                std::exp((r - q) * dt - 1.0 / 2.0 * vol * vol * dt +
                         vol * std::sqrt(dt) * z);
        }
    }
    return simulatedPrices;
};

void validateGbmInputs(const ModelParameters& modelParams,
                       const SimulationParameters& simParams) {

    if (modelParams.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams.maturityInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");

    if (simParams.nPaths <= 0)
        throw std::invalid_argument(
            "number of paths cannot be negative or zero");

    if (simParams.nSteps <= 0)
        throw std::invalid_argument(
            "number of steps cannot be negative or zero");
}

std::mt19937 createRng() {
    std::random_device randomDevice;
    return std::mt19937(randomDevice());
};

std::mt19937 createRng(unsigned int seed) { return std::mt19937(seed); };