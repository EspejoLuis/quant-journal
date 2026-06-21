#include "digitalEuropeanOption.hpp"

DigitalEuropeanOption::DigitalEuropeanOption(
    const DigitalOptionParameters& optParams)
    : optParams_(optParams) {

    validateInputs();
};

double DigitalEuropeanOption::payoff(const std::vector<double>& path) const {

    const double sign =
        optParams_.direction == OptionDirection::Long ? 1.0 : -1.0;

    const double underlyingPrice = path.back();

    const bool indicator = optParams_.type == OptionType::Call
                               ? underlyingPrice >= optParams_.strike
                               : underlyingPrice < optParams_.strike;

    const double payoutAmount =
        optParams_.digitalType == DigitalType::AssetOrNothing
            ? underlyingPrice
            : optParams_.strike;

    return sign * indicator * payoutAmount;
};

const DigitalOptionParameters& DigitalEuropeanOption::parameters() const {
    return optParams_;
};

void DigitalEuropeanOption::validateInputs() const {

    if (optParams_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (optParams_.type != OptionType::Call &&
        optParams_.type != OptionType::Put)
        throw std::invalid_argument("invalid OptionType");

    if (optParams_.direction != OptionDirection::Short &&
        optParams_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");

    if (optParams_.digitalType != DigitalType::AssetOrNothing &&
        optParams_.digitalType != DigitalType::CashOrNothing)
        throw std::invalid_argument("invalid DigitalType");
};
