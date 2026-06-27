#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

class VanillaEuropeanOption : public Instrument {

  public:
    VanillaEuropeanOption(const OptionParameters& params);

    struct Arguments : Engine::Arguments {
        OptionParameters engineParams;
        double payoff(double underlying) const;
    };

  private:
    const OptionParameters params_;

    void setArguments(Engine::Arguments*) const override;
    void validateInputs() const;
};
