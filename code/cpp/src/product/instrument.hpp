#pragma once
#include "engine.hpp"
#include <stdexcept>

class Instrument {

  public:
    void setPricingEngine(Engine* engine);

    double price();
    virtual ~Instrument() = default;

  private:
    virtual void setArguments(Engine::Arguments* arguments) const {
        throw std::logic_error("setArguments is not set for this instrument");
    };
    Engine* engine_ = nullptr;
};