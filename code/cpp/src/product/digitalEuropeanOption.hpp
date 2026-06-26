#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

class DigitalEuropeanOption : public Instrument {

  public:
    DigitalEuropeanOption(const DigitalOptionParameters& optParams);

    double payoff(const std::vector<double>& path) const override;

    const DigitalOptionParameters& parameters() const;

  private:
    const DigitalOptionParameters optParams_;

    void validateInputs() const;
};
