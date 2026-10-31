#pragma once
// Building automation / instrumentation (GA/MSR): analog signals, RTD/NTC sensors,
// valves and hydraulics.

#include <array>

#include "elc/formulas/common.hpp"

namespace elc::automation {

/// Linear mapping of x from [inLo, inHi] to [outLo, outHi]
[[nodiscard]] constexpr double scale(double x, double inLo, double inHi, double outLo, double outHi) noexcept {
    return outLo + (x - inLo) * (outHi - outLo) / (inHi - inLo);
}

enum class Rtd { Pt100, Pt500, Pt1000, Ni1000, Ni1000TK5000 };

/// R(T) in Ω.
/// Pt: IEC 60751 Callendar–Van Dusen (A = 3.9083e-3, B = −5.775e-7, C = −4.183e-12 below 0 °C)
/// Ni1000: DIN 43760 (TK 6180 ppm/K); Ni1000 TK5000 (LG-Ni 1000)
[[nodiscard]] double rtdResistance(Rtd type, double T);
/// dR/dT in Ω/K
[[nodiscard]] double rtdSensitivity(Rtd type, double T);
/// Inverse of rtdResistance (Newton iteration with bisection fallback)
[[nodiscard]] double rtdTemperature(Rtd type, double R);
/// Valid temperature range of the characteristic {min, max} in °C
[[nodiscard]] std::array<double, 2> rtdRange(Rtd type);
[[nodiscard]] double rtdR0(Rtd type);

/// NTC beta model: R = R25·exp(B·(1/T − 1/T25)), T in K internally
[[nodiscard]] double ntcResistance(double R25, double B, double T_C);
[[nodiscard]] double ntcTemperature(double R25, double B, double R);

/// kv = Q·√(ρ/1000 / Δp[bar])  (Q in m³/h, Δp in kPa, ρ in kg/m³)
[[nodiscard]] double kvValue(double Q_m3h, double dp_kPa, double rho);
/// Δp in kPa for flow Q through a valve with kvs
[[nodiscard]] double pressureDrop(double Q_m3h, double kvs, double rho);

/// Common kvs series (R10-like)
inline constexpr std::array<double, 22> kKvsSeries{0.1, 0.16, 0.25, 0.4, 0.63, 1.0, 1.6, 2.5,  4.0,  6.3,   10.0,
                                                   16.0, 25.0, 40.0, 63.0, 100.0, 160.0, 250.0, 400.0, 630.0, 1000.0, 1600.0};

/// V̇ [m³/h] = P / (ρ·c·ΔT)   (P in kW, ρ in kg/m³, c in kJ/(kg·K), ΔT in K)
[[nodiscard]] double volumeFlowFromPower(double P_kW, double dT, double rho, double c);
/// P [kW] = V̇·ρ·c·ΔT
[[nodiscard]] double powerFromVolumeFlow(double V_m3h, double dT, double rho, double c);

}  // namespace elc::automation
