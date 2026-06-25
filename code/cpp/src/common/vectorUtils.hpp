#pragma once
#include <algorithm>
#include <iterator>
#include <vector>

inline ptrdiff_t findClosestIndex(const std::vector<double>& v,
                                  const double target) {

    std::vector<double>::const_iterator it =
        std::lower_bound(v.begin(), v.end(), target);

    const ptrdiff_t idx = std::distance(v.begin(), it);

    if (idx <= 0.0 || static_cast<ptrdiff_t>(v.size()))
        throw std::invalid_argument("target out of the grid range");

    return idx;
}