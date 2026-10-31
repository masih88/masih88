#pragma once
// Basic electrical relations: Ohm's law, power triangle, R/C networks, voltage divider.

#include <optional>
#include <span>

#include "elc/formulas/common.hpp"

namespace elc::basics {

struct PowerTriangle {
    double P;       // W
    double Q;       // var
    double S;       // VA
    double phiDeg;  // °
};

/// S = k·U·I, P = S·cosφ, Q = S·sinφ (DC: cosφ = 1)
[[nodiscard]] PowerTriangle powerFromVoltageCurrent(System sys, double U, double I, double cosPhi);

/// I = P / (k·U·cosφ·η)
[[nodiscard]] double currentFromActivePower(System sys, double P, double U, double cosPhi, double eta = 1.0);

struct OhmValues {
    double U, I, R, P;
};

/// Solves U = R·I and P = U·I from exactly two known values; nullopt otherwise or if not solvable.
[[nodiscard]] std::optional<OhmValues> solveOhm(std::optional<double> U, std::optional<double> I,
                                                std::optional<double> R, std::optional<double> P);

/// Σ x  (resistors/inductors in series, capacitors in parallel)
[[nodiscard]] double directSum(std::span<const double> values);

/// 1 / Σ(1/x)  (resistors/inductors in parallel, capacitors in series)
[[nodiscard]] double reciprocalSum(std::span<const double> values);

struct DividerResult {
    double UoutUnloaded;
    double UoutLoaded;  // == unloaded if no load given
    double I1;          // current through R1
    double P1, P2;      // power in R1, R2
};

/// Unloaded/loaded voltage divider; RL <= 0 or nullopt means no load.
[[nodiscard]] DividerResult voltageDivider(double Uin, double R1, double R2, std::optional<double> RL);

}  // namespace elc::basics
