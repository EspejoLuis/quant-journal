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
    // Properties
    const ModelParameters modelParams_;
    const PdeParameters pdeParams_;

    PdeGridParameters grid_;
    PdeCoefficients pdeCoeffs_;

    // Methods
    void defineGrid();
    void computeCoefficients();
    void validateInputs() const;
    void cflCondition() const;
    double explicitMethod(std::vector<double> valuesCurrent,
                          std::vector<double> valuesNext) const;
};
