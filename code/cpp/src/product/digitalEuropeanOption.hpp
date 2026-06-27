#pragma once
#include "instrument.hpp"
#include "optionParameters.hpp"

class DigitalEuropeanOption : public Instrument {

  public:
    DigitalEuropeanOption(const DigitalOptionParameters& optParams);

    struct Arguments : Engine::Arguments {

        DigitalOptionParameters engineParams;
        double payoff(const double underlying) const;
    };

  private:
    const DigitalOptionParameters params_;

    void setArguments(Engine::Arguments*) const override;
    void validateInputs() const;
};
