
#pragma once
#include "chooserEuropeanOption.hpp"
#include "digitalEuropeanOption.hpp"
#include "engine.hpp"
#include "mathFunctions.hpp"
#include "modelParameters.hpp"
#include "vanillaEuropeanOption.hpp"
#include <cmath>
#include <stdexcept>

template <typename T> class BsCloseForm : public Engine {

  public:
    BsCloseForm(const ModelParameters& modelParams)
        : modelParams_(modelParams) {

        validateInputs();
    };

    Engine::Arguments* getArguments() override { return &args_; };

    void calculate() override {};

    double getResult() const override { return result_; };

  private:
    const ModelParameters modelParams_;

    typename T::Arguments args_;
    double result_ = 0.0;
    struct DFactors {
        double d1;
        double d2;
    };

    DFactors computeDFactors() {

        const double d1 =
            (std::log(modelParams_.underlyingPrice /
                      args_.engineParams.strike) +
             (modelParams_.interestRate - modelParams_.dividendRate +
              1.0 / 2.0 * modelParams_.volatility * modelParams_.volatility) *
                 modelParams_.timeHorizonInYears) /
            (modelParams_.volatility *
             std::sqrt(modelParams_.timeHorizonInYears));

        const double d2 = d1 - (modelParams_.volatility *
                                std::sqrt(modelParams_.timeHorizonInYears));
        DFactors dFactors{.d1 = d1, .d2 = d2};

        return dFactors;
    };

    void validateInputs() const {
        if (modelParams_.underlyingPrice < 0.0)
            throw std::invalid_argument("underlying price cannot be negative");

        if (modelParams_.volatility < 0.0)
            throw std::invalid_argument("volatility cannot be negative");

        if (modelParams_.timeHorizonInYears <= 0)
            throw std::invalid_argument(
                "maturity in years cannot be negative or zero");
    };
};

template <> void BsCloseForm<VanillaEuropeanOption>::calculate();
template <> void BsCloseForm<DigitalEuropeanOption>::calculate();
template <> void BsCloseForm<ChooserEuropeanOption>::calculate();