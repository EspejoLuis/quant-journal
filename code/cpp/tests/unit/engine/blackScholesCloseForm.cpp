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
                                .maturityInYears = T};

    REQUIRE_THROWS_AS(BsCloseForm(modelParams), std::invalid_argument);
}

TEST_CASE("price returns correct. Call (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

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
             modelParams.maturityInYears) /
        (modelParams.volatility * std::sqrt(modelParams.maturityInYears));

    double d2 =
        d1 - (modelParams.volatility * std::sqrt(modelParams.maturityInYears));

    double unsignedPrice =
        modelParams.underlyingPrice *
            std::exp(-modelParams.dividendRate * modelParams.maturityInYears) *
            normalCdf(d1) -
        strike *
            std::exp(-modelParams.interestRate * modelParams.maturityInYears) *
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

TEST_CASE("price returns correct. Put (Long/Short)",
          "[blackScholesCloseForm]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

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
             modelParams.maturityInYears) /
        (modelParams.volatility * std::sqrt(modelParams.maturityInYears));

    double d2 =
        d1 - (modelParams.volatility * std::sqrt(modelParams.maturityInYears));

    double unsignedPrice =
        -modelParams.underlyingPrice *
            std::exp(-modelParams.dividendRate * modelParams.maturityInYears) *
            normalCdf(-d1) +
        strike *
            std::exp(-modelParams.interestRate * modelParams.maturityInYears) *
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
