#pragma once
#include "instrument.h"

enum class OptionType { Call, Put };
enum class OptionDirection { Long, Short };

struct OptionParameters {
    double strike;
    OptionType type;
    OptionDirection direction;
};

class VanillaEuropeanOption : public Instrument {

  public:
    VanillaEuropeanOption(const OptionParameters& optParams);

    double payoff(const std::vector<double>& path) const override;

    const OptionParameters& parameters() const;

  private:
    const OptionParameters optParams_;

    void validateInputs() const;
};
