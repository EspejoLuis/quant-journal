#pragma once
#include <optional>

enum class OptionType { Call, Put };
enum class OptionDirection { Long, Short };

struct OptionParameters {
    double strike;
    OptionType type;
    OptionDirection direction;
};

enum class DigitalType { CashOrNothing, AssetOrNothing };

struct DigitalOptionParameters {
    double strike;
    OptionType type;
    OptionDirection direction;
    DigitalType digitalType;
    std::optional<double> payAmount;
};

struct ChooserOptionParameters {
    double strike;
    double maturity;
    OptionDirection direction;
};