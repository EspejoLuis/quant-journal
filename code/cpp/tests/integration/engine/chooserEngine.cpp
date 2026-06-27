#include "chooserEngine.hpp"
#include "blackScholesCloseForm.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Price returns correct - Long", "[ChooserEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 0.5};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 100};

    ChooserOptionParameters optionParams{
        .strike = 85,
        .maturity = 1,
        .direction = OptionDirection::Long,
    };

    ChooserEngine chooserEng{modelParams, simParams};

    ChooserEuropeanOption chooserEuropeanOption =
        ChooserEuropeanOption{optionParams};

    chooserEuropeanOption.setPricingEngine(&chooserEng);

    double mcAndBsPrice = chooserEuropeanOption.price();

    BsCloseForm<ChooserEuropeanOption> bsCloseForm{modelParams};

    chooserEuropeanOption.setPricingEngine(&bsCloseForm);

    double bsPrice = chooserEuropeanOption.price();

    REQUIRE(mcAndBsPrice == Catch::Approx(bsPrice).epsilon(0.01));
};

TEST_CASE("Price returns correct - Short", "[ChooserEngine]") {

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

    ChooserEngine chooserEng{modelParams, simParams};

    ChooserEuropeanOption chooserEuropeanOption =
        ChooserEuropeanOption{optionParams};

    chooserEuropeanOption.setPricingEngine(&chooserEng);

    double mcAndBsPrice = chooserEuropeanOption.price();

    BsCloseForm<ChooserEuropeanOption> bsCloseForm{modelParams};
    chooserEuropeanOption.setPricingEngine(&bsCloseForm);

    double bsPrice = chooserEuropeanOption.price();

    REQUIRE(mcAndBsPrice == Catch::Approx(bsPrice).epsilon(0.01));
};