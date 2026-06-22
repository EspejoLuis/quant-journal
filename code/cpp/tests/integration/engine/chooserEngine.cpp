#include "chooserEngine.hpp"
#include "blackScholesCloseForm.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Price returns correct", "[ChooserEngine]") {

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

    ChooserEngine chooserEng = ChooserEngine{modelParams, simParams};

    ChooserEuropeanOption chooserEuropeanOption =
        ChooserEuropeanOption{optionParams};

    double mcAndBsPrice = chooserEng.price(chooserEuropeanOption);

    BsCloseForm bsCloseForm = BsCloseForm{modelParams};

    double bsPrice = bsCloseForm.price(chooserEuropeanOption);

    REQUIRE(mcAndBsPrice == Catch::Approx(bsPrice).epsilon(0.01));
};