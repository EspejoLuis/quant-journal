#pragma once
#include <stdexcept>
#include <vector>

// What to price
class Instrument {

  public:
    virtual double payoff(const std::vector<double>& path) const {
        throw std::logic_error("payoff() not implemented for this instrument");
    };
    virtual ~Instrument() = default;
};