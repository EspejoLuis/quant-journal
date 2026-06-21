// monteCarloEngine.h - Monte Carlo generator.
#pragma once
#include "engine.hpp"
#include "modelParameters.hpp"
#include "simulationParameters.hpp"
#include <optional>
#include <random>
#include <vector>

class McEngine : public Engine {

  public:
    McEngine(const ModelParameters& modelParams,
             const SimulationParameters& simParams);

    double price(const Instrument& instrument) override;

  private:
    const ModelParameters modelParams_;
    const SimulationParameters simParams_;

    void validateInputs() const;
};
