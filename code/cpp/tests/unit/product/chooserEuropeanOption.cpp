#include "chooserEuropeanOption.hpp"
#include "monteCarloEngine.hpp"
#include "vanillaEuropeanOption.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

TEST_CASE("ChooserEuropeanOption returns error. invalid Maturity.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = 10,
        .maturity = -1,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(ChooserEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("ChooserEuropeanOption returns error. invalid OptionDirection.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = 10,
        .maturity = 1,
        .direction = static_cast<OptionDirection>(99),
    };

    REQUIRE_THROWS_AS(ChooserEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("ChooserEuropeanOption returns error. Negative Strike.",
          "[ChooserEuropeanOption]") {

    ChooserOptionParameters optionParamsCall{
        .strike = -10,
        .maturity = 1,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(ChooserEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("DigitalEuropeanOption returns error - wrong engine type",
          "[DigitalEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 0.5};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 100};

    ChooserOptionParameters optionParams{
        .strike = 85,
        .maturity = 1,
        .direction = OptionDirection::Short,
    };

    McEngine<VanillaEuropeanOption> mcEngine{modelParams, simParams};

    ChooserEuropeanOption chooserEuropeanOption =
        ChooserEuropeanOption{optionParams};

    chooserEuropeanOption.setPricingEngine(&mcEngine);

    REQUIRE_THROWS_AS(chooserEuropeanOption.price(), std::logic_error);
}