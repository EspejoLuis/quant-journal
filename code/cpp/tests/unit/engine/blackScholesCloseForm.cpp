#include "blackScholesCloseForm.hpp"
#include "mathFunctions.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

TEST_CASE("validateInputs returns errors. Incorrect ModelParameters",
          "[blackScholesCloseForm]") {

    auto [s0, r, q, sigma, T] =
        GENERATE(table<double, double, double, double, double>({
            {-100.0, 0.05, 0.02, 0.2, 1.0}, // Negative price
            {100.0, 0.05, 0.02, -0.2, 1.0}, // Negative volatility
            {100.0, 0.05, 0.02, 0.2, -1.0}, // Negative maturity in years
            {100.0, 0.05, 0.02, 0.2, 0.0},
        }));

    ModelParameters modelParams{.underlyingPrice = s0,
                                .interestRate = r,
                                .dividendRate = q,
                                .volatility = sigma,
                                .timeHorizonInYears = T};

    REQUIRE_THROWS_AS(BsCloseForm<VanillaEuropeanOption>(modelParams),
                      std::invalid_argument);
}

TEST_CASE("price returns correct. Vanilla European Option - Call (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    const double strike = 90;
    OptionParameters optionParamsCallLong{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    OptionParameters optionParamsCallShort{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
    };

    double d1 =
        (std::log(modelParams.underlyingPrice / strike) +
         (modelParams.interestRate - modelParams.dividendRate +
          1.0 / 2.0 * modelParams.volatility * modelParams.volatility) *
             modelParams.timeHorizonInYears) /
        (modelParams.volatility * std::sqrt(modelParams.timeHorizonInYears));

    double d2 = d1 - (modelParams.volatility *
                      std::sqrt(modelParams.timeHorizonInYears));

    double unsignedPrice = modelParams.underlyingPrice *
                               std::exp(-modelParams.dividendRate *
                                        modelParams.timeHorizonInYears) *
                               normalCdf(d1) -
                           strike *
                               std::exp(-modelParams.interestRate *
                                        modelParams.timeHorizonInYears) *
                               normalCdf(d2);

    BsCloseForm<VanillaEuropeanOption> bsCloseForm{modelParams};

    VanillaEuropeanOption call{optionParamsCallLong};
    call.setPricingEngine(&bsCloseForm);

    double expectedPriceLong = unsignedPrice;

    VanillaEuropeanOption put{optionParamsCallShort};
    put.setPricingEngine(&bsCloseForm);

    double expectedPriceShort = -unsignedPrice;

    CHECK(call.price() == Catch::Approx(expectedPriceLong).epsilon(0.01));
    CHECK(put.price() == Catch::Approx(expectedPriceShort).epsilon(0.01));
}

TEST_CASE("price returns correct. Vanilla European Option - Put (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    const double strike = 90;
    OptionParameters optionParamsCallLong{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
    };

    OptionParameters optionParamsCallShort{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
    };

    double d1 =
        (std::log(modelParams.underlyingPrice / strike) +
         (modelParams.interestRate - modelParams.dividendRate +
          1.0 / 2.0 * modelParams.volatility * modelParams.volatility) *
             modelParams.timeHorizonInYears) /
        (modelParams.volatility * std::sqrt(modelParams.timeHorizonInYears));

    double d2 = d1 - (modelParams.volatility *
                      std::sqrt(modelParams.timeHorizonInYears));

    double unsignedPrice = -modelParams.underlyingPrice *
                               std::exp(-modelParams.dividendRate *
                                        modelParams.timeHorizonInYears) *
                               normalCdf(-d1) +
                           strike *
                               std::exp(-modelParams.interestRate *
                                        modelParams.timeHorizonInYears) *
                               normalCdf(-d2);

    BsCloseForm<VanillaEuropeanOption> bsCloseForm{modelParams};

    VanillaEuropeanOption call{optionParamsCallLong};
    call.setPricingEngine(&bsCloseForm);

    double expectedPriceLong = unsignedPrice;

    VanillaEuropeanOption put{optionParamsCallShort};
    put.setPricingEngine(&bsCloseForm);

    double expectedPriceShort = -unsignedPrice;

    CHECK(call.price() == Catch::Approx(expectedPriceLong).epsilon(0.01));
    CHECK(put.price() == Catch::Approx(expectedPriceShort).epsilon(0.01));
}

TEST_CASE("price returns correct. Digital European Option - Call (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    const double strike = 90;
    DigitalOptionParameters optionParamsCallLongAoN{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing,
    };

    DigitalOptionParameters optionParamsCallShortAoN{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::AssetOrNothing,
    };

    const double payAmount = 100;
    DigitalOptionParameters optionParamsCallLongCoN{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing,
        .payAmount = payAmount,
    };

    DigitalOptionParameters optionParamsCallShortCoN{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::CashOrNothing,
        .payAmount = payAmount,
    };

    double d1 =
        (std::log(modelParams.underlyingPrice / strike) +
         (modelParams.interestRate - modelParams.dividendRate +
          1.0 / 2.0 * modelParams.volatility * modelParams.volatility) *
             modelParams.timeHorizonInYears) /
        (modelParams.volatility * std::sqrt(modelParams.timeHorizonInYears));

    double d2 = d1 - (modelParams.volatility *
                      std::sqrt(modelParams.timeHorizonInYears));

    double unsignedPriceAoN =
        modelParams.underlyingPrice *
        std::exp(-modelParams.dividendRate * modelParams.timeHorizonInYears) *
        normalCdf(d1);

    double unsignedPriceCoN =
        payAmount *
        std::exp(-modelParams.interestRate * modelParams.timeHorizonInYears) *
        normalCdf(d2);

    BsCloseForm<DigitalEuropeanOption> bsCloseForm{modelParams};

    DigitalEuropeanOption longCoN{optionParamsCallLongCoN};
    longCoN.setPricingEngine(&bsCloseForm);

    DigitalEuropeanOption shortCoN{optionParamsCallShortCoN};
    shortCoN.setPricingEngine(&bsCloseForm);

    DigitalEuropeanOption longAoN{optionParamsCallLongAoN};
    longAoN.setPricingEngine(&bsCloseForm);

    DigitalEuropeanOption shortAoN{optionParamsCallShortAoN};
    shortAoN.setPricingEngine(&bsCloseForm);

    CHECK(longCoN.price() == Catch::Approx(unsignedPriceCoN).epsilon(0.01));
    CHECK(shortCoN.price() == Catch::Approx(-unsignedPriceCoN).epsilon(0.01));
    CHECK(longAoN.price() == Catch::Approx(unsignedPriceAoN).epsilon(0.01));
    CHECK(shortAoN.price() == Catch::Approx(-unsignedPriceAoN).epsilon(0.01));
}

TEST_CASE("price returns correct. Digital European Option - Put (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    const double strike = 90;
    DigitalOptionParameters optionParamsPutLongAoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing,
    };

    DigitalOptionParameters optionParamsPutShortAoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::AssetOrNothing,
    };

    const double payAmount = 100;
    DigitalOptionParameters optionParamsPutLongCoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing,
        .payAmount = payAmount,
    };

    DigitalOptionParameters optionParamsPutShortCoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::CashOrNothing,
        .payAmount = payAmount,
    };

    double d1 =
        (std::log(modelParams.underlyingPrice / strike) +
         (modelParams.interestRate - modelParams.dividendRate +
          1.0 / 2.0 * modelParams.volatility * modelParams.volatility) *
             modelParams.timeHorizonInYears) /
        (modelParams.volatility * std::sqrt(modelParams.timeHorizonInYears));

    double d2 = d1 - (modelParams.volatility *
                      std::sqrt(modelParams.timeHorizonInYears));

    double unsignedPriceAoN =
        modelParams.underlyingPrice *
        std::exp(-modelParams.dividendRate * modelParams.timeHorizonInYears) *
        normalCdf(-d1);

    double unsignedPriceCoN =
        payAmount *
        std::exp(-modelParams.interestRate * modelParams.timeHorizonInYears) *
        normalCdf(-d2);

    BsCloseForm<DigitalEuropeanOption> bsCloseForm{modelParams};

    DigitalEuropeanOption longCoN{optionParamsPutLongCoN};
    longCoN.setPricingEngine(&bsCloseForm);

    DigitalEuropeanOption shortCoN{optionParamsPutShortCoN};
    shortCoN.setPricingEngine(&bsCloseForm);

    DigitalEuropeanOption longAoN{optionParamsPutLongAoN};
    longAoN.setPricingEngine(&bsCloseForm);

    DigitalEuropeanOption shortAoN{optionParamsPutShortAoN};
    shortAoN.setPricingEngine(&bsCloseForm);

    CHECK(longCoN.price() == Catch::Approx(unsignedPriceCoN).epsilon(0.01));
    CHECK(shortCoN.price() == Catch::Approx(-unsignedPriceCoN).epsilon(0.01));
    CHECK(longAoN.price() == Catch::Approx(unsignedPriceAoN).epsilon(0.01));
    CHECK(shortAoN.price() == Catch::Approx(-unsignedPriceAoN).epsilon(0.01));
}