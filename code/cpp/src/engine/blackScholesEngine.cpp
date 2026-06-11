#include "blackScholesEngine.h"
#include <cmath>

static double normalCdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
};

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

    double d1 = (std::log(s0 / K) + (r - q + 1.0 / 2.0 * vol * vol) * T) /
                (vol * std::sqrt(T));
    double d2 = d1 - (vol * std::sqrt(T));

    double sign = 0.0;

    switch (direction) {
    case OptionDirection::Long:
        sign = +1.0;
        break;
    case OptionDirection::Short:
        sign = -1.0;
        break;
    }

    double unsignedPrice = 0.0;
    switch (type) {
    case OptionType::Call: {
        unsignedPrice = s0 * std::exp(-q * T) * normalCdf(d1) -
                        K * std::exp(-r * T) * normalCdf(d2);
        break;
    }
    case OptionType::Put: {
        unsignedPrice = -s0 * std::exp(-q * T) * normalCdf(-d1) +
                        K * std::exp(-r * T) * normalCdf(-d2);
        break;
    }
    }

    return sign * unsignedPrice;
}

void BsCloseForm::validateInputs() {

    if (modelParams_.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams_.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams_.maturityInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");
};
