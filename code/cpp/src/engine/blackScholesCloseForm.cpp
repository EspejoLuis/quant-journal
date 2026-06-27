#include "blackScholesCloseForm.hpp"
#include "vanillaEuropeanOption.hpp"

template <> void BsCloseForm<VanillaEuropeanOption>::calculate() {

    const double s0 = modelParams_.underlyingPrice;
    const double q = modelParams_.dividendRate;
    const double r = modelParams_.interestRate;
    const double T = modelParams_.timeHorizonInYears;

    const double K = args_.engineParams.strike;
    const OptionDirection direction = args_.engineParams.direction;
    const OptionType type = args_.engineParams.type;

    const double sign = direction == OptionDirection::Long ? 1.0 : -1.0;

    DFactors dFactors = computeDFactors();

    const double unsignedPrice =
        type == OptionType::Call
            ? s0 * std::exp(-q * T) * normalCdf(dFactors.d1) -
                  K * std::exp(-r * T) * normalCdf(dFactors.d2)
            : -s0 * std::exp(-q * T) * normalCdf(-dFactors.d1) +
                  K * std::exp(-r * T) * normalCdf(-dFactors.d2);

    result_ = sign * unsignedPrice;
}

template <> void BsCloseForm<DigitalEuropeanOption>::calculate() {

    const double s0 = modelParams_.underlyingPrice;
    const double q = modelParams_.dividendRate;
    const double r = modelParams_.interestRate;
    const double T = modelParams_.timeHorizonInYears;

    const OptionDirection direction = args_.engineParams.direction;
    const OptionType type = args_.engineParams.type;
    const DigitalType digitalType = args_.engineParams.digitalType;

    DFactors dFactors = computeDFactors();

    const double sign = direction == OptionDirection::Long ? 1.0 : -1.0;

    const double payoutFactor =
        digitalType == DigitalType::AssetOrNothing
            ? s0 * std::exp(-q * T)
            : args_.engineParams.payAmount.value() * std::exp(-r * T);

    const double dFactor =
        digitalType == DigitalType::AssetOrNothing
            ? (type == OptionType::Call ? +dFactors.d1 : -dFactors.d1)
            : (type == OptionType::Call ? +dFactors.d2 : -dFactors.d2);

    result_ = sign * payoutFactor * normalCdf(dFactor);
}

template <> void BsCloseForm<ChooserEuropeanOption>::calculate() {

    const double K = args_.engineParams.strike;
    const OptionDirection direction = args_.engineParams.direction;
    const double T = args_.engineParams.maturity;
    const double T_c = modelParams_.timeHorizonInYears;

    ModelParameters callParams = modelParams_;
    callParams.timeHorizonInYears = T;

    VanillaEuropeanOption call{OptionParameters{K, OptionType::Call, direction}};
    BsCloseForm<VanillaEuropeanOption> bsCloseFormCall{callParams};
    call.setPricingEngine(&bsCloseFormCall);

    const double KDiscounted =
        K * std::exp(-(modelParams_.interestRate - modelParams_.dividendRate) *
                     (T - T_c));
    const double discountingFactor =
        std::exp(-modelParams_.dividendRate * (T - T_c));

    ModelParameters putParams = modelParams_;
    putParams.timeHorizonInYears = T_c;

    VanillaEuropeanOption put{
        OptionParameters{KDiscounted, OptionType::Put, direction}};
    BsCloseForm<VanillaEuropeanOption> bsCloseFormPut{putParams};
    put.setPricingEngine(&bsCloseFormPut);

    result_ = call.price() + put.price() * discountingFactor;
}
