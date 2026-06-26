#include "blackScholesCloseForm.hpp"
#include "mathFunctions.hpp"
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
    const double T = modelParams_.timeHorizonInYears;

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

double BsCloseForm::price(
    const DigitalEuropeanOption& digitalEuropeanOption) const {

    const double s0 = modelParams_.underlyingPrice;
    const double vol = modelParams_.volatility;
    const double q = modelParams_.dividendRate;
    const double r = modelParams_.interestRate;
    const double T = modelParams_.timeHorizonInYears;

    const double K = digitalEuropeanOption.parameters().strike;

    const OptionDirection direction =
        digitalEuropeanOption.parameters().direction;
    const OptionType type = digitalEuropeanOption.parameters().type;
    const DigitalType digitalType =
        digitalEuropeanOption.parameters().digitalType;

    const double d1 = (std::log(s0 / K) + (r - q + 1.0 / 2.0 * vol * vol) * T) /
                      (vol * std::sqrt(T));
    const double d2 = d1 - (vol * std::sqrt(T));

    const double sign = direction == OptionDirection::Long ? 1.0 : -1.0;

    const double payoutFactor =
        digitalType == DigitalType::AssetOrNothing
            ? s0 * std::exp(-q * T)
            : digitalEuropeanOption.parameters().payAmount.value() *
                  std::exp(-r * T);

    const double dFactor = digitalType == DigitalType::AssetOrNothing
                               ? (type == OptionType::Call ? +d1 : -d1)
                               : (type == OptionType::Call ? +d2 : -d2);

    return sign * payoutFactor * normalCdf(dFactor);
}

double BsCloseForm::price(
    const ChooserEuropeanOption& chooserEuropeanOption) const {

    const double K = chooserEuropeanOption.parameters().strike;
    const OptionDirection direction =
        chooserEuropeanOption.parameters().direction;

    VanillaEuropeanOption call =
        VanillaEuropeanOption{OptionParameters{K, OptionType::Call, direction}};

    ModelParameters callParams = modelParams_;

    const double T = chooserEuropeanOption.parameters().maturity;
    callParams.timeHorizonInYears = T;

    const double callPrice = BsCloseForm{callParams}.price(call);

    const double T_c = modelParams_.timeHorizonInYears;
    const double KDiscounted =
        K * std::exp(-(modelParams_.interestRate - modelParams_.dividendRate) *
                     (T - T_c));
    const double discountingFactor =
        std::exp(-modelParams_.dividendRate * (T - T_c));

    ModelParameters putParams = modelParams_;
    putParams.timeHorizonInYears = T_c;

    VanillaEuropeanOption put = VanillaEuropeanOption{
        OptionParameters{KDiscounted, OptionType::Put, direction}};

    const double putPrice = BsCloseForm{putParams}.price(put);

    return callPrice + putPrice * discountingFactor;
}

void BsCloseForm::validateInputs() const {

    if (modelParams_.underlyingPrice < 0.0)
        throw std::invalid_argument("underlying price cannot be negative");

    if (modelParams_.volatility < 0.0)
        throw std::invalid_argument("volatility cannot be negative");

    if (modelParams_.timeHorizonInYears <= 0)
        throw std::invalid_argument(
            "maturity in years cannot be negative or zero");
};
