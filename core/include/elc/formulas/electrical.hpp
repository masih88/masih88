#pragma once
// Electrical engineering (area "Elektrotechnik", v1.7): power and impedance, cables (temperature, losses,
// neutral current, distributed loads), compensation, earthing, overvoltage risk, component codes,
// transformer windings, antennas, CCTV, Δ-Y transformation, wire gauges, NEC motor currents,
// busbar and mineral-insulated cable ratings.

#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "elc/formulas/cables.hpp"
#include "elc/formulas/common.hpp"

namespace elc::electrical {

// ---- power, voltage, impedance --------------------------------------------------------------
/// U = P / (k·I·cos φ), k = 1 (DC/1~) or √3 (3~); DC ignores cos φ. P in W.
[[nodiscard]] double voltageFromPower(System sys, double P, double I, double cosPhi);
/// cos φ = P / (k·U·I)
[[nodiscard]] double powerFactor(System sys, double P, double U, double I);

struct Impedance {
    double Z, R, X;
};
/// Impedance per phase from U and I: 1~ Z = U/I, 3~ (star equivalent) Z = U/(√3·I); R = Z·cos φ, X = Z·sin φ
[[nodiscard]] Impedance impedanceFromUI(System sys, double U, double I, double cosPhi);

/// Operating current I_b = P·k_u·k_s / (k·U·cos φ·η) (P installed active power in W)
[[nodiscard]] double operatingCurrent(System sys, double U, double P, double ku, double ks, double cosPhi, double eta);

/// Joule heat W = I²·R·t in J; temperature rise ΔT = W/(m·c)
[[nodiscard]] constexpr double jouleHeat(double R, double I, double t) noexcept { return I * I * R * t; }
[[nodiscard]] constexpr double temperatureRise(double W, double m, double c) noexcept { return W / (m * c); }

// ---- cables ---------------------------------------------------------------------------------
/// Conductor temperature θ = θa + (θmax − θa)·(Ib/Iz)²  (IEC 60287 simplification, losses ∝ I²)
[[nodiscard]] double cableTemperature(double Ib, double Iz, double ambient, double maxTemp);

/// Neutral current of a 3~ four-wire circuit from the three phase currents (same cos φ, 120° apart)
[[nodiscard]] double neutralCurrentFundamental(double I1, double I2, double I3);
/// Neutral current including the 3rd harmonic h3 (in % of each phase current): the 3rd harmonics add up
/// arithmetically in the neutral. I_N = √(I_N1² + (h3·(I1 + I2 + I3))²). Phase currents = fundamental.
[[nodiscard]] double neutralCurrent(double I1, double I2, double I3, double h3Percent);

/// IEC 60364-5-52 Annex E, Table E.52.1: sizing of 4-/5-core cables with 3rd harmonics
struct HarmonicSizing {
    bool byNeutral;  // true: select the cable on the neutral current
    double factor;   // reduction factor applied to the tabulated ampacity
};
[[nodiscard]] HarmonicSizing harmonicSizing(double h3Percent);

/// Σ I·L of n equal loads (current I each), the first at L1, then every d metres
[[nodiscard]] double distributedMoment(int n, double I, double L1, double d);
/// Voltage drop at the last load: ΔU = k·Σ(I·L)·(ρ/A·cos φ + X'·sin φ), k = 2 (DC/1~) or √3 (3~)
[[nodiscard]] double distributedVoltageDrop(System sys, double moment, double A, double rho, double cosPhi, double xPerM);
/// Cross-section for ΔU_max (in V); +∞ if the reactance alone exceeds the limit
[[nodiscard]] double distributedRequiredArea(System sys, double moment, double rho, double cosPhi, double xPerM, double dUmax);

// ---- compensation ---------------------------------------------------------------------------
/// No-load reactive power of a transformer Q0 = i0 · S_N (i0 in %); load part Q_L = u_k · S_N · (S/S_N)²
[[nodiscard]] constexpr double transformerNoLoadQ(double Sn, double i0Percent) noexcept { return Sn * i0Percent / 100.0; }
[[nodiscard]] constexpr double transformerLoadQ(double Sn, double ukPercent, double S) noexcept {
    return Sn * ukPercent / 100.0 * (S / Sn) * (S / Sn);
}
/// Capacitor at another voltage/frequency: Q = Q_N·(U/U_N)²·(f/f_N)
[[nodiscard]] constexpr double capacitorPowerAt(double Qn, double Un, double fn, double U, double f) noexcept {
    return Qn * (U / Un) * (U / Un) * (f / fn);
}
/// Capacitance for Q at U (single capacitor / single phase): C = Q/(2π·f·U²)
[[nodiscard]] double capacitanceFor(double Q, double U, double f);

// ---- earthing and fault protection ----------------------------------------------------------
/// TT with RCD: R_A·I_Δn ≤ U_L (IEC 60364-4-41, 411.5.3)
[[nodiscard]] constexpr double maxEarthResistanceTT(double UL, double IdeltaN) noexcept { return UL / IdeltaN; }
/// TN: Z_s·I_a ≤ U_0 (411.4.4)
[[nodiscard]] constexpr double maxLoopImpedanceTN(double U0, double Ia) noexcept { return U0 / Ia; }

struct MinShortCircuit {
    double ik1;  // line – PE (fault loop)
    double ik1N; // line – neutral
    double ik2;  // line – line
    double rLine, rPe, rN;  // conductor resistances at the end temperature in Ω
};
/// Minimum short-circuit currents at the end of a cable (IEC 60909 simplified): c_min·U0 / |Z_up + Z_line|.
/// Zup = upstream loop impedance (L–PE) at the origin in Ω (R/X split by xr = X/R of the upstream network),
/// cable resistance at θ, X' per conductor in Ω/m. Ik2 uses the line–line loop (2 conductors).
[[nodiscard]] MinShortCircuit minShortCircuit(double c, double U0, double ZupLoop, double xrUp, double L, double Sph,
                                              double Spe, double Sn, cables::Material mat, double theta, double xPerM);

/// Overvoltage risk per IEC 60364-4-44 443.5: L_P = 2·L_PAL + L_PCL + 0.4·L_PAH + 0.2·L_PCH (km),
/// CRL = f_env / (L_P · N_g). Protection required if CRL < 1000.
[[nodiscard]] constexpr double surgeLP(double lpal, double lpcl, double lpah, double lpch) noexcept {
    return 2.0 * lpal + lpcl + 0.4 * lpah + 0.2 * lpch;
}
[[nodiscard]] constexpr double surgeCRL(double fenv, double LP, double Ng) noexcept { return fenv / (LP * Ng); }

// ---- transformers, antennas, CCTV -----------------------------------------------------------
/// Turns per volt N/U = 1 / (4.44·f·B·A_Fe), A_Fe in m²
[[nodiscard]] double turnsPerVolt(double f, double B, double areaM2);
/// λ = c/f (f in Hz, λ in m)
[[nodiscard]] double wavelength(double f);
/// Bits per pixel (Richtwert) of a CCTV stream: H.264 0.08, H.265 0.04, MJPEG 0.75
[[nodiscard]] double cctvBitsPerPixel(std::string_view codec);

// ---- electronics ----------------------------------------------------------------------------
struct ZenerDesign {
    double Rv;     // series resistor in Ω
    double PR;     // max. power in the resistor (U_e,max) in W
    double IzMax;  // max. Zener current (U_e,max, I_L,min) in A
    double PzMax;  // max. Zener power in W
};
/// Series resistor so that I_Z ≥ I_Z,min at U_e,min and I_L,max: R_v = (U_e,min − U_Z)/(I_Z,min + I_L,max)
[[nodiscard]] ZenerDesign zenerDesign(double UeMin, double UeMax, double Uz, double IzMin, double ILmin, double ILmax);

/// Δ → Y: R1 = R12·R31/ΣR, R2 = R12·R23/ΣR, R3 = R23·R31/ΣR
[[nodiscard]] std::array<double, 3> deltaToStar(double R12, double R23, double R31);
/// Y → Δ: R12 = (R1R2 + R2R3 + R3R1)/R3, R23 = …/R1, R31 = …/R2
[[nodiscard]] std::array<double, 3> starToDelta(double R1, double R2, double R3);

/// SMD resistor code: "472", "4702", "4R7", "R47", "0", "000", EIA-96 "01C". Value in Ω.
struct SmdCode {
    double ohms;
    std::string kind;  // "3", "4", "R", "EIA-96", "0"
};
[[nodiscard]] std::optional<SmdCode> smdResistor(std::string_view code);
/// EIA-96 values (index 1…96 → 100 … 976)
[[nodiscard]] std::span<const double> eia96Values();

/// Capacitor marking: "104", "104K", "2A104J", "4n7", "1R5", "0.1", "47p", "22u". Value in pF.
struct CapacitorCode {
    double pF;
    std::optional<double> tolerancePercent;  // F/G/J/K/M; Z = +80/−20 % → nullopt, text in toleranceText
    std::optional<double> tolerancePF;       // B/C/D (absolute, small capacitors)
    std::string toleranceText;
    std::optional<double> voltage;           // EIA voltage code prefix, e.g. "2A" = 100 V
};
[[nodiscard]] std::optional<CapacitorCode> capacitorCode(std::string_view code);
/// EIA voltage codes (e.g. "1H" = 50 V)
struct VoltageCode {
    std::string_view code;
    double volts;
};
[[nodiscard]] std::span<const VoltageCode> capacitorVoltageCodes();

/// Inductor marking: "101" (µH, 3 digits), "4R7" (µH), "R10", "47N" (nH). Value in µH.
[[nodiscard]] std::optional<double> inductorCode(std::string_view code);

/// Colour-band digits for a value: significant digits (2 or 3) and the multiplier exponent (−2 … 9).
/// Returns nullopt if the value cannot be represented exactly with that many digits (after rounding to 3 significant
/// digits for 5 bands, 2 for 4 bands) – the caller then shows the rounded value.
struct BandCode {
    std::vector<int> digits;
    int exponent;      // multiplier 10^exponent
    double value;      // value represented by the bands
};
[[nodiscard]] std::optional<BandCode> bandsFor(double value, int significant);

// ---- wire gauges ----------------------------------------------------------------------------
struct Gauge {
    std::string_view name;  // "7/0" … "50"
    double inch;            // diameter in inch
};
/// Imperial Standard Wire Gauge (SWG, BS 3737), 7/0 … 50
[[nodiscard]] std::span<const Gauge> swgTable();
inline constexpr double kMmPerInch = 25.4;
inline constexpr double kMm2PerKcmil = 0.5067074790975;  // 1 kcmil = 1000 circular mils

// ---- motors ---------------------------------------------------------------------------------
/// NEC full-load currents (Table 430.248 single-phase, 430.250 three-phase induction motors), nullopt if not listed
[[nodiscard]] std::optional<double> necFullLoadCurrent(int phases, double hp, int volts);
/// Horsepower ratings of the NEC tables (single-phase / three-phase)
[[nodiscard]] std::span<const double> necHorsepowers(int phases);
/// Voltage columns of the NEC tables
[[nodiscard]] std::span<const int> necVoltages(int phases);
inline constexpr double kWattPerHp = 745.6999;

// ---- current-carrying capacity: busbars and MI cables ---------------------------------------
struct Busbar {
    double width, thickness;  // mm
    double area;              // mm²
    double kgPerM;
    double i1;                // A, 1 bar
    double i2;                // A, 2 bars (0 = not listed)
};
/// E-Cu busbars, indoor, 25 °C air, max. 65 °C bar temperature (Striebel & John catalogue, after DIN 43671)
[[nodiscard]] std::span<const Busbar> busbars();

enum class MiConfig { TwoCore, ThreeCore3ph, FourCore3ph, SingleCore1ph, SingleCore3ph, SevenCore, TwelveCore, NineteenCore };
enum class MiDuty { Light500V, Heavy750V };
/// Mineral-insulated copper cables, sheath 70 °C (PVC covered or bare exposed to touch), clipped direct (method C),
/// 30 °C ambient. Values per BS 7671 Tab. 4G1A / IEC 60364-5-52 Tab. B.52.6 (manufacturer data sheet).
[[nodiscard]] std::optional<double> miAmpacity(MiDuty duty, MiConfig cfg, double area);
[[nodiscard]] std::vector<double> miAreas(MiDuty duty, MiConfig cfg);

}  // namespace elc::electrical
