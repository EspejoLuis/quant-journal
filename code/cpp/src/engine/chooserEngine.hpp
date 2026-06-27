#pragma once
#include "chooserEuropeanOption.hpp"
#include "modelParameters.hpp"
#include "simulationParameters.hpp"

class ChooserEngine : public Engine {

  public:
    ChooserEngine(const ModelParameters& modelParams,
                  const SimulationParameters& simParams);

    Engine::Arguments* getArguments() override { return &args_; };

    void calculate() override;

    double getResult() const override { return result_; };

  private:
    const ModelParameters modelParams_;
    const SimulationParameters simParams_;
    ChooserEuropeanOption::Arguments args_;
    double result_ = 0.0;
};
