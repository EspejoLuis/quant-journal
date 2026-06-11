#include "monteCarloEngine.h"
#include "vanillaEuropeanOption.h"
#include <catch2/catch_all.hpp>
#include <numeric>

TEST_CASE("validateInputs returns error. Incorrec SimulationParameters ",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

    auto [nP, nS] = GENERATE(table<int, int>({
        {-100, 5},
        {-100, -5},
        {100, -5},
        {0, 0},
        {23, 0},
        {1, 0},
    }));

    SimulationParameters simParams{.nPaths = nP, .nSteps = nS};

    REQUIRE_THROWS_AS(McEngine(modelParams, simParams), std::invalid_argument);
}

TEST_CASE("validateInputs returns error. Incorrec ModelParameters ",
          "[monteCarloEngine]") {

    auto [s0, r, q, sigma, T] =
        GENERATE(table<double, double, double, double, double>({
            {-100.0, 0.05, 0.02, 0.2, 1.0},
            {100.0, 0.05, 0.02, -0.2, 1.0},
            {100.0, 0.05, 0.02, 0.2, -1.0},
            {100.0, 0.05, 0.02, 0.2, 0.0},
        }));

    ModelParameters modelParams{.underlyingPrice = s0,
                                .interestRate = r,
                                .dividendRate = q,
                                .volatility = sigma,
                                .maturityInYears = T};

    SimulationParameters simParams{.nPaths = 10, .nSteps = 10};

    REQUIRE_THROWS_AS(McEngine(modelParams, simParams), std::invalid_argument);
}

TEST_CASE("price returns correct. Fixed seed. Zero volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};

    double deterministicDiscountFactor =
        std::exp(-modelParams.interestRate * modelParams.maturityInYears);

    double payoffAtMaturity =
        modelParams.underlyingPrice *
            std::exp((modelParams.interestRate - modelParams.dividendRate) *
                     modelParams.maturityInYears) -
        optionParamsCall.strike;

    REQUIRE(mcEng.price(vanillaOption) ==
            Catch::Approx(payoffAtMaturity * deterministicDiscountFactor)
                .epsilon(1e-10));
}

TEST_CASE("simulateGbmPath returns correct. Fixed seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{
        .nPaths = 100'000, .nSteps = 100, .seed = 11};

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};
}

TEST_CASE("simulateGbmPath returns correct. No Seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 1'000, .nSteps = 10};

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};
}
