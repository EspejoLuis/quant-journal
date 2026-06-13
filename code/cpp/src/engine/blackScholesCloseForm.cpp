#include "blackScholesCloseForm.h"
#include "mathFunctions.h"
#include <cmath>

BsCloseForm::BsCloseForm(const ModelParameters& modelParams)
    : modelParams_(modelParams) {

    validateInputs();
};

double BsCloseForm::price(
    const VanillaEuropeanOption& vanillaEuropeanOption) const {

    const double s0 = modelParams_.underlyingPrice;
    const double vol = modelParams_.volatility;
    const double q = modelParams_.dividendRate;
    const double r = modelParams_.interestRate;
    const double T = modelParams_.maturityInYears;

    const double K = vanillaEuropeanOption.parameters().strike;
    const OptionDirection direction =
        vanillaEuropeanOption.parameters().direction;
    const OptionType type = vanillaEuropeanOption.parameters().type;

    const double d1 = (std::log(s0 / K) + (r - q + 1.0 / 2.0 * vol * vol) * T) /
                      (vol * std::sqrt(T));
    const double d2 = d1 - (vol * std::sqrt(T));

    const double sign = direction == OptionDirection::Long ? 1.0 : -1.0;

    const double unsignedPrice =
        type == OptionType::Call ? s0 * std::exp(-q * T) * normalCdf(d1) -
                                       K * std::exp(-r * T) * normalCdf(d2)
                                 : -s0 * std::exp(-q * T) * normalCdf(-d1) +
                                       K * std::exp(-r * T) * normalCdf(-d2);

    return sign * unsignedPrice;
}

void BsCloseForm::validateInputs() const {

    if (modelParams_.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams_.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams_.maturityInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");
};
