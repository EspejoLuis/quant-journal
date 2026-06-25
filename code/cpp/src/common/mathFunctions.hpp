#pragma once
#include <cmath>
#include <numeric>
#include <vector>

inline double mean(const std::vector<double>& v) {

    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
};

inline double sampleVariance(const std::vector<double>& v) {
    double m{mean(v)};
    return std::accumulate(
               v.begin(), v.end(), 0.0,
               [m](double acc, double x) { return acc + (x - m) * (x - m); }) /
           (v.size() - 1);
};

inline double sampleStandardDeviation(const std::vector<double>& v) {

    return std::sqrt(sampleVariance(v));
};

inline double normalCdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
};

inline double interpolationLinear(double x, double x0, double x1, double y0,
                                  double y1) {
    return y0 + (x - x0) / (x1 - x0) * (y1 - y0);
};