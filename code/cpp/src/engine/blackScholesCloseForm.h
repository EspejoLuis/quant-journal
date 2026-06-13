
// blackScholesEngine.h - Black Scholes Closed Form computation.
#pragma once
#include "digitalEuropeanOption.h"
#include "modelParameters.h"
#include "vanillaEuropeanOption.h"

class BsCloseForm {

  public:
    BsCloseForm(const ModelParameters& modelParams);

    double price(const VanillaEuropeanOption& vanillaEuropeanOption) const;
    double price(const DigitalEuropeanOption& digitalEuropeanOption) const;

  private:
    const ModelParameters modelParams_;

    void validateInputs() const;
};
