#pragma once
#include <vector>

enum class PdeScheme { Explicit, Implicit, CrankNicolson };
enum class PdeGrid { Uniform, Log };

struct PdeParameters {
    int nSpaceSteps;
    int nTimeSteps;
    PdeGrid grid;
    PdeScheme scheme;
};

struct PdeCoefficients {

    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
};