#include "monteCarloEngine.h"
#include <cmath>
#include <stdexcept>

McEngine::McEngine(const ModelParameters& modelParams,
                   const SimulationParameters& simParams)
    : modelParams_(modelParams), simParams_(simParams) {

    validateInputs();
};

double McEngine::price(const Instrument& instrument) {

    std::vector<std::vector<double>> simulatedPaths = simulateGbmPath();

    double sumPayoffsDiscounted = 0.0;

    double deterministicDiscountFactor =
        std::exp(-modelParams_.interestRate * modelParams_.maturityInYears);

    for (const std::vector<double>& path : simulatedPaths) {
        sumPayoffsDiscounted +=
            instrument.payoff(path) * deterministicDiscountFactor;
    }

    return sumPayoffsDiscounted / simParams_.nPaths;
};

// Returns matrix of nPaths x nSteps
std::vector<std::vector<double>> McEngine::simulateGbmPath() {

    const double s0 = modelParams_.underlyingPrice;
    const double vol = modelParams_.volatility;
    const double q = modelParams_.dividendRate;
    const double r = modelParams_.interestRate;
    const double T = modelParams_.maturityInYears;

    const int nSteps = simParams_.nSteps;
    const int nPaths = simParams_.nPaths;
    const std::optional<unsigned int> seed = simParams_.seed;

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

void McEngine::validateInputs() {

    if (modelParams_.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams_.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams_.maturityInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");

    if (simParams_.nPaths <= 0)
        throw std::invalid_argument(
            "number of paths cannot be negative or zero");

    if (simParams_.nSteps <= 0)
        throw std::invalid_argument(
            "number of steps cannot be negative or zero");
}

std::mt19937 McEngine::createRng() {
    std::random_device randomDevice;
    return std::mt19937(randomDevice());
};

std::mt19937 McEngine::createRng(unsigned int seed) {
    return std::mt19937(seed);
};
