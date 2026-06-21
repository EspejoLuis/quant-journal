#pragma once
#include "instrument.hpp"

// How to price
class Engine {

  public:
    virtual double price(const Instrument& instrument) = 0;
    virtual ~Engine() = default;
};