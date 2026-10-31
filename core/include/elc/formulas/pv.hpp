#pragma once
// Photovoltaics: temperature-corrected string design, yield, storage.

#include "elc/formulas/common.hpp"

namespace elc::pv {

/// Value at cell temperature T for a temperature coefficient in %/K (STC = 25 °C):
/// X(T) = X_STC · (1 + β/100 · (T − 25))
[[nodiscard]] constexpr double atTemperature(double valueStc, double betaPercentPerK, double cellTempC) noexcept {
    return valueStc * (1.0 + betaPercentPerK / 100.0 * (cellTempC - 25.0));
}

struct StringLimits {
    double vocMin;      // Voc at T_min (per module)
    double vmpMin;      // Vmp at T_min
    double vmpMax;      // Vmp at T_max (lowest MPP voltage)
    int maxByVoltage;   // floor(U_dc,max / Voc(T_min))
    int maxByMpp;       // floor(U_mpp,max / Vmp(T_min))
    int minByMpp;       // ceil(U_mpp,min / Vmp(T_max))
};

struct StringInput {
    double voc, vmp;            // module STC values [V]
    double betaVoc, betaVmp;    // [%/K], negative
    double tMin, tMax;          // cell temperatures [°C]
    double udcMax, umppMin, umppMax;  // inverter [V]
};

[[nodiscard]] StringLimits stringLimits(const StringInput& in);

/// Number of parallel strings above which string fuses are needed: (Np − 1) · 1.25 · Isc > I_R
[[nodiscard]] bool stringFuseRequired(int parallelStrings, double isc, double reverseCurrentRating);

}  // namespace elc::pv
