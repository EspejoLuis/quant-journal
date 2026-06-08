#include "vanillaEuropeanOption.h"
#include <catch2/catch_all.hpp>
#include <numeric>
#include <stdexcept>

TEST_CASE("priceEuropeanVanillaOption returns correct. Long Put-Call parity",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 10;
    OptionParameters optionParamsCall{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    OptionParameters optionParamsPut{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
    };

    double putPrice =
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsPut);

    double callPrice =
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsCall);

    REQUIRE(callPrice - putPrice ==
            Catch::Approx(modelParams.underlyingPrice *
                              std::exp(-modelParams.dividendRate *
                                       modelParams.maturityInYears) -
                          strike * std::exp(-modelParams.interestRate *
                                            modelParams.maturityInYears))
                .epsilon(1e-10));
}

TEST_CASE("priceEuropeanVanillaOption returns error. Negative Strike.",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    OptionParameters optionParamsCall{
        .strike = -10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsCall),
        std::invalid_argument);
}

TEST_CASE("priceEuropeanVanillaOption returns correct. Short Put-Call parity.",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 10;
    OptionParameters optionParamsCall{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
    };

    OptionParameters optionParamsPut{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
    };

    double putPrice =
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsPut);

    double callPrice =
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsCall);

    REQUIRE(callPrice - putPrice ==
            -Catch::Approx(modelParams.underlyingPrice *
                               std::exp(-modelParams.dividendRate *
                                        modelParams.maturityInYears) -
                           strike * std::exp(-modelParams.interestRate *
                                             modelParams.maturityInYears))
                 .epsilon(1e-10));
}

TEST_CASE("priceEuropeanVanillaOption returns error. invalid OptionType.",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = static_cast<OptionType>(99),
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsCall),
        std::invalid_argument);
}

TEST_CASE("priceEuropeanVanillaOption returns error. invalid OptionDirection.",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = static_cast<OptionDirection>(99),
    };

    REQUIRE_THROWS_AS(
        priceEuropeanVanillaOption(modelParams, simParams, optionParamsCall),
        std::invalid_argument);
}