#pragma once

// What to price
class Instrument {

  public:
    virtual double payoff() = 0;
    virtual ~Instrument() = default;
};