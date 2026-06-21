#include "monteCarloEngine.hpp"
#include "blackScholesCloseForm.hpp"
#include "digitalEuropeanOption.hpp"
#include "mathFunctions.hpp"
#include "vanillaEuropeanOption.hpp"
#include <catch2/catch_all.hpp>
#include <numeric>

TEST_CASE("validateInputs returns error. Incorrect SimulationParameters ",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

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

    SimulationParameters simParamsVarRed{
        .nPaths = 1,
        .nSteps = 1,
        .varianceReduction = static_cast<VarianceReduction>(10)};

    REQUIRE_THROWS_AS(McEngine(modelParams, simParamsVarRed),
                      std::invalid_argument);
}

TEST_CASE("validateInputs returns error. Incorrect ModelParameters ",
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
                                .timeHorizonInYears = T};

    SimulationParameters simParams{.nPaths = 10, .nSteps = 10};

    REQUIRE_THROWS_AS(McEngine(modelParams, simParams), std::invalid_argument);
}

TEST_CASE("price returns correct. Fixed seed. Zero volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    OptionParameters optionParamsCall{
        .strike = 87,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};

    double deterministicDiscountFactor =
        std::exp(-modelParams.interestRate * modelParams.timeHorizonInYears);

    double payoffAtMaturity =
        modelParams.underlyingPrice *
            std::exp((modelParams.interestRate - modelParams.dividendRate) *
                     modelParams.timeHorizonInYears) -
        optionParamsCall.strike;

    REQUIRE(mcEng.price(vanillaOption) ==
            Catch::Approx(payoffAtMaturity * deterministicDiscountFactor)
                .epsilon(1e-10));
}

TEST_CASE("simulateGbmPath returns price within 1 percent vs Black Scholes"
          "Vanilla European Option - Fixed seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{
        .nPaths = 100'000, .nSteps = 100, .seed = 11};

    OptionParameters optionParamsCall{
        .strike = 87,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};

    double mcPrice = mcEng.price(vanillaOption);

    BsCloseForm bsCloseForm = BsCloseForm{modelParams};

    double bsPrice = bsCloseForm.price(vanillaOption);

    REQUIRE(mcPrice ==
            Catch::Approx(bsPrice).epsilon(0.01)); // 1% relative difference
}

TEST_CASE("SimulateGbmPath returns price within 1 percent vs Black Scholes"
          "Vanilla European Option - No Seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 100};

    OptionParameters optionParamsCall{
        .strike = 85,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};

    double mcPrice = mcEng.price(vanillaOption);

    BsCloseForm bsCloseForm = BsCloseForm{modelParams};

    double bsPrice = bsCloseForm.price(vanillaOption);

    REQUIRE(mcPrice ==
            Catch::Approx(bsPrice).epsilon(0.01)); // 1% relative difference
}

TEST_CASE("SimulateGbmPath with/without Antithetic variates - variance "
          "reduction . No Seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 1'000, .nSteps = 10};
    SimulationParameters simParamsVarRed{.nPaths = 1'000,
                                         .nSteps = 10,
                                         .varianceReduction =
                                             VarianceReduction::Antithetic};

    OptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEng = McEngine{modelParams, simParams};
    McEngine mcEngVarRed = McEngine{modelParams, simParamsVarRed};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};

    const int runs = 30;
    std::vector<double> mcPrices = std::vector<double>(runs);
    std::vector<double> mcPricesVarianceReduction = std::vector<double>(runs);

    for (int i = 0; i < runs; i++) {
        mcPrices[i] = mcEng.price(vanillaOption);
        mcPricesVarianceReduction[i] = mcEngVarRed.price(vanillaOption);
    }

    REQUIRE(sampleStandardDeviation(mcPrices) >
            sampleStandardDeviation(mcPricesVarianceReduction));
}

TEST_CASE("SimulateGbmPath with Antithetic Variates returns price within 1 "
          "percent vs Black Scholes"
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParamsVarRed{.nPaths = 100'000,
                                         .nSteps = 10,
                                         .seed = 19,
                                         .varianceReduction =
                                             VarianceReduction::Antithetic};

    OptionParameters optionParamsCall{
        .strike = 95,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    McEngine mcEngVarRed = McEngine{modelParams, simParamsVarRed};
    BsCloseForm bsCloseForm = BsCloseForm{modelParams};

    VanillaEuropeanOption vanillaOption =
        VanillaEuropeanOption{optionParamsCall};

    REQUIRE(mcEngVarRed.price(vanillaOption) ==
            Catch::Approx(bsCloseForm.price(vanillaOption)).epsilon(0.01));
}

TEST_CASE("simulateGbmPath returns price within 1 percent vs Black Scholes"
          "Digital European Option - Fixed seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{
        .nPaths = 100'000, .nSteps = 100, .seed = 11};

    DigitalOptionParameters optionParamsCallCoN{
        .strike = 87,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing};

    DigitalOptionParameters optionParamsCallAoN{
        .strike = 87,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing};

    McEngine mcEng = McEngine{modelParams, simParams};

    DigitalEuropeanOption vanillaOptionCoN =
        DigitalEuropeanOption{optionParamsCallCoN};
    DigitalEuropeanOption vanillaOptionAoN =
        DigitalEuropeanOption{optionParamsCallAoN};

    double mcPriceAoN = mcEng.price(vanillaOptionAoN);
    double mcPriceCoN = mcEng.price(vanillaOptionCoN);

    BsCloseForm bsCloseForm = BsCloseForm{modelParams};

    double bsPriceAoN = bsCloseForm.price(vanillaOptionAoN);
    double bsPriceCoN = bsCloseForm.price(vanillaOptionCoN);

    REQUIRE(mcPriceAoN ==
            Catch::Approx(bsPriceAoN).epsilon(0.01)); // 1% relative difference
    REQUIRE(mcPriceCoN ==
            Catch::Approx(bsPriceCoN).epsilon(0.01)); // 1% relative difference
}

TEST_CASE("SimulateGbmPath returns price within 1 percent vs Black Scholes"
          "Digital European Option - No Seed. Positive volatility",
          "[monteCarloEngine]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.2,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 100};

    DigitalOptionParameters optionParamsCallCoN{
        .strike = 87,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing};

    DigitalOptionParameters optionParamsCallAoN{
        .strike = 87,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing};

    McEngine mcEng = McEngine{modelParams, simParams};

    DigitalEuropeanOption vanillaOptionCoN =
        DigitalEuropeanOption{optionParamsCallCoN};
    DigitalEuropeanOption vanillaOptionAoN =
        DigitalEuropeanOption{optionParamsCallAoN};

    double mcPriceAoN = mcEng.price(vanillaOptionAoN);
    double mcPriceCoN = mcEng.price(vanillaOptionCoN);

    BsCloseForm bsCloseForm = BsCloseForm{modelParams};

    double bsPriceAoN = bsCloseForm.price(vanillaOptionAoN);
    double bsPriceCoN = bsCloseForm.price(vanillaOptionCoN);

    REQUIRE(mcPriceAoN ==
            Catch::Approx(bsPriceAoN).epsilon(0.01)); // 1% relative difference
    REQUIRE(mcPriceCoN ==
            Catch::Approx(bsPriceCoN).epsilon(0.01)); // 1% relative difference
}
