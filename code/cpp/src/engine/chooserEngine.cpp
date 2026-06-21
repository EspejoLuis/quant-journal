#include "chooserEngine.hpp"
#include "blackScholesCloseForm.hpp"
#include "pathGeneration.hpp"
#include <cmath>
#include <numeric>
#include <vector>

ChooserEngine::ChooserEngine(const ModelParameters& modelParams,
                             const SimulationParameters& simParams)
    : modelParams_(modelParams), simParams_(simParams) {};

double ChooserEngine::price(
    const ChooserEuropeanOption& chooserEuropeanOption) {

    const std::vector<std::vector<double>> simulatedPaths =
        simulateGbmPath(modelParams_, simParams_);

    ModelParameters modelParamsBsCloseForm = modelParams_;
    modelParamsBsCloseForm.timeHorizonInYears =
        chooserEuropeanOption.parameters().maturity -
        modelParams_.timeHorizonInYears;

    const double strike = chooserEuropeanOption.parameters().strike;

    std::vector<double> prices = std::vector<double>(simulatedPaths.size());

    for (size_t i = 0; i < simulatedPaths.size(); ++i) {
        modelParamsBsCloseForm.underlyingPrice = simulatedPaths[i].back();

        VanillaEuropeanOption call(
            OptionParameters{strike, OptionType::Call, OptionDirection::Long});

        VanillaEuropeanOption put(
            OptionParameters{strike, OptionType::Put, OptionDirection::Long});

        BsCloseForm bsCloseForm(modelParamsBsCloseForm);

        double callPrice = bsCloseForm.price(call);
        double putPrice = bsCloseForm.price(put);

        prices[i] = std::max(callPrice, putPrice);
    };

    const double sign =
        chooserEuropeanOption.parameters().direction == OptionDirection::Long
            ? 1.0
            : -1.0;

    const double deterministicDiscountFactor =
        std::exp(-modelParams_.interestRate * modelParams_.timeHorizonInYears);

    return std::accumulate(prices.begin(), prices.end(), 0.0) /
           simulatedPaths.size() * sign * deterministicDiscountFactor;
};
