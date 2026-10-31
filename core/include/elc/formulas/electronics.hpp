#pragma once
// Electronics: colour code, E series, RC/LC, reactance.

#include <array>
#include <span>
#include <string_view>
#include <vector>

#include "elc/formulas/common.hpp"

namespace elc::electronics {

enum class ESeries { E6, E12, E24, E48, E96 };

/// Base values of one decade (E6/E12/E24: 10…91, E48/E96: 100…976)
[[nodiscard]] std::span<const double> eSeriesBase(ESeries s);
/// All series values across decades 1e-1 … 1e9 in ascending order
[[nodiscard]] std::vector<double> eSeriesValues(ESeries s);

struct ESeriesMatch {
    double lower, upper, nearest;
};
[[nodiscard]] ESeriesMatch eSeriesMatch(ESeries s, double value);

/// IEC 60062 colour code: digit 0–9 for black…white, nullopt for gold/silver/none
inline constexpr std::array<std::string_view, 10> kDigitColours{"black", "brown", "red",    "orange", "yellow",
                                                                "green", "blue",  "violet", "grey",   "white"};

/// τ = R·C, f_c = 1/(2π·R·C)
[[nodiscard]] constexpr double timeConstant(double R, double C) noexcept { return R * C; }
[[nodiscard]] double cutoffFrequency(double R, double C);
/// Capacitor voltage while charging: u(t) = U0·(1 − e^(−t/τ))
[[nodiscard]] double chargingVoltage(double U0, double t, double tau);
/// f0 = 1/(2π·√(L·C))
[[nodiscard]] double resonanceFrequency(double L, double C);
/// X_L = 2πfL, X_C = 1/(2πfC)
[[nodiscard]] double inductiveReactance(double f, double L);
[[nodiscard]] double capacitiveReactance(double f, double C);

}  // namespace elc::electronics
