#include "monteCarloEngine.hpp"
#include "pathGeneration.hpp"
#include <cmath>
#include <stdexcept>

McEngine::McEngine(const ModelParameters& modelParams,
                   const SimulationParameters& simParams)
    : modelParams_(modelParams), simParams_(simParams) {

    validateInputs();
};

double McEngine::price(const Instrument& instrument) {

    std::vector<std::vector<double>> simulatedPaths =
        simulateGbmPath(modelParams_, simParams_);

    double sumPayoffsDiscounted = 0.0;

    double deterministicDiscountFactor =
        std::exp(-modelParams_.interestRate * modelParams_.timeHorizonInYears);

    for (const std::vector<double>& path : simulatedPaths) {
        sumPayoffsDiscounted +=
            instrument.payoff(path) * deterministicDiscountFactor;
    }

    return sumPayoffsDiscounted / simulatedPaths.size();
};

void McEngine::validateInputs() const {

    if (modelParams_.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams_.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams_.timeHorizonInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");

    if (simParams_.nPaths <= 0)
        throw std::invalid_argument(
            "number of paths cannot be negative or zero");

    if (simParams_.nSteps <= 0)
        throw std::invalid_argument(
            "number of steps cannot be negative or zero");

    if (simParams_.varianceReduction != VarianceReduction::Antithetic &&
        simParams_.varianceReduction != VarianceReduction::ControlVariates &&
        simParams_.varianceReduction != VarianceReduction::ImportantSampling &&
        simParams_.varianceReduction)
        throw std::invalid_argument("invalid VarianceReduction");
}
