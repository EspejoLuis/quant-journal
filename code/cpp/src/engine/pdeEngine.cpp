#include "pdeEngine.hpp"
#include <stdexcept>

PdeEngine::PdeEngine(const ModelParameters& modelParams,
                     const PdeParameters& pdeParams)
    : modelParams_(modelParams), pdeParams_(pdeParams) {
    validateInputs();
};

double PdeEngine::price(const Instrument& instrument) { return 0.0 };

void PdeEngine::validateInputs() const {

    if (modelParams_.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams_.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams_.timeHorizonInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");

    if (pdeParams_.nSpaceSteps <= 0)
        throw std::invalid_argument(
            "number of space steps cannot be negative or zero");

    if (pdeParams_.nTimeSteps <= 0)
        throw std::invalid_argument(
            "number of time steps cannot be negative or zero");

    if (pdeParams_.grid != PdeGrid::Log && pdeParams_.grid != PdeGrid::Uniform)
        throw std::invalid_argument("invalid PdeGrid");

    if (pdeParams_.scheme != PdeScheme::Explicit &&
        pdeParams_.scheme != PdeScheme::Implicit &&
        pdeParams_.scheme != PdeScheme::CrankNicolson)
        throw std::invalid_argument("invalid PdeScheme");
};