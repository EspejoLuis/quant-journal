#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

class VanillaEuropeanOption : public Instrument {

  public:
    VanillaEuropeanOption(const OptionParameters& optParams);

    double payoff(const std::vector<double>& path) const override;

    const OptionParameters& parameters() const;

  private:
    const OptionParameters optParams_;

    void validateInputs() const;
};
