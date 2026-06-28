#include "chooserEuropeanOption.hpp"

ChooserEuropeanOption::ChooserEuropeanOption(
    const ChooserOptionParameters& params)
    : params_(params) {

    validateInputs();
};

void ChooserEuropeanOption::validateInputs() const {

    if (params_.maturity <= 0)
        throw std::invalid_argument(
            "choice date in years cannot be negative or zero");

    if (params_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (params_.direction != OptionDirection::Short &&
        params_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");
};

void ChooserEuropeanOption::setArguments(Engine::Arguments* args) const {

    ChooserEuropeanOption::Arguments* engineArgs =
        dynamic_cast<ChooserEuropeanOption::Arguments*>(args);

    if (!engineArgs)
        throw std::logic_error("This is not the right engine");

    engineArgs->engineParams = params_;
};
