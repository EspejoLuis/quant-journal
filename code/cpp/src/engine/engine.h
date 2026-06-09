#pragma once
#include "instrument.h"

// How to price
class Engine {

  public:
    virtual double price(const Instrument&) = 0;
    virtual ~Engine() = default;
};