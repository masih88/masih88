#pragma once
// Motors and drives, power factor correction, transformers, short-circuit estimate,
// enclosure heat balance.

#include <optional>

#include "elc/formulas/cables.hpp"
#include "elc/formulas/common.hpp"

namespace elc::motors {

/// I_N = P_N / (k·U·cosφ·η)   (P_N = mechanical shaft power in W)
[[nodiscard]] double ratedCurrent(System sys, double Pmech, double U, double cosPhi, double eta);

struct TorqueValues {
    double P_kW, n_rpm, M_Nm;
};
/// M = 9549.3·P/n from exactly two of P [kW], n [1/min], M [Nm]
[[nodiscard]] std::optional<TorqueValues> solveTorque(std::optional<double> P_kW, std::optional<double> n_rpm,
                                                      std::optional<double> M_Nm);

/// n_s = 120·f / poles  [1/min]
[[nodiscard]] constexpr double synchronousSpeed(double f, double poles) noexcept { return 120.0 * f / poles; }
/// s = (n_s − n)/n_s
[[nodiscard]] constexpr double slip(double ns, double n) noexcept { return (ns - n) / ns; }

struct StarDelta {
    double IstartDirect;     // A, direct-on-line
    double IstartStar;       // A, = 1/3 of DOL
    double torqueStarRatio;  // M_A,Y / M_N
    double relaySetting;     // A, overload relay in the motor phase: I_N/√3
};
[[nodiscard]] StarDelta starDelta(double In, double IaIn, double MaMn);

struct Steinmetz {
    double Crun_uF;
    double CstartMin_uF, CstartMax_uF;
    double Pavail_kW;
};
/// Rule of thumb: C_B ≈ 70 µF/kW at 230 V/50 Hz, scaled with (230/U)²·(50/f); C_A ≈ 2…3·C_B; P ≈ 0.75·P_N
[[nodiscard]] Steinmetz steinmetz(double P_kW, double U, double f);

}  // namespace elc::motors

namespace elc::industry {

/// Q_C = P·(tanφ1 − tanφ2)
[[nodiscard]] double compensationPower(double P, double cosPhi1, double cosPhi2);

enum class CapConnection { Delta, Star, Single };
/// Capacitance per capacitor in F for total reactive power Qc [var] at line voltage U.
/// Δ: C = Qc/(3·U²·ω), Y: C = Qc/(U²·ω), 1~: C = Qc/(U²·ω)
[[nodiscard]] double capacitance(double Qc, double U, double f, CapConnection c);

/// I = S/(√3·U) or S/U
[[nodiscard]] double transformerCurrent(double S, double U, bool threePhase);

struct ShortCircuitInput {
    double SrT_kVA;
    double uk_pct;
    double uR_pct;
    double Un_V;
    std::optional<double> SkQ_MVA;  // upstream network, nullopt = infinite
    double L_m;
    double Aph_mm2;
    double Ape_mm2;
    cables::Material material;
    double x_mOhm_per_m;
};

struct ShortCircuitResult {
    double ZQ_mOhm, ZT_mOhm, ZL_mOhm;
    double Ik3Trafo_kA;  // at LV terminals, c_max
    double Ik3End_kA;    // end of line, c_max, conductors at 20 °C
    double Ik1End_kA;    // end of line, c_min, conductors at 80 °C, loop L + PE
};
/// Simplified IEC 60909-0 estimate: c_max 1.05, c_min 0.95, network X/R = 10, no K_T correction.
[[nodiscard]] ShortCircuitResult shortCircuit(const ShortCircuitInput& in);

/// Effective enclosure surface (IEC/TR 60890 resp. DIN VDE 0660-507):
/// free-standing A = 1.8·H·(W+D) + 1.4·W·D, wall-mounted A = 1.4·W·(H+D) + 1.8·D·H  [m]
[[nodiscard]] double enclosureSurface(double H, double W, double D, bool wallMounted);

}  // namespace elc::industry
