#include "elc/formulas/basics.hpp"

#include <cmath>
#include <numeric>

namespace elc::basics {

PowerTriangle powerFromVoltageCurrent(System sys, double U, double I, double cosPhi) {
    if (sys == System::DC) cosPhi = 1.0;
    const double S = powerFactorK(sys) * U * I;
    const double phi = std::acos(std::clamp(cosPhi, -1.0, 1.0));
    return {S * cosPhi, S * std::sin(phi), S, phi * 180.0 / kPi};
}

double currentFromActivePower(System sys, double P, double U, double cosPhi, double eta) {
    if (sys == System::DC) cosPhi = 1.0;
    return P / (powerFactorK(sys) * U * cosPhi * eta);
}

std::optional<OhmValues> solveOhm(std::optional<double> U, std::optional<double> I, std::optional<double> R,
                                  std::optional<double> P) {
    const int known = int{U.has_value()} + int{I.has_value()} + int{R.has_value()} + int{P.has_value()};
    if (known != 2) return std::nullopt;

    OhmValues v{};
    if (U && I) {
        if (*I == 0) return std::nullopt;
        v = {*U, *I, *U / *I, *U * *I};
    } else if (U && R) {
        if (*R == 0) return std::nullopt;
        v = {*U, *U / *R, *R, *U * *U / *R};
    } else if (U && P) {
        if (*U == 0 || *P == 0) return std::nullopt;
        v = {*U, *P / *U, *U * *U / *P, *P};
    } else if (I && R) {
        v = {*I * *R, *I, *R, *I * *I * *R};
    } else if (I && P) {
        if (*I == 0) return std::nullopt;
        v = {*P / *I, *I, *P / (*I * *I), *P};
    } else {  // R && P
        if (*R <= 0 || *P < 0) return std::nullopt;
        v = {std::sqrt(*P * *R), std::sqrt(*P / *R), *R, *P};
    }
    return v;
}

double directSum(std::span<const double> values) { return std::accumulate(values.begin(), values.end(), 0.0); }

double reciprocalSum(std::span<const double> values) {
    double s = 0.0;
    for (const double v : values) s += 1.0 / v;
    return s == 0.0 ? kNaN : 1.0 / s;
}

DividerResult voltageDivider(double Uin, double R1, double R2, std::optional<double> RL) {
    DividerResult r{};
    r.UoutUnloaded = Uin * R2 / (R1 + R2);
    const double R2eff = (RL && *RL > 0) ? R2 * *RL / (R2 + *RL) : R2;
    r.UoutLoaded = Uin * R2eff / (R1 + R2eff);
    r.I1 = Uin / (R1 + R2eff);
    r.P1 = r.I1 * r.I1 * R1;
    r.P2 = r.UoutLoaded * r.UoutLoaded / R2;
    return r;
}

}  // namespace elc::basics
