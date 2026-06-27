#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

class ChooserEuropeanOption : public Instrument {

  public:
    ChooserEuropeanOption(const ChooserOptionParameters& params);

    struct Arguments : Engine::Arguments {

        ChooserOptionParameters engineParams;
    };

  private:
    const ChooserOptionParameters params_;

    void setArguments(Engine::Arguments*) const override;
    void validateInputs() const;
};
