
// blackScholesEngine.h - Black Scholes Closed Form computation.
#pragma once
#include "chooserEuropeanOption.hpp"
#include "digitalEuropeanOption.hpp"
#include "modelParameters.hpp"
#include "vanillaEuropeanOption.hpp"

class BsCloseForm {

  public:
    BsCloseForm(const ModelParameters& modelParams);

    double price(const VanillaEuropeanOption& vanillaEuropeanOption) const;
    double price(const DigitalEuropeanOption& digitalEuropeanOption) const;
    double price(const ChooserEuropeanOption& chooserEuropeanOption) const;

  private:
    const ModelParameters modelParams_;

    void validateInputs() const;
};
