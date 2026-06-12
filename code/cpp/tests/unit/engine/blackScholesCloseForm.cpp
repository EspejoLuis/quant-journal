#include "blackScholesCloseForm.h"
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