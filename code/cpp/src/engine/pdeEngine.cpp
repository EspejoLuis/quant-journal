#include "pdeEngine.hpp"
#include "mathFunctions.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

PdeEngine::PdeEngine(const ModelParameters& modelParams,
                     const PdeParameters& pdeParams)
    : modelParams_(modelParams), pdeParams_(pdeParams) {
    validateInputs();
};

double PdeEngine::price(const Instrument& instrument) {

    const std::vector<double> spaceGrid = defineSpaceGrid();

    std::vector<double> valuesCurrent(pdeParams_.nSpaceSteps + 1);

    for (int i = 0; i <= pdeParams_.nSpaceSteps; i++)
        valuesCurrent[i] = instrument.payoff({spaceGrid[i]});

    std::vector<double> valuesNext(pdeParams_.nSpaceSteps + 1);

    PdeCoefficients pdeCoeffs = computeCoefficients(spaceGrid);

    const double dt = modelParams_.timeHorizonInYears / pdeParams_.nTimeSteps;

    switch (pdeParams_.scheme) {
    case PdeScheme::Explicit: {

        // cfl

        for (int t = pdeParams_.nTimeSteps; t > 0; t--) {
            for (int i = 1; i < pdeParams_.nSpaceSteps; i++) {
                valuesNext[i] = valuesCurrent[i] +
                                dt * (pdeCoeffs.a[i] * valuesCurrent[i - 1] +
                                      pdeCoeffs.b[i] * valuesCurrent[i] +
                                      pdeCoeffs.c[i] * valuesCurrent[i + 1]);
            }

            valuesNext.front() = 0;
            valuesNext.back() = 2 * valuesNext[valuesNext.size() - 2] -
                                valuesNext[valuesNext.size() - 3];

            std::swap(valuesCurrent, valuesNext);
        }

        std::vector<double>::const_iterator it = std::lower_bound(
            spaceGrid.begin(), spaceGrid.end(), modelParams_.underlyingPrice);

        const ptrdiff_t idx = std::distance(spaceGrid.begin(), it);

        return interpolationLinear(modelParams_.underlyingPrice,
                                   spaceGrid[idx - 1], spaceGrid[idx],
                                   valuesCurrent[idx - 1], valuesCurrent[idx]);
    }
    case PdeScheme::Implicit: {
        return 0.0;
    }
    case PdeScheme::CrankNicolson: {
        return 0.0;
    }
    };
};

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

std::vector<double> PdeEngine::defineSpaceGrid() const {

    std::vector<double> spaceGrid(pdeParams_.nSpaceSteps + 1);

    const double sMax{
        modelParams_.underlyingPrice *
        std::exp((modelParams_.interestRate - modelParams_.dividendRate -
                  0.5 * modelParams_.volatility * modelParams_.volatility) *
                     modelParams_.timeHorizonInYears +
                 modelParams_.volatility *
                     std::sqrt(modelParams_.timeHorizonInYears) * 3.0)};

    switch (pdeParams_.grid) {
    case PdeGrid::Uniform: {

        const double sMin{0.0};
        const double ds{(sMax - sMin) / pdeParams_.nSpaceSteps};

        for (int i = 0; i <= pdeParams_.nSpaceSteps; i++)
            spaceGrid[i] = sMin + ds * i;

        return spaceGrid;
    }
    case PdeGrid::Log: {

        const double sMin{0.0001};
        const double logSMin{std::log(sMin)};
        const double logSMax{std::log(sMax)};
        const double logDs{(logSMax - logSMin) / pdeParams_.nSpaceSteps};

        for (int i = 0; i <= pdeParams_.nSpaceSteps; i++)
            spaceGrid[i] = std::exp(logSMin + logDs * i);

        return spaceGrid;
    }
    };
}

PdeCoefficients PdeEngine::computeCoefficients(
    const std::vector<double>& spaceGrid) const {

    const double factor1 =
        (modelParams_.volatility * modelParams_.volatility) /
        ((spaceGrid[1] - spaceGrid[0]) * (spaceGrid[1] - spaceGrid[0]));

    const double factor2 =
        (modelParams_.interestRate - modelParams_.dividendRate) /
        (2.0 * (spaceGrid[1] - spaceGrid[0]));

    std::vector<double> a(spaceGrid.size());
    std::vector<double> b(spaceGrid.size());
    std::vector<double> c(spaceGrid.size());

    for (size_t i = 0; i < spaceGrid.size(); i++) {

        const double diffusionTerm =
            (0.5 * factor1 * (spaceGrid[i] * spaceGrid[i]));
        const double driftTerm = spaceGrid[i] * factor2;

        a[i] = diffusionTerm - driftTerm;
        b[i] = -2 * diffusionTerm - modelParams_.interestRate;
        c[i] = diffusionTerm + driftTerm;
    }

    return PdeCoefficients{a, b, c};
};
