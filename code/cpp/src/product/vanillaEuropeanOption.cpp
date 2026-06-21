#include "vanillaEuropeanOption.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

VanillaEuropeanOption::VanillaEuropeanOption(const OptionParameters& optParams)
    : optParams_(optParams) {

    validateInputs();
};

double VanillaEuropeanOption::payoff(const std::vector<double>& path) const {

    const double sign =
        optParams_.direction == OptionDirection::Long ? 1.0 : -1.0;

    const double payoff = optParams_.type == OptionType::Call
                              ? std::max(path.back() - optParams_.strike, 0.0)
                              : std::max(optParams_.strike - path.back(), 0.0);

    return sign * payoff;
}

const OptionParameters& VanillaEuropeanOption::parameters() const {
    return optParams_;
};

void VanillaEuropeanOption::validateInputs() const {

    if (optParams_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (optParams_.type != OptionType::Call &&
        optParams_.type != OptionType::Put)
        throw std::invalid_argument("invalid OptionType");

    if (optParams_.direction != OptionDirection::Short &&
        optParams_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");
};