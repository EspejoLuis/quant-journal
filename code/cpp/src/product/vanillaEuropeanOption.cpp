#include "vanillaEuropeanOption.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

double priceEuropeanVanillaOption(const ModelParameters& modelParams,
                                  const SimulationParameters& simParams,
                                  const OptionParameters& optParams) {
    if (optParams.strike < 0)
        throw std::invalid_argument("strike cannot be negative");

    std::vector<std::vector<double>> simulatedGbmPaths =
        simulateGbmPath(modelParams, simParams);

    double sumPayoffs = 0.0;

    switch (optParams.type) {
    case OptionType::Call:
        for (const std::vector<double>& path : simulatedGbmPaths)
            sumPayoffs += std::max(path.back() - optParams.strike, 0.0);
        break;
    case OptionType::Put:
        for (const std::vector<double>& path : simulatedGbmPaths)
            sumPayoffs += std::max(optParams.strike - path.back(), 0.0);
        break;
    default:
        throw std::invalid_argument("invalid OptionType");
    };

    double sign = 0.0;

    switch (optParams.direction) {
    case OptionDirection::Long:
        sign = +1.0;
        break;
    case OptionDirection::Short:
        sign = -1.0;
        break;
    default:
        throw std::invalid_argument("invalid OptionDirection");
    }

    double expectedPayoff = sign * sumPayoffs / simParams.nPaths;
    double deterministicDiscountFactor =
        std::exp(-modelParams.interestRate * modelParams.maturityInYears);

    return deterministicDiscountFactor * expectedPayoff;
}
