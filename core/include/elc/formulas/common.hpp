#pragma once
// Shared constants and helpers for all formula modules.

#include <algorithm>
#include <cmath>
#include <concepts>
#include <numbers>
#include <optional>
#include <ranges>

#include "elc/constants.hpp"

namespace elc {

/// Supply system of a circuit.
enum class System { DC, AC1, AC3 };

/// S = k·U·I  (k = 1 for DC/single phase, √3 for three phase with line voltage U)
constexpr double powerFactorK(System s) noexcept { return s == System::AC3 ? kSqrt3 : 1.0; }

/// ΔU = k·L·I·(R'cosφ + X'sinφ)  (k = 2 for DC/single phase: out + return, √3 for three phase)
constexpr double dropFactorK(System s) noexcept { return s == System::AC3 ? kSqrt3 : 2.0; }

/// A range of floating point values, e.g. a standard series (cross-sections, ratings, kvs).
template <typename R>
concept FloatSeries = std::ranges::forward_range<R> && std::floating_point<std::ranges::range_value_t<R>>;

/// Smallest series value >= v (relative tolerance 1e-9), or nullopt if v exceeds the series.
template <FloatSeries R>
[[nodiscard]] std::optional<double> nextAtLeast(const R& series, double v) {
    for (const double s : series) {
        if (s >= v * (1.0 - 1e-9)) return s;
    }
    return std::nullopt;
}

/// Largest series value <= v, or nullopt if v is below the series.
template <FloatSeries R>
[[nodiscard]] std::optional<double> nextAtMost(const R& series, double v) {
    std::optional<double> best;
    for (const double s : series) {
        if (s <= v * (1.0 + 1e-9)) best = s;
    }
    return best;
}

/// Series value closest to v on a logarithmic scale (natural for E-series, kvs, cross-sections).
template <FloatSeries R>
[[nodiscard]] double nearestLog(const R& series, double v) {
    double best = *std::ranges::begin(series);
    double bestDist = std::fabs(std::log(v / best));
    for (const double s : series) {
        const double d = std::fabs(std::log(v / s));
        if (d < bestDist) {
            best = s;
            bestDist = d;
        }
    }
    return best;
}

}  // namespace elc
