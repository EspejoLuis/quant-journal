#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

struct ChooserOptionParameters {
    double strike;
    double choiceDateInYears; // Date at which max(C,P)
    OptionDirection direction;
};

class ChooserEuropeanOption : public Instrument {

  public:
    ChooserEuropeanOption(const ChooserOptionParameters& optParams);

    const ChooserOptionParameters& parameters() const;

  private:
    const ChooserOptionParameters optParams_;

    void validateInputs() const;
};
