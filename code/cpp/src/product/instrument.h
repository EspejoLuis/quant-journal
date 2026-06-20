#pragma once
#include <vector>
// What to price
class Instrument {

  public:
    double payoff(const std::vector<double>& path) const = 0;
    virtual ~Instrument() = default;
};