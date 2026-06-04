#pragma once 
#include <vector>

std::vector<std::vector<double>> SimulateGBMPath(
    double underlyingPrice,
    double interestRate,
    double divindedRate,
    double volatility,
    double maturity,
    int nTimeSteps,
    int nPaths
);