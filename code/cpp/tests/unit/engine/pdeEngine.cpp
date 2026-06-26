#include "pdeEngine.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

TEST_CASE("validateInputs returns errors. Incorrect ModelParameters",
          "[pdeEngine]") {

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

    PdeParameters pdeParams{.nSpaceSteps = 1,
                            .nTimeSteps = 2,
                            .grid = PdeGrid::Log,
                            .scheme = PdeScheme::Explicit};

    REQUIRE_THROWS_AS(PdeEngine(modelParams, pdeParams), std::invalid_argument);
}

TEST_CASE("validateInputs returns errors. Incorrect PdeParameters",
          "[pdeEngine]") {

    auto [s, t, grid, scheme] = GENERATE(table<int, int, PdeGrid, PdeScheme>({
        {-100, 1, PdeGrid::Log, PdeScheme::CrankNicolson},
        {0, 1, PdeGrid::Log, PdeScheme::CrankNicolson},
        {2, 0, PdeGrid::Uniform, PdeScheme::CrankNicolson},
        {3, -1, PdeGrid::Uniform, PdeScheme::CrankNicolson},
        {2, 1, static_cast<PdeGrid>(3), PdeScheme::CrankNicolson},
        {3, 2, PdeGrid::Uniform, static_cast<PdeScheme>(5)},
    }));

    ModelParameters modelParams{.underlyingPrice = 1,
                                .interestRate = 0.2,
                                .dividendRate = 0.3,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1};

    PdeParameters pdeParams{
        .nSpaceSteps = s, .nTimeSteps = t, .grid = grid, .scheme = scheme};

    REQUIRE_THROWS_AS(PdeEngine(modelParams, pdeParams), std::invalid_argument);
}
