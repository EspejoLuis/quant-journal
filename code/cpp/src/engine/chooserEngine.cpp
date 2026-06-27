#include "chooserEngine.hpp"
#include "blackScholesCloseForm.hpp"
#include "pathGeneration.hpp"
#include <cmath>
#include <numeric>
#include <vector>

ChooserEngine::ChooserEngine(const ModelParameters& modelParams,
                             const SimulationParameters& simParams)
    : modelParams_(modelParams), simParams_(simParams) {};

void ChooserEngine::calculate() {

    const std::vector<std::vector<double>> simulatedPaths =
        simulateGbmPath(modelParams_, simParams_);

    ModelParameters modelParamsBsCloseForm = modelParams_;
    modelParamsBsCloseForm.timeHorizonInYears =
        args_.engineParams.maturity - modelParams_.timeHorizonInYears;

    const double strike = args_.engineParams.strike;

    std::vector<double> prices(simulatedPaths.size());

    for (size_t i = 0; i < simulatedPaths.size(); ++i) {
        modelParamsBsCloseForm.underlyingPrice = simulatedPaths[i].back();

        BsCloseForm<VanillaEuropeanOption> bsCloseForm{modelParamsBsCloseForm};

        VanillaEuropeanOption call(
            OptionParameters{strike, OptionType::Call, OptionDirection::Long});

        call.setPricingEngine(&bsCloseForm);

        VanillaEuropeanOption put(
            OptionParameters{strike, OptionType::Put, OptionDirection::Long});

        put.setPricingEngine(&bsCloseForm);

        prices[i] = std::max(call.price(), put.price());
    };

    const double sign =
        args_.engineParams.direction == OptionDirection::Long ? 1.0 : -1.0;

    const double deterministicDiscountFactor =
        std::exp(-modelParams_.interestRate * modelParams_.timeHorizonInYears);

    result_ = std::accumulate(prices.begin(), prices.end(), 0.0) /
              simulatedPaths.size() * sign * deterministicDiscountFactor;
};
