#include "mathFunctions.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

TEST_CASE("Mean of a vector of doubles", "mean") {

    auto [expectedResults, inputs] =
        GENERATE(table<double, std::vector<double>>({
            {25.205, {100, 0.02, -0.2, 1.0}},
            {0.0, {0.0, 1, -1, 0.0}},
        }));

    REQUIRE(expectedResults == Catch::Approx(mean(inputs)).epsilon(1e-10));
}

TEST_CASE("Sample variance of a vector of doubles", "sampleVariance") {
    auto [expectedResults, inputs] =
        GENERATE(table<double, std::vector<double>>({
            {2486.6241, {100, 0.02, -0.2, 1.0}},
            {2.0 / 3.0, {0.0, 1, -1, 0.0}},
        }));

    REQUIRE(expectedResults ==
            Catch::Approx(sampleVariance(inputs)).epsilon(1e-10));
}

TEST_CASE("Sample standard deviation of a vector of doubles",
          "sampleStandardDeviation") {
    auto [expectedResults, inputs] =
        GENERATE(table<double, std::vector<double>>({
            {std::sqrt(2486.6241), {100, 0.02, -0.2, 1.0}},
            {std::sqrt(2.0 / 3.0), {0.0, 1, -1, 0.0}},
        }));

    REQUIRE(expectedResults ==
            Catch::Approx(sampleStandardDeviation(inputs)).epsilon(1e-10));
}

TEST_CASE("normalCdf") {
    auto [expectedResults, inputs] =
        GENERATE(table<double, double>({{0.5, 0.0},
                                        {0.975, 1.96},
                                        {0.99, 2.33},
                                        {1 - 0.975, -1.96},
                                        {1 - 0.99, -2.33},
                                        {1, 5.0},
                                        {0, -5.0}}));

    REQUIRE(expectedResults == Catch::Approx(normalCdf(inputs)).margin(0.001));
}

TEST_CASE("interpolationLinear") {
    auto [expectedResult, x, x0, x1, y0, y1] =
        GENERATE(table<double, double, double, double, double, double>({
            {2.0, 1.0, 1.0, 3.0, 2.0, 6.0},
            {6.0, 3.0, 1.0, 3.0, 2.0, 6.0},
        }));

    REQUIRE(
        expectedResult ==
        Catch::Approx(interpolationLinear(x, x0, x1, y0, y1)).epsilon(1e-10));
}

TEST_CASE("interpolationLinear: out of range - lower bound") {
    REQUIRE_THROWS_AS(interpolationLinear(-2, 1, 3, 4, 5), std::out_of_range);
}

TEST_CASE("interpolationLinear: out of range - upper bound") {
    REQUIRE_THROWS_AS(interpolationLinear(4, 1, 3, 4, 5), std::out_of_range);
}