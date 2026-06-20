#include "digitalEuropeanOption.h"
#include "monteCarloEngine.h"
#include <catch2/catch_all.hpp>
#include <numeric>
#include <stdexcept>

TEST_CASE("DigitalEuropeanOption returns correct. Long Put-Call parity - Asset "
          "or nothing",
          "[digitalEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 98;
    DigitalOptionParameters optionParamsCall{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing,
    };

    DigitalOptionParameters optionParamsPut{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::AssetOrNothing,
    };

    McEngine mcEngine = McEngine{modelParams, simParams};

    DigitalEuropeanOption digitalCall = DigitalEuropeanOption{optionParamsCall};
    DigitalEuropeanOption digitalPut = DigitalEuropeanOption{optionParamsPut};

    double callPrice = mcEngine.price(digitalCall);
    double putPrice = mcEngine.price(digitalPut);
    /*
    Put-Call parity: only P + C has same value regardless of state
    Asset or nothing:
        - S_T >= K:
            - Call: S_T
            - Put: 0
        - S_T < K:
            - Call: 0
            - Put: S_T
    */

    REQUIRE(callPrice + putPrice ==
            Catch::Approx(modelParams.underlyingPrice *
                          std::exp(-modelParams.dividendRate *
                                   modelParams.timeHorizonInYears))
                .epsilon(1e-10));
}

TEST_CASE(
    "DigitalEuropeanOption returns correct. Short Put-Call parity - Asset "
    "or nothing",
    "[digitalEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 98;
    DigitalOptionParameters optionParamsCall{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::AssetOrNothing,
    };

    DigitalOptionParameters optionParamsPut{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::AssetOrNothing,
    };

    McEngine mcEngine = McEngine{modelParams, simParams};

    DigitalEuropeanOption digitalCall = DigitalEuropeanOption{optionParamsCall};
    DigitalEuropeanOption digitalPut = DigitalEuropeanOption{optionParamsPut};

    double callPrice = mcEngine.price(digitalCall);
    double putPrice = mcEngine.price(digitalPut);
    /*
    Put-Call parity: only P + C has same value regardless of state
    Asset or nothing:
        - S_T >= K:
            - Call: S_T
            - Put: 0
        - S_T < K:
            - Call: 0
            - Put: S_T
    */

    REQUIRE(callPrice + putPrice ==
            -Catch::Approx(modelParams.underlyingPrice *
                           std::exp(-modelParams.dividendRate *
                                    modelParams.timeHorizonInYears))
                 .epsilon(1e-10));
}

TEST_CASE("DigitalEuropeanOption returns correct. Long Put-Call parity - Cash "
          "or nothing",
          "[digitalEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 98;
    DigitalOptionParameters optionParamsCall{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing,
    };

    DigitalOptionParameters optionParamsPut{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Long,
        .digitalType = DigitalType::CashOrNothing,
    };

    McEngine mcEngine = McEngine{modelParams, simParams};

    DigitalEuropeanOption digitalCall = DigitalEuropeanOption{optionParamsCall};
    DigitalEuropeanOption digitalPut = DigitalEuropeanOption{optionParamsPut};

    double callPrice = mcEngine.price(digitalCall);
    double putPrice = mcEngine.price(digitalPut);
    /*
    Put-Call parity: only P + C has same value regardless of state
    Cash or nothing:
        - S_T >= K:
            - Call: K
            - Put: 0
        - S_T < K:
            - Call: 0
            - Put: K
    */

    REQUIRE(callPrice + putPrice ==
            Catch::Approx(strike * std::exp(-modelParams.interestRate *
                                            modelParams.timeHorizonInYears))
                .epsilon(1e-10));
}

TEST_CASE("DigitalEuropeanOption returns correct. Short Put-Call parity - Cash "
          "or nothing",
          "[digitalEuropeanOption]") {

    ModelParameters modelParams{.underlyingPrice = 100.0,
                                .interestRate = 0.02,
                                .dividendRate = 0.05,
                                .volatility = 0.0,
                                .timeHorizonInYears = 1.0};

    SimulationParameters simParams{.nPaths = 100'000, .nSteps = 10, .seed = 42};

    double strike = 98;
    DigitalOptionParameters optionParamsCall{
        .strike = strike,
        .type = OptionType::Call,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::CashOrNothing,
    };

    DigitalOptionParameters optionParamsPut{
        .strike = strike,
        .type = OptionType::Put,
        .direction = OptionDirection::Short,
        .digitalType = DigitalType::CashOrNothing,
    };

    McEngine mcEngine = McEngine{modelParams, simParams};

    DigitalEuropeanOption digitalCall = DigitalEuropeanOption{optionParamsCall};
    DigitalEuropeanOption digitalPut = DigitalEuropeanOption{optionParamsPut};

    double callPrice = mcEngine.price(digitalCall);
    double putPrice = mcEngine.price(digitalPut);
    /*
    Put-Call parity: only P + C has same value regardless of state
    Asset or nothing:
        - S_T >= K:
            - Call: K
            - Put: 0
        - S_T < K:
            - Call: 0
            - Put: K
    */

    REQUIRE(callPrice + putPrice ==
            -Catch::Approx(strike * std::exp(-modelParams.interestRate *
                                             modelParams.timeHorizonInYears))
                 .epsilon(1e-10));
}

TEST_CASE("DigitalEuropeanOption returns error. invalid OptionType.",
          "[digitalEuropeanOption]") {

    DigitalOptionParameters optionParamsCall{
        .strike = 10,
        .type = static_cast<OptionType>(99),
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(DigitalEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("DigitalEuropeanOption returns error. invalid OptionDirection.",
          "[digitalEuropeanOption]") {

    DigitalOptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = static_cast<OptionDirection>(99),
    };

    REQUIRE_THROWS_AS(DigitalEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("DigitalEuropeanOption returns error. invalid DigitalType.",
          "[digitalEuropeanOption]") {

    DigitalOptionParameters optionParamsCall{
        .strike = 10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
        .digitalType = static_cast<DigitalType>(4),
    };

    REQUIRE_THROWS_AS(DigitalEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}

TEST_CASE("DigitalEuropeanOption returns error. Negative Strike.",
          "[digitalEuropeanOption]") {

    DigitalOptionParameters optionParamsCall{
        .strike = -10,
        .type = OptionType::Call,
        .direction = OptionDirection::Long,
    };

    REQUIRE_THROWS_AS(DigitalEuropeanOption{optionParamsCall},
                      std::invalid_argument);
}