#pragma once
#include "engine.hpp"
#include "modelParameters.hpp"
#include "pdeParameters.hpp"
#include <vector>

class PdeEngine : public Engine {

  public:
    PdeEngine(const ModelParameters& modelParams,
              const PdeParameters& pdeParams);

    double price(const Instrument& instrument) override;

  private:
    const ModelParameters modelParams_;
    const PdeParameters pdeParams_;

    std::vector<double> defineSpaceGrid() const;

    PdeCoefficients computeCoefficients(
        const std::vector<double>& spaceGrid) const;

    void validateInputs() const;
};
