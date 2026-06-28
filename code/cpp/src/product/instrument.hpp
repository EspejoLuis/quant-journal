#pragma once
#include "engine.hpp"
#include <stdexcept>

class Instrument {

  public:
    void setPricingEngine(Engine* engine);

    double price();
    virtual ~Instrument() = default;

  private:
    virtual void setArguments(Engine::Arguments* arguments) const = 0;
    Engine* engine_ = nullptr;
};