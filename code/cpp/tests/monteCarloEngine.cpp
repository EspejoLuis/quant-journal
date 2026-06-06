#include "monteCarloEngine.h"
#include <catch2/catch_all.hpp>
#include <numeric>

TEST_CASE("simulateGbmPath returns correct. No randomness",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    std::vector<std::vector<double>> simulatedPaths =
        simulateGbmPath(modelParams, simParams);

    REQUIRE(simulatedPaths.size() == simParams.nPaths);
    REQUIRE(simulatedPaths[0].size() == simParams.nSteps + 1);

    double sum = 0.0;
    for (const std::vector<double>& path : simulatedPaths)
        sum += path.back();
    double mean = sum / simParams.nPaths;

    REQUIRE(mean == Catch::Approx(modelParams.underlyingPrice *
                                  std::exp((modelParams.interestRate -
                                            modelParams.dividendRate) *
                                           modelParams.maturityInYears))
                        .epsilon(1e-10));
}