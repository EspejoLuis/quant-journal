#include "blackScholesCloseForm.h"
#include "mathFunctions.h"
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

    REQUIRE_THROWS_AS(BsCloseForm(modelParams), std::invalid_argument);
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

    double bsPriceLong = BsCloseForm{modelParams}.price(
        VanillaEuropeanOption{optionParamsCallLong});
    double expectedPriceLong = unsignedPrice;

    double bsPriceShort = BsCloseForm{modelParams}.price(
        VanillaEuropeanOption{optionParamsCallShort});
    double expectedPriceShort = -unsignedPrice;

    CHECK(bsPriceLong == Catch::Approx(expectedPriceLong).epsilon(0.01));
    CHECK(bsPriceShort == Catch::Approx(expectedPriceShort).epsilon(0.01));
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

    double bsPriceLong = BsCloseForm{modelParams}.price(
        VanillaEuropeanOption{optionParamsCallLong});
    double expectedPriceLong = unsignedPrice;

    double bsPriceShort = BsCloseForm{modelParams}.price(
        VanillaEuropeanOption{optionParamsCallShort});
    double expectedPriceShort = -unsignedPrice;

    CHECK(bsPriceLong == Catch::Approx(expectedPriceLong).epsilon(0.01));
    CHECK(bsPriceShort == Catch::Approx(expectedPriceShort).epsilon(0.01));
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

    DigitalOptionParameters optionParamsCallLongCoN{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing,
    };

    DigitalOptionParameters optionParamsCallShortCoN{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::CashOrNothing,
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
        strike *
        std::exp(-modelParams.interestRate * modelParams.timeHorizonInYears) *
        normalCdf(d2);

    double bsPriceLongCoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallLongCoN});
    double bsPriceShortCoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallShortCoN});
    double bsPriceLongAoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallLongAoN});
    double bsPriceShortAoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallShortAoN});

    CHECK(bsPriceLongCoN == Catch::Approx(unsignedPriceCoN).epsilon(0.01));
    CHECK(bsPriceShortCoN == Catch::Approx(-unsignedPriceCoN).epsilon(0.01));
    CHECK(bsPriceLongAoN == Catch::Approx(unsignedPriceAoN).epsilon(0.01));
    CHECK(bsPriceShortAoN == Catch::Approx(-unsignedPriceAoN).epsilon(0.01));
}

TEST_CASE("price returns correct. Digital European Option - Put (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    const double strike = 90;
    DigitalOptionParameters optionParamsCallLongAoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing,
    };

    DigitalOptionParameters optionParamsCallShortAoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::AssetOrNothing,
    };

    DigitalOptionParameters optionParamsCallLongCoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing,
    };

    DigitalOptionParameters optionParamsCallShortCoN{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::CashOrNothing,
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
        strike *
        std::exp(-modelParams.interestRate * modelParams.timeHorizonInYears) *
        normalCdf(-d2);

    double bsPriceLongCoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallLongCoN});
    double bsPriceShortCoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallShortCoN});
    double bsPriceLongAoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallLongAoN});
    double bsPriceShortAoN = BsCloseForm{modelParams}.price(
        DigitalEuropeanOption{optionParamsCallShortAoN});

    CHECK(bsPriceLongCoN == Catch::Approx(unsignedPriceCoN).epsilon(0.01));
    CHECK(bsPriceShortCoN == Catch::Approx(-unsignedPriceCoN).epsilon(0.01));
    CHECK(bsPriceLongAoN == Catch::Approx(unsignedPriceAoN).epsilon(0.01));
    CHECK(bsPriceShortAoN == Catch::Approx(-unsignedPriceAoN).epsilon(0.01));
}