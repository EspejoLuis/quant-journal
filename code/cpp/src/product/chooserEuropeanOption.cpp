#include "chooserEuropeanOption.hpp"

ChooserEuropeanOption::ChooserEuropeanOption(
    const ChooserOptionParameters& optParams)
    : optParams_(optParams) {

    validateInputs();
};

const ChooserOptionParameters& ChooserEuropeanOption::parameters() const {
    return optParams_;
};

void ChooserEuropeanOption::validateInputs() const {

    if (optParams_.maturity <= 0)
        throw std::invalid_argument(
            "choice date in years cannot be negative or zero");

    if (optParams_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (optParams_.direction != OptionDirection::Short &&
        optParams_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");
};
