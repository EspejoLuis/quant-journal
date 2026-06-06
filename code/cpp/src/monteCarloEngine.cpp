#include "monteCarloEngine.h"
#include <cassert>
#include <cmath>

// Returns matrix of nPaths x nSteps
std::vector<std::vector<double>> simulateGbmPath(
    const ModelParameters& modelParams, const SimulationParameters& simParams) {

    const double s0 = modelParams.underlyingPrice;
    const double q = modelParams.dividendRate;
    const double r = modelParams.interestRate;
    const double vol = modelParams.volatility;
    const double T = modelParams.maturityInYears;

    assert(s0 >= 0.0);
    assert(vol >= 0.0);
    assert(q >= 0.0);
    assert(T > 0.0);

    const int nPaths = simParams.nPaths;
    const int nSteps = simParams.nSteps;
    const std::optional<unsigned int> seed = simParams.seed;

    assert(nPaths > 0);
    assert(nSteps > 0);

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

std::mt19937 createRng() {
    std::random_device randomDevice;
    return std::mt19937(randomDevice());
};

std::mt19937 createRng(unsigned int seed) { return std::mt19937(seed); };