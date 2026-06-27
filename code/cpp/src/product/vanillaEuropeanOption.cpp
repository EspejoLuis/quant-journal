#include "vanillaEuropeanOption.hpp"
#include <algorithm>
#include <stdexcept>

VanillaEuropeanOption::VanillaEuropeanOption(const OptionParameters& params)
    : params_(params) {

    validateInputs();
};

void VanillaEuropeanOption::validateInputs() const {

    if (params_.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    if (params_.type != OptionType::Call && params_.type != OptionType::Put)
        throw std::invalid_argument("invalid OptionType");

    if (params_.direction != OptionDirection::Short &&
        params_.direction != OptionDirection::Long)
        throw std::invalid_argument("invalid OptionDirection");
};

void VanillaEuropeanOption::setArguments(Engine::Arguments* args) const {
    if (!args)
        throw std::logic_error("This is not the right engine");

    VanillaEuropeanOption::Arguments* engineArgs =
        dynamic_cast<VanillaEuropeanOption::Arguments*>(args);

    if (!engineArgs)
        throw std::logic_error("This is not the right engine");

    engineArgs->engineParams = params_;
};

double VanillaEuropeanOption::Arguments::payoff(const double underlying) const {

    const double sign =
        engineParams.direction == OptionDirection::Long ? 1.0 : -1.0;

    const double payoff = engineParams.type == OptionType::Call
                              ? std::max(underlying - engineParams.strike, 0.0)
                              : std::max(engineParams.strike - underlying, 0.0);

    return sign * payoff;
}
