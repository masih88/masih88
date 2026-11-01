#pragma once
// Control engineering: process difficulty and controller tuning rules.
// Source: Siemens "Regeltechnik – Grundlagen" (08/2022), chapter 3.3/3.4 (difficulty S) and
// chapter 6.5 (Fig. 6-15 Ziegler/Nichols, Fig. 6-18 Chien/Hrones/Reswick).

#include <optional>

#include "elc/constants.hpp"

namespace elc::control {

enum class Controller { P, PI, PD, PID };

enum class Rule {
    ZieglerNichols,           // max. 20 % overshoot, single-capacity process with dead time
    ChrAperiodicDisturbance,  // C/H/R, no overshoot, disturbance response
    ChrAperiodicSetpoint,     // C/H/R, no overshoot, set-point response
    Chr20Disturbance,         // C/H/R, 20 % overshoot (shortest settling), disturbance response
    Chr20Setpoint,            // C/H/R, 20 % overshoot, set-point response
};

/// Controller parameters; tn/tv are NaN when the controller has no I or D part.
struct Tuning {
    double kp = kNaN;
    double tn = kNaN;
    double tv = kNaN;
};

enum class Difficulty { Easy, Medium, Hard };

/// S = Tu / Tg (multi-capacity) or Tt / T (single capacity with dead time)
[[nodiscard]] inline double difficultyRatio(double tu, double tg) noexcept { return tu / tg; }
/// S < 0,1 easy, 0,1 … 0,3 medium, > 0,3 hard to control
[[nodiscard]] Difficulty classify(double S) noexcept;

/// Tuning by rule. ks = process gain, tu = dead/delay time Tt resp. Tu, tg = time constant T resp. Tg.
/// C/H/R gives V0 = Ks·Kp, so Kp = V0 / Ks. nullopt if the rule has no entry for the controller (C/H/R PD).
[[nodiscard]] std::optional<Tuning> tune(Rule rule, Controller c, double ks, double tu, double tg);

}  // namespace elc::control
