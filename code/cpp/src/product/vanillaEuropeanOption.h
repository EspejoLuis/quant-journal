#pragma once
#include "monteCarloEngine.h"

enum class OptionType { Call, Put };
enum class OptionDirection { Long, Short };

struct OptionParameters {
    double strike;
    OptionType type;
    OptionDirection direction;
};

double priceEuropeanVanillaOption(const ModelParameters& modelParams,
                                  const SimulationParameters& simParams,
                                  const OptionParameters& optParams);
