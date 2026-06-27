#pragma once
#include "engine.hpp"
#include "modelParameters.hpp"
#include "pathGeneration.hpp"
#include "simulationParameters.hpp"
#include <cmath>
#include <stdexcept>
#include <vector>

template <typename T> class McEngine : public Engine {

  public:
    McEngine(const ModelParameters& modelParams,
             const SimulationParameters& simParams)
        : modelParams_(modelParams), simParams_(simParams) {

        validateInputs();
    };

    Engine::Arguments* getArguments() override { return &args_; };

    void calculate() override {

        std::vector<std::vector<double>> simulatedPaths =
            simulateGbmPath(modelParams_, simParams_);

        double sumPayoffsDiscounted = 0.0;

        const double deterministicDiscountFactor = std::exp(
            -modelParams_.interestRate * modelParams_.timeHorizonInYears);

        for (const std::vector<double>& path : simulatedPaths) {
            sumPayoffsDiscounted +=
                args_.payoff(path.back()) * deterministicDiscountFactor;
        }

        result_ = sumPayoffsDiscounted / simulatedPaths.size();
    };

    double getResult() const override { return result_; };

  private:
    const ModelParameters modelParams_;
    const SimulationParameters simParams_;
    typename T::Arguments args_;
    double result_ = 0.0;

    void validateInputs() const {
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
            simParams_.varianceReduction !=
                VarianceReduction::ControlVariates &&
            simParams_.varianceReduction !=
                VarianceReduction::ImportantSampling &&
            simParams_.varianceReduction)
            throw std::invalid_argument("invalid VarianceReduction");
    };
};