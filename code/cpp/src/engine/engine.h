#pragma once
#include "instrument.h"

// How to price
class Engine {

  public:
    virtual double price(const Instrument& instrument) = 0;
    virtual ~Engine() = default;
};