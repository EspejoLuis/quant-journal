#pragma once
#include <optional>

enum class VarianceReduction {
    Antithetic,
    ImportantSampling,
    ControlVariates,
};

struct SimulationParameters {
    int nPaths;
    int nSteps;
    std::optional<unsigned int> seed;
    std::optional<VarianceReduction> varianceReduction;
};