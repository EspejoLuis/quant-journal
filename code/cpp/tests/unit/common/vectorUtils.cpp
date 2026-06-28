#include "vectorUtils.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>
#include <vector>

TEST_CASE(
    "upperBracketIndexInSorted throws out_of_range for out-of-bounds target",
    "[vectorUtils]") {

    const std::vector<double> grid{1.0, 2.0, 3.0, 4.0, 5.0};

    const double target = GENERATE(values<double>({0.5, 6.0}));

    REQUIRE_THROWS_AS(upperBracketIndexInSorted(grid, target),
                      std::out_of_range);
}

TEST_CASE(
    "upperBracketIndexInSorted returns correct index for in-bounds target",
    "[vectorUtils]") {

    auto [expectedValue, grid] = GENERATE(table<double, std::vector<double>>({

        {2, {1.0, 1.5, 3.0, 5.0, 7.0}},
        {2, {1.0, 1.5, 5.0, 6.0, 7.0}},
    }));

    REQUIRE(expectedValue ==
            Catch::Approx(upperBracketIndexInSorted(grid, 3)).epsilon(1e-10));
}

TEST_CASE("upperBracketIndexInSorted returns error - invalid_argument",
          "[vectorUtils]") {

    auto [expectedValue, grid] = GENERATE(table<double, std::vector<double>>({

        {2, {1.0, 1.5, 100.0, 6.0, 7.0}},
        {2, {7.0, 6.0, 1.5, 7.0, 100.0}},
    }));

    REQUIRE_THROWS_AS(upperBracketIndexInSorted(grid, 3),
                      std::invalid_argument);
}