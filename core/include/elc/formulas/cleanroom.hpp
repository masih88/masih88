#pragma once
// Cleanrooms: ISO 14644-1 classification, sampling, air change, FFU ceilings, overflow.

#include <optional>

#include "elc/formulas/common.hpp"

namespace elc::cleanroom {

/// ISO 14644-1:2015 class limit C_n = 10^N · (0.1/D)^2.08 [particles/m³], D in µm (unrounded)
[[nodiscard]] double classLimitExact(double isoClass, double particleSize);
/// Same, rounded like Table 1: whole number, at most three significant figures
[[nodiscard]] double classLimit(double isoClass, double particleSize);
/// Class number from a measured concentration: N = log10(C) + 2.08 · log10(D/0.1)
[[nodiscard]] double classFromConcentration(double concentration, double particleSize);
/// True where ISO 14644-1:2015 Table 1 lists a value (others are not applicable)
[[nodiscard]] bool classLimitTabulated(int isoClass, double particleSize);

/// Minimum number of sampling locations N_L, ISO 14644-1:2015 Table A.1
[[nodiscard]] int samplingLocations(double area_m2);
/// Minimum single sample volume V_s = 20 / C_n,m · 1000 [l], at least 2 l (ISO 14644-1 A.4.4)
[[nodiscard]] double sampleVolumeLitres(double classLimitLargestSize);

/// Time to reduce a concentration by `factor` with ideal mixing at n air changes per hour [h]
[[nodiscard]] double recoveryTimeHours(double airChangesPerHour, double factor = 100.0);

/// Flow through a leakage area: Q = C_d · A · √(2·Δp/ρ)  [m³/s]
[[nodiscard]] double leakageFlow(double area_m2, double dp_Pa, double cd = 0.61, double rho = 1.2);

}  // namespace elc::cleanroom
