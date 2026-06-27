#include "digitalEuropeanOption.hpp"

DigitalEuropeanOption::DigitalEuropeanOption(
    const DigitalOptionParameters& params)
    : params_(params) {

    validateInputs();
};

void DigitalEuropeanOption::validateInputs() const {

    if (params_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (params_.type != OptionType::Call && params_.type != OptionType::Put)
        throw std::invalid_argument("invalid OptionType");

    if (params_.direction != OptionDirection::Short &&
        params_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");

    if (params_.digitalType != DigitalType::AssetOrNothing &&
        params_.digitalType != DigitalType::CashOrNothing)
        throw std::invalid_argument("invalid DigitalType");

    if (params_.digitalType == DigitalType::AssetOrNothing && params_.payAmount)
        throw std::invalid_argument("Asset or Nothing doesn't need payAmount");

    if (params_.digitalType == DigitalType::CashOrNothing && !params_.payAmount)
        throw std::invalid_argument("Cash or Nothing needs payAmount");

    if (params_.digitalType == DigitalType::CashOrNothing &&
        params_.payAmount < 0)
        throw std::invalid_argument("payAmount cannot be negative");
};

void DigitalEuropeanOption::setArguments(Engine::Arguments* args) const {
    if (!args)
        throw std::logic_error("This is not the right engine");

    DigitalEuropeanOption::Arguments* engineArgs =
        dynamic_cast<DigitalEuropeanOption::Arguments*>(args);

    if (!engineArgs)
        throw std::logic_error("This is not the right engine");

    engineArgs->engineParams = params_;
};

double DigitalEuropeanOption::Arguments::payoff(const double underlying) const {

    const double sign =
        engineParams.direction == OptionDirection::Long ? 1.0 : -1.0;

    const double underlyingPrice = underlying;

    const bool indicator = engineParams.type == OptionType::Call
                               ? underlyingPrice >= engineParams.strike
                               : underlyingPrice < engineParams.strike;

    const double payoutAmount =
        engineParams.digitalType == DigitalType::AssetOrNothing
            ? underlyingPrice
            : engineParams.payAmount.value();

    return sign * indicator * payoutAmount;
};
