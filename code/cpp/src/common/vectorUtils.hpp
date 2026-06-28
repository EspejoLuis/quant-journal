#pragma once
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>

inline ptrdiff_t upperBracketIndexInSorted(const std::vector<double>& v,
                                           const double target) {

    if (!std::is_sorted(v.begin(), v.end()))
        throw std::invalid_argument("upperBracketIndexInSorted: grid must be "
                                    "sorted in ascending order");

    std::vector<double>::const_iterator it{
        std::lower_bound(v.begin(), v.end(), target)};

    const ptrdiff_t idx{std::distance(v.begin(), it)};

    if (idx <= 0 || idx >= static_cast<ptrdiff_t>(v.size()))
        throw std::out_of_range(
            "upperBracketIndexInSorted: target out of grid range");

    return idx;
}