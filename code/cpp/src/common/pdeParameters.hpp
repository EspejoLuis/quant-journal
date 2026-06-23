#pragma once

enum class PdeScheme { Explicit, Implicit, CrankNicolson };
enum class PdeGrid { Uniform, Log };

struct PdeParameters {
    int nSpaceSteps;
    int nTimeSteps;
    PdeGrid grid;
    PdeScheme scheme;
};