#pragma once
// Cables and protection: resistance, voltage drop, current-carrying capacity
// (IEC 60364-5-52 / DIN VDE 0298-4 / NIN 5.2.3), overload and fault protection,
// adiabatic short-circuit withstand.

#include <array>
#include <optional>
#include <span>

#include "elc/formulas/common.hpp"

namespace elc::cables {

enum class Material { Cu, Al };
enum class Insulation { PVC, XLPE };
/// Reference installation methods (IEC 60364-5-52 Table B.52.1)
enum class Method { A1, A2, B1, B2, C, D1, E };
/// Grouping arrangements (IEC 60364-5-52 Table B.52.17, items 1–5)
enum class Arrangement { Bunched, SingleLayerWall, SingleLayerCeiling, PerforatedTray, Ladder };
/// Protective devices
enum class Device { MCB_B, MCB_C, MCB_D, FuseGG };

// ---- resistance --------------------------------------------------------------------------

/// ρ at 20 °C in Ω·mm²/m (Cu 1/58 IEC 60028, Al 1/35.38 IEC 60889)
[[nodiscard]] constexpr double resistivity20(Material m) noexcept { return m == Material::Cu ? 0.017241 : 0.028264; }
/// Temperature coefficient α20 in 1/K
[[nodiscard]] constexpr double alpha20(Material m) noexcept { return m == Material::Cu ? 0.00393 : 0.00403; }
/// ρ(θ) = ρ20·(1 + α20·(θ − 20 °C))
[[nodiscard]] double resistivity(Material m, double tempC);

// ---- voltage drop ------------------------------------------------------------------------

/// ΔU = k·L·I·(ρ/A·cosφ + X'·sinφ) in V. L in m, A in mm², ρ in Ω·mm²/m, X' in Ω/m. DC: cosφ=1, X'=0.
[[nodiscard]] double voltageDrop(System sys, double L, double I, double A, double rho, double cosPhi, double xPerM);
/// Cross-section that just meets ΔU_max; +∞ if unreachable (reactance alone exceeds the limit).
[[nodiscard]] double requiredArea(System sys, double L, double I, double rho, double cosPhi, double xPerM,
                                  double dUmax);
/// Length at which ΔU reaches ΔU_max.
[[nodiscard]] double maxLength(System sys, double I, double A, double rho, double cosPhi, double xPerM, double dUmax);
/// Ohmic losses of the circuit in W (DC/1~: 2 conductors, 3~: 3 conductors).
[[nodiscard]] double lineLoss(System sys, double L, double I, double A, double rho);

inline constexpr std::array<double, 19> kStandardAreas{0.5, 0.75, 1.0, 1.5, 2.5, 4.0,  6.0,  10.0, 16.0, 25.0,
                                                       35.0, 50.0, 70.0, 95.0, 120.0, 150.0, 185.0, 240.0, 300.0};
/// Cross-sections covered by the ampacity tables (1.5 … 300 mm²)
inline constexpr std::array<double, 16> kAmpacityAreas{1.5,  2.5,  4.0,   6.0,   10.0,  16.0,  25.0,  35.0,
                                                       50.0, 70.0, 95.0, 120.0, 150.0, 185.0, 240.0, 300.0};

// ---- current-carrying capacity (copper) --------------------------------------------------

/// Iz0 in A for copper at 30 °C air / 20 °C ground, from IEC 60364-5-52 Tables B.52.2–B.52.5 and
/// B.52.10/B.52.11 (method E). loaded = 2 or 3. nullopt if the area is not tabulated.
[[nodiscard]] std::optional<double> baseAmpacity(Insulation ins, Method method, int loaded, double area);
/// Max. conductor temperature: PVC 70 °C, XLPE/EPR 90 °C
[[nodiscard]] constexpr double maxConductorTemp(Insulation ins) noexcept { return ins == Insulation::PVC ? 70.0 : 90.0; }
/// f_T = √((θmax − θa)/(θmax − θref)), θref = 30 °C (air) or 20 °C (ground, method D1).
/// Reproduces Tables B.52.14/B.52.15. Returns 0 if θa >= θmax.
[[nodiscard]] double ambientFactor(Insulation ins, Method method, double ambientC);
/// f_G from Table B.52.17. Counts between tabulated values use the next higher count.
[[nodiscard]] double groupingFactor(Arrangement arr, int circuits);

// ---- protection --------------------------------------------------------------------------

/// Standard rated currents In in A
[[nodiscard]] std::span<const double> ratedCurrents(Device d);
/// I2/In: MCB 1.45 (IEC 60898-1); gG 2.1 (In ≤ 4 A), 1.9 (< 16 A), 1.6 (≥ 16 A) (IEC 60269-1)
[[nodiscard]] double conventionalTripFactor(Device d, double In);
/// Instantaneous release, upper limit of the MCB characteristic: B 5·In, C 10·In, D 20·In
[[nodiscard]] double instantaneousMultiple(Device d);
/// ρ for fault-loop calculation (≈1.25·ρ20 at operating temperature): Cu 0.0225, Al 0.036 Ω·mm²/m
[[nodiscard]] constexpr double faultResistivity(Material m) noexcept { return m == Material::Cu ? 0.0225 : 0.036; }
/// TN system, conventional method: L_max = 0.8·U0·S_ph / (ρ·(1 + m)·Ia), m = S_ph / S_PE
[[nodiscard]] double maxLengthDisconnection(double U0, double Sph, double Spe, Material mat, double Ia);
/// k factor for the adiabatic equation S = √(I²t)/k (IEC 60364-4-43 Table 43A)
[[nodiscard]] double kFactor(Material m, Insulation ins);

}  // namespace elc::cables
