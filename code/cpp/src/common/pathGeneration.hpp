#pragma once
#include "modelParameters.hpp"
#include "simulationParameters.hpp"
#include <cmath>
#include <random>
#include <vector>

std::mt19937 inline createRng() {
    std::random_device randomDevice;
    return std::mt19937(randomDevice());
};

std::mt19937 inline createRng(unsigned int seed) { return std::mt19937(seed); };

std::vector<std::vector<double>> inline simulateGbmPath(
    const ModelParameters& modelParams, const SimulationParameters& simParams) {

    const double s0{modelParams.underlyingPrice};
    const double vol{modelParams.volatility};
    const double q{modelParams.dividendRate};
    const double r{modelParams.interestRate};
    const double T{modelParams.timeHorizonInYears};

    const int nSteps{simParams.nSteps};
    const int nPaths{simParams.nPaths};
    const std::optional<unsigned int> seed{simParams.seed};
    const std::optional<VarianceReduction> varReduction{
        simParams.varianceReduction};

    // Standard Normal Distribution
    std::normal_distribution<double> distr(0.0, 1.0);
    std::mt19937 rng{seed ? createRng(*seed) : createRng()};

    const double dt{T / nSteps};
    const double expFactor{
        std::exp((r - q) * dt - 1.0 / 2.0 * vol * vol * dt)};
    const double volTime{vol * std::sqrt(dt)};

    if (varReduction && varReduction == VarianceReduction::Antithetic) {

        const int nPathAdjusted{nPaths * 2};

        std::vector<std::vector<double>> simulatedPrices(
            nPathAdjusted, std::vector<double>(nSteps + 1, s0));

        for (int i = 0; i < nPaths; i++) {
            for (int j = 0; j < nSteps; j++) {

                double z{distr(rng)};

                simulatedPrices[i][j + 1] =
                    simulatedPrices[i][j] * expFactor * std::exp(volTime * z);

                simulatedPrices[nPathAdjusted - 1 - i][j + 1] =
                    simulatedPrices[nPathAdjusted - 1 - i][j] * expFactor *
                    std::exp(volTime * -z);
            }
        }
        return simulatedPrices;

    } else {

        std::vector<std::vector<double>> simulatedPrices(
            nPaths, std::vector<double>(nSteps + 1, s0));

        for (int i = 0; i < nPaths; i++) {
            for (int j = 0; j < nSteps; j++) {

                double z{distr(rng)};

                simulatedPrices[i][j + 1] =
                    simulatedPrices[i][j] * expFactor * std::exp(volTime * z);
            };
        }
        return simulatedPrices;
    }
};
