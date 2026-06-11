#include "vanillaEuropeanOption.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

VanillaEuropeanOption::VanillaEuropeanOption(const OptionParameters& optParams)
    : optParams_(optParams) {

    if (optParams_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (optParams_.type != OptionType::Call &&
        optParams_.type != OptionType::Put)
        throw std::invalid_argument("invalid OptionType");

    if (optParams_.direction != OptionDirection::Short &&
        optParams_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");
};

double VanillaEuropeanOption::payoff(const std::vector<double>& path) const {

    double payoff = 0.0;

    switch (optParams_.type) {
    case OptionType::Call:
        payoff += std::max(path.back() - optParams_.strike, 0.0);
        break;
    case OptionType::Put:
        payoff += std::max(optParams_.strike - path.back(), 0.0);
        break;
    };

    double sign = 0.0;

    switch (optParams_.direction) {
    case OptionDirection::Long:
        sign = +1.0;
        break;
    case OptionDirection::Short:
        sign = -1.0;
        break;
    }

    return sign * payoff;
}

const OptionParameters& VanillaEuropeanOption::parameters() const {
    return optParams_;
};
