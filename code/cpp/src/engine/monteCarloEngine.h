// monteCarloEngine.h - Monte Carlo generator.
#pragma once
#include "engine.h"
#include "modelParameters.h"
#include <optional>
#include <random>
#include <vector>

enum class VarianceReduction {
    Antithetic,
    ImportantSampling,
    ControlVariates,
};

struct SimulationParameters {
    int nPaths;
    int nSteps;
    std::optional<unsigned int> seed;
    std::optional<VarianceReduction> varianceReduction;
};

class McEngine : public Engine {

  public:
    McEngine(const ModelParameters& modelParams,
             const SimulationParameters& simParams);

    double price(const Instrument& instrument) override;

  private:
    const ModelParameters modelParams_;
    const SimulationParameters simParams_;

    // Returns matrix of nPaths x nSteps
    std::vector<std::vector<double>> simulateGbmPath();

    void validateInputs() const;

    std::mt19937 createRng(unsigned int seed);

    std::mt19937 createRng();
};
