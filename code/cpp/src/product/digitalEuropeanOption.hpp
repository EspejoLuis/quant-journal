#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

enum class DigitalType { CashOrNothing, AssetOrNothing };

struct DigitalOptionParameters {
    double strike;
    OptionType type;
    OptionDirection direction;
    DigitalType digitalType;
};

class DigitalEuropeanOption : public Instrument {

  public:
    DigitalEuropeanOption(const DigitalOptionParameters& optParams);

    double payoff(const std::vector<double>& path) const override;

    const DigitalOptionParameters& parameters() const;

  private:
    const DigitalOptionParameters optParams_;

    void validateInputs() const;
};
