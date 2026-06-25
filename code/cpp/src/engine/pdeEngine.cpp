#include "pdeEngine.hpp"
#include "mathFunctions.hpp"
#include "vectorUtils.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

PdeEngine::PdeEngine(const ModelParameters& modelParams,
                     const PdeParameters& pdeParams)
    : modelParams_(modelParams), pdeParams_(pdeParams) {

    validateInputs();
    defineGrid();
    computeCoefficients();
};

double PdeEngine::price(const Instrument& instrument) {

    std::vector<double> valuesCurrent(pdeParams_.nSpaceSteps + 1);

    for (int i = 0; i <= pdeParams_.nSpaceSteps; i++)
        valuesCurrent[i] = instrument.payoff({grid_.spaceGrid[i]});

    std::vector<double> valuesNext(pdeParams_.nSpaceSteps + 1);

    switch (pdeParams_.scheme) {
    case PdeScheme::Explicit: {

        cflCondition();
        return explicitMethod(valuesCurrent, valuesNext);
    }
    case PdeScheme::Implicit: {
        return 0.0;
    }
    case PdeScheme::CrankNicolson: {
        return 0.0;
    }
    };
};

double PdeEngine::explicitMethod(std::vector<double> valuesCurrent,
                                 std::vector<double> valuesNext) const {

    for (int t = pdeParams_.nTimeSteps; t > 0; t--) {
        for (int i = 1; i < pdeParams_.nSpaceSteps; i++) {
            valuesNext[i] =
                valuesCurrent[i] +
                grid_.timeDelta * (pdeCoeffs_.a[i] * valuesCurrent[i - 1] +
                                   pdeCoeffs_.b[i] * valuesCurrent[i] +
                                   pdeCoeffs_.c[i] * valuesCurrent[i + 1]);
        }

        valuesNext.front() = 0;
        valuesNext.back() = 2 * valuesNext[valuesNext.size() - 2] -
                            valuesNext[valuesNext.size() - 3];

        std::swap(valuesCurrent, valuesNext);
    }

    const ptrdiff_t idx =
        findClosestIndex(grid_.spaceGrid, modelParams_.underlyingPrice);

    return interpolationLinear(modelParams_.underlyingPrice,
                               grid_.spaceGrid[idx - 1], grid_.spaceGrid[idx],
                               valuesCurrent[idx - 1], valuesCurrent[idx]);
}

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

void PdeEngine::cflCondition() const {
    switch (pdeParams_.grid) {

    case PdeGrid::Uniform: {
        if (grid_.timeDelta >
            grid_.spaceDelta * grid_.spaceDelta /
                (modelParams_.volatility * modelParams_.volatility *
                 grid_.spaceMax * grid_.spaceMax))
            throw std::invalid_argument("CFL condition not satisfied");
        break;
    }
    case PdeGrid::Log: {
        if (grid_.timeDelta >
            grid_.spaceDelta * grid_.spaceDelta /
                (modelParams_.volatility * modelParams_.volatility))
            throw std::invalid_argument("CFL condition not satisfied");
        break;
    }
    };
}

void PdeEngine::defineGrid() {

    const double sMax{
        modelParams_.underlyingPrice *
        std::exp((modelParams_.interestRate - modelParams_.dividendRate -
                  0.5 * modelParams_.volatility * modelParams_.volatility) *
                     modelParams_.timeHorizonInYears +
                 modelParams_.volatility *
                     std::sqrt(modelParams_.timeHorizonInYears) * 3.0)};

    std::vector<double> spaceGrid(pdeParams_.nSpaceSteps + 1);

    switch (pdeParams_.grid) {
    case PdeGrid::Uniform: {

        const double sMin{0.0};
        const double ds{(sMax - sMin) / pdeParams_.nSpaceSteps};

        for (int i = 0; i <= pdeParams_.nSpaceSteps; i++)
            spaceGrid[i] = sMin + ds * i;

        grid_.spaceMax = sMax;
        grid_.spaceMin = sMin;
        grid_.spaceDelta = ds;
        grid_.spaceGrid = spaceGrid;

        break;
    }
    case PdeGrid::Log: {

        const double sMin{0.0001};
        const double logSMin{std::log(sMin)};
        const double logSMax{std::log(sMax)};
        const double logDs{(logSMax - logSMin) / pdeParams_.nSpaceSteps};

        for (int i = 0; i <= pdeParams_.nSpaceSteps; i++)
            spaceGrid[i] = std::exp(logSMin + logDs * i);

        grid_.spaceMax = logSMax;
        grid_.spaceMin = logSMin;
        grid_.spaceDelta = logDs;
        grid_.spaceGrid = spaceGrid;

        break;
    }
    };
    grid_.timeDelta = modelParams_.timeHorizonInYears / pdeParams_.nTimeSteps;
}

void PdeEngine::computeCoefficients() {

    std::vector<double> a(grid_.spaceGrid.size());
    std::vector<double> b(grid_.spaceGrid.size());
    std::vector<double> c(grid_.spaceGrid.size());

    switch (pdeParams_.grid) {
    case PdeGrid::Uniform: {

        const double factor1 =
            (modelParams_.volatility * modelParams_.volatility) /
            ((grid_.spaceDelta) * (grid_.spaceDelta));

        const double factor2 =
            (modelParams_.interestRate - modelParams_.dividendRate) /
            (2.0 * (grid_.spaceDelta));

        for (size_t i = 0; i < grid_.spaceGrid.size(); i++) {

            const double diffusionTerm =
                (0.5 * factor1 * (grid_.spaceGrid[i] * grid_.spaceGrid[i]));
            const double driftTerm = grid_.spaceGrid[i] * factor2;

            a[i] = diffusionTerm - driftTerm;
            b[i] = -2 * diffusionTerm - modelParams_.interestRate;
            c[i] = diffusionTerm + driftTerm;
        }
        break;
    }
    case PdeGrid::Log: {

        const double diffusionTerm =
            0.5 * (modelParams_.volatility * modelParams_.volatility) /
            ((grid_.spaceDelta) * (grid_.spaceDelta));

        const double driftTerm =
            (modelParams_.interestRate - modelParams_.dividendRate -
             modelParams_.volatility * modelParams_.volatility * 0.5) /
            (2.0 * (grid_.spaceDelta));

        std::fill(a.begin(), a.end(), diffusionTerm - driftTerm);
        std::fill(b.begin(), b.end(),
                  -2 * diffusionTerm - modelParams_.interestRate);
        std::fill(c.begin(), c.end(), diffusionTerm + driftTerm);
        break;
    }
    }

    pdeCoeffs_.a = std::move(a);
    pdeCoeffs_.b = std::move(b);
    pdeCoeffs_.c = std::move(c);
}
