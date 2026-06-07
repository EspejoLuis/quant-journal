#include "monteCarloEngine.h"
#include <catch2/catch_all.hpp>
#include <numeric>

TEST_CASE("simulateGbmPath returns correct. Fixed seed. Zero volatility",
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

    for (const std::vector<double>& path : simulatedPaths)
        REQUIRE(path.front() == modelParams.underlyingPrice);

    for (const std::vector<double>& path : simulatedPaths)
        for (const double& step : path)
            REQUIRE(step >= 0);
}

TEST_CASE("simulateGbmPath returns correct. Fixed seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{
        .nPaths = 100'000, .nSteps = 1000, .seed = 11};

    std::vector<std::vector<double>> simulatedPaths =
        simulateGbmPath(modelParams, simParams);

    // Dimension Check
    REQUIRE(simulatedPaths.size() == simParams.nPaths);
    REQUIRE(simulatedPaths[0].size() == simParams.nSteps + 1);

    double sum = 0.0;
    for (const std::vector<double>& path : simulatedPaths)
        sum += path.back();

    double expectedValueSimulated = sum / simParams.nPaths;

    double expectedValueExpected =
        modelParams.underlyingPrice *
        std::exp((modelParams.interestRate - modelParams.dividendRate) *
                 modelParams.maturityInYears);

    double standardDeviationSimulated =
        expectedValueExpected *
        std::sqrt(std::exp(modelParams.volatility * modelParams.volatility *
                           modelParams.maturityInYears) -
                  1);

    double standardError =
        standardDeviationSimulated / std::sqrt(simParams.nPaths);

    // Output check with fixed seed
    REQUIRE(expectedValueSimulated ==
            Catch::Approx(expectedValueExpected).margin(3 * standardError));

    for (const std::vector<double>& path : simulatedPaths)
        REQUIRE(path.front() == modelParams.underlyingPrice);

    for (const std::vector<double>& path : simulatedPaths)
        for (const double& step : path)
            REQUIRE(step >= 0);
}

TEST_CASE("simulateGbmPath returns correct. No Seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .maturityInYears = 1.0};

    SimulationParameters simParams{.nPaths = 1'000, .nSteps = 10};

    std::vector<std::vector<double>> simulatedPaths =
        simulateGbmPath(modelParams, simParams);

    // Dimension Check
    REQUIRE(simulatedPaths.size() == simParams.nPaths);
    REQUIRE(simulatedPaths[0].size() == simParams.nSteps + 1);

    for (const std::vector<double>& path : simulatedPaths)
        REQUIRE(path.front() == modelParams.underlyingPrice);

    for (const std::vector<double>& path : simulatedPaths)
        for (const double& step : path)
            REQUIRE(step >= 0);
}
