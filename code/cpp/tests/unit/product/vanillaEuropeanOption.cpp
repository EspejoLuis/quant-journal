#include "vanillaEuropeanOption.h"
#include "monteCarloEngine.h"
#include <catch2/catch_all.hpp>
#include <numeric>
#include <stdexcept>

TEST_CASE("VanillaEuropeanOption returns correct. Long Put-Call parity",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 86;
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

    McEngine mcEngine = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaCall = VanillaEuropeanOption{optionParamsCall};
    VanillaEuropeanOption vanillaPut = VanillaEuropeanOption{optionParamsPut};

    double callPrice = mcEngine.price(vanillaCall);

    double putPrice = mcEngine.price(vanillaPut);

    REQUIRE(callPrice - putPrice ==
            Catch::Approx(modelParams.underlyingPrice *
                              std::exp(-modelParams.dividendRate *
                                       modelParams.maturityInYears) -
                          strike * std::exp(-modelParams.interestRate *
                                            modelParams.maturityInYears))
                .epsilon(1e-10));
}

TEST_CASE("VanillaEuropeanOption returns correct. Short Put-Call parity.",
          "[vanillaEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 85;
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

    McEngine mcEngine = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaCall = VanillaEuropeanOption{optionParamsCall};
    VanillaEuropeanOption vanillaPut = VanillaEuropeanOption{optionParamsPut};

    double callPrice = mcEngine.price(vanillaCall);

    double putPrice = mcEngine.price(vanillaPut);

    REQUIRE(callPrice - putPrice ==
            -Catch::Approx(modelParams.underlyingPrice *
                               std::exp(-modelParams.dividendRate *
                                        modelParams.maturityInYears) -
                           strike * std::exp(-modelParams.interestRate *
                                             modelParams.maturityInYears))
                 .epsilon(1e-10));
}

TEST_CASE("VanillaEuropeanOption returns error. invalid OptionType.",
          "[vanillaEuropeanOption]") {

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = static_cast<OptionType>(99),
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(VanillaEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("VanillaEuropeanOption returns error. invalid OptionDirection.",
          "[vanillaEuropeanOption]") {

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = static_cast<OptionDirection>(99),
    };

    REQUIRE_THROWS_AS(VanillaEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("VanillaEuropeanOption returns error. Negative Strike.",
          "[vanillaEuropeanOption]") {

    OptionParameters optionParamsCall{
        .strike = -10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(VanillaEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}