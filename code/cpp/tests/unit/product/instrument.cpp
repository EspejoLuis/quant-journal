#include "instrument.hpp"
#include "digitalEuropeanOption.hpp"
#include "monteCarloEngine.hpp"
#include "vanillaEuropeanOption.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Instrument returns error - null ptr engine", "[Instrument]") {
    Instrument instrument;
    REQUIRE_THROWS_AS(instrument.setPricingEngine(nullptr),
                      std::invalid_argument);
}

TEST_CASE("Instrument returns error - price with no engine", "[Instrument]") {
    Instrument instrument;
    REQUIRE_THROWS_AS(instrument.price(), std::logic_error);
}

TEST_CASE("Instrument returns error - wrong engine type", "[Instrument]") {

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    VanillaEuropeanOption opt{optionParamsCall};

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    McEngine<DigitalEuropeanOption> mcEngine{modelParams, simParams};

    opt.setPricingEngine(&mcEngine);

    REQUIRE_THROWS_AS(opt.price(), std::logic_error);
}
