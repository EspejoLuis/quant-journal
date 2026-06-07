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

    if (optParams.type == OptionType::Call) {
        for (const std::vector<double>& path : simulatedGbmPaths)
            sumPayoffs += std::max(path.back() - optParams.strike, 0.0);
    } else {
        for (const std::vector<double>& path : simulatedGbmPaths)
            sumPayoffs += std::max(optParams.strike - path.back(), 0.0);
    };

    double expectedPayoff = optParams.direction == OptionDirection::Long
                                ? sumPayoffs / simParams.nPaths
                                : -sumPayoffs / simParams.nPaths;

    double deterministicDiscountFactor =
        std::exp(-modelParams.interestRate * modelParams.maturityInYears);

    return deterministicDiscountFactor * expectedPayoff;
}
