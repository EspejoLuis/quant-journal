#pragma once
#include "chooserEuropeanOption.hpp"
#include "modelParameters.hpp"
#include "simulationParameters.hpp"

class ChooserEngine {

  public:
    ChooserEngine(const ModelParameters& modelParams,
                  const SimulationParameters& simParams);

    double price(const ChooserEuropeanOption& chooserEuropeanOption);

  private:
    const ModelParameters modelParams_;
    const SimulationParameters simParams_;
};
