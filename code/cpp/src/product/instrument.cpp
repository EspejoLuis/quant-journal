#include "instrument.hpp"

void Instrument::setPricingEngine(Engine* engine) {

    if (!engine)
        throw std::invalid_argument("engine has not been defined");

    engine_ = engine;
};

double Instrument::price() {

    if (!engine_)
        throw std::logic_error("engine has not been defined");

    Engine::Arguments* args = engine_->getArguments();
    setArguments(args);
    engine_->calculate();
    return engine_->getResult();
};
