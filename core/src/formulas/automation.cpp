#include "elc/formulas/automation.hpp"

#include <cmath>
#include <stdexcept>

namespace elc::automation {

namespace {

constexpr double kPtA = 3.9083e-3;
constexpr double kPtB = -5.775e-7;
constexpr double kPtC = -4.183e-12;

// DIN 43760 Ni1000 (TK 6180)
constexpr double kNiA = 5.485e-3;
constexpr double kNiB = 6.65e-6;
constexpr double kNiD = 2.805e-11;
constexpr double kNiF = -2.0e-17;

// LG-Ni 1000 (TK 5000)
constexpr double kLgA = 4.427e-3;
constexpr double kLgB = 5.172e-6;
constexpr double kLgC = 5.585e-9;

constexpr double kT25 = 298.15;

}  // namespace

double copperAlpha(Rtd type) { return type == Rtd::Cu10 ? 0.00427 : 0.00428; }

double rtdR0(Rtd type) {
    switch (type) {
        case Rtd::Pt100: return 100.0;
        case Rtd::Pt500: return 500.0;
        case Rtd::Cu10: return 10.0;
        case Rtd::Cu50: return 50.0;
        case Rtd::Cu100: return 100.0;
        default: return 1000.0;
    }
}

std::array<double, 2> rtdRange(Rtd type) {
    switch (type) {
        case Rtd::Ni1000: return {-60.0, 250.0};
        case Rtd::Ni1000TK5000:
        case Rtd::Cu10:
        case Rtd::Cu50:
        case Rtd::Cu100: return {-50.0, 150.0};
        default: return {-200.0, 850.0};
    }
}

double rtdResistance(Rtd type, double T) {
    const double R0 = rtdR0(type);
    switch (type) {
        case Rtd::Pt100:
        case Rtd::Pt500:
        case Rtd::Pt1000: {
            double r = 1.0 + kPtA * T + kPtB * T * T;
            if (T < 0.0) r += kPtC * (T - 100.0) * T * T * T;
            return R0 * r;
        }
        case Rtd::Ni1000:
            return R0 * (1.0 + kNiA * T + kNiB * T * T + kNiD * std::pow(T, 4) + kNiF * std::pow(T, 6));
        case Rtd::Ni1000TK5000:
            return R0 * (1.0 + kLgA * T + kLgB * T * T + kLgC * T * T * T);
        case Rtd::Cu10:
        case Rtd::Cu50:
        case Rtd::Cu100:
            return R0 * (1.0 + copperAlpha(type) * T);
    }
    return kNaN;
}

double rtdSensitivity(Rtd type, double T) {
    const double R0 = rtdR0(type);
    switch (type) {
        case Rtd::Pt100:
        case Rtd::Pt500:
        case Rtd::Pt1000: {
            double d = kPtA + 2.0 * kPtB * T;
            if (T < 0.0) d += kPtC * (4.0 * T * T * T - 300.0 * T * T);
            return R0 * d;
        }
        case Rtd::Ni1000:
            return R0 * (kNiA + 2.0 * kNiB * T + 4.0 * kNiD * std::pow(T, 3) + 6.0 * kNiF * std::pow(T, 5));
        case Rtd::Ni1000TK5000:
            return R0 * (kLgA + 2.0 * kLgB * T + 3.0 * kLgC * T * T);
        case Rtd::Cu10:
        case Rtd::Cu50:
        case Rtd::Cu100:
            return R0 * copperAlpha(type);
    }
    return kNaN;
}

double rtdTemperature(Rtd type, double R) {
    // All characteristics are strictly increasing in their valid range: bracket + Newton.
    const auto [lo0, hi0] = rtdRange(type);
    double lo = lo0 - 50.0;
    double hi = hi0 + 50.0;
    double T = (R / rtdR0(type) - 1.0) / 0.004;  // linear first guess
    for (int i = 0; i < 100; ++i) {
        const double f = rtdResistance(type, T) - R;
        if (std::fabs(f) < 1e-10 * R) return T;
        if (f > 0) hi = std::min(hi, T); else lo = std::max(lo, T);
        double next = T - f / rtdSensitivity(type, T);
        if (!(next > lo && next < hi)) next = 0.5 * (lo + hi);  // bisection fallback
        T = next;
    }
    return T;
}

double ntcResistance(double R25, double B, double T_C) {
    const double T = T_C + 273.15;
    return R25 * std::exp(B * (1.0 / T - 1.0 / kT25));
}

double ntcTemperature(double R25, double B, double R) {
    if (R <= 0.0) throw std::domain_error("NTC resistance must be positive");
    return 1.0 / (1.0 / kT25 + std::log(R / R25) / B) - 273.15;
}

double kvValue(double Q_m3h, double dp_kPa, double rho) { return Q_m3h * std::sqrt((rho / 1000.0) / (dp_kPa / 100.0)); }

double pressureDrop(double Q_m3h, double kvs, double rho) {
    const double ratio = Q_m3h / kvs;
    return ratio * ratio * (rho / 1000.0) * 100.0;
}

double volumeFlowFromPower(double P_kW, double dT, double rho, double c) { return P_kW * 3600.0 / (rho * c * dT); }

double powerFromVolumeFlow(double V_m3h, double dT, double rho, double c) { return V_m3h * rho * c * dT / 3600.0; }

}  // namespace elc::automation
