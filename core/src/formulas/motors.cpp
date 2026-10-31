#include "elc/formulas/motors.hpp"

#include <cmath>
#include <complex>

namespace elc::motors {

namespace {
constexpr double kTorqueConst = 60000.0 / (2.0 * kPi);  // 9549.3: M[Nm] = k·P[kW]/n[1/min]
}

double ratedCurrent(System sys, double Pmech, double U, double cosPhi, double eta) {
    if (sys == System::DC) cosPhi = 1.0;
    return Pmech / (powerFactorK(sys) * U * cosPhi * eta);
}

std::optional<TorqueValues> solveTorque(std::optional<double> P, std::optional<double> n, std::optional<double> M) {
    const int known = int{P.has_value()} + int{n.has_value()} + int{M.has_value()};
    if (known != 2) return std::nullopt;
    if (P && n) {
        if (*n == 0) return std::nullopt;
        return TorqueValues{*P, *n, kTorqueConst * *P / *n};
    }
    if (P && M) {
        if (*M == 0) return std::nullopt;
        return TorqueValues{*P, kTorqueConst * *P / *M, *M};
    }
    return TorqueValues{*M * *n / kTorqueConst, *n, *M};
}

StarDelta starDelta(double In, double IaIn, double MaMn) {
    const double dol = In * IaIn;
    return {dol, dol / 3.0, MaMn / 3.0, In / kSqrt3};
}

Steinmetz steinmetz(double P_kW, double U, double f) {
    const double scale = (230.0 / U) * (230.0 / U) * (50.0 / f);
    const double cb = 70.0 * P_kW * scale;
    return {cb, 2.0 * cb, 3.0 * cb, 0.75 * P_kW};
}

}  // namespace elc::motors

namespace elc::industry {

double compensationPower(double P, double cosPhi1, double cosPhi2) {
    const auto tanOf = [](double c) { return std::tan(std::acos(c)); };
    return P * (tanOf(cosPhi1) - tanOf(cosPhi2));
}

double capacitance(double Qc, double U, double f, CapConnection c) {
    const double omega = 2.0 * kPi * f;
    const double divisor = c == CapConnection::Delta ? 3.0 : 1.0;
    return Qc / (divisor * U * U * omega);
}

double transformerCurrent(double S, double U, bool threePhase) { return S / ((threePhase ? kSqrt3 : 1.0) * U); }

ShortCircuitResult shortCircuit(const ShortCircuitInput& in) {
    using C = std::complex<double>;
    constexpr double cMax = 1.05;
    constexpr double cMin = 0.95;
    const double Un = in.Un_V;
    const double U0 = Un / kSqrt3;

    // upstream network (X/R = 10)
    C zQ{0.0, 0.0};
    if (in.SkQ_MVA && *in.SkQ_MVA > 0) {
        const double ZQ = cMax * Un * Un / (*in.SkQ_MVA * 1e6);
        const double XQ = 0.995 * ZQ;
        zQ = C{0.1 * XQ, XQ};
    }
    // transformer
    const double SrT = in.SrT_kVA * 1e3;
    const double ZT = in.uk_pct / 100.0 * Un * Un / SrT;
    const double RT = std::min(in.uR_pct, in.uk_pct) / 100.0 * Un * Un / SrT;
    const C zT{RT, std::sqrt(std::max(0.0, ZT * ZT - RT * RT))};
    // line
    const double x = in.x_mOhm_per_m * 1e-3;
    const double rho20 = cables::resistivity20(in.material);
    const double rho80 = cables::resistivity(in.material, 80.0);
    const C zL20{rho20 * in.L_m / in.Aph_mm2, x * in.L_m};
    const C zL80{rho80 * in.L_m / in.Aph_mm2, x * in.L_m};
    const C zPE80{rho80 * in.L_m / in.Ape_mm2, x * in.L_m};

    ShortCircuitResult r{};
    r.ZQ_mOhm = std::abs(zQ) * 1e3;
    r.ZT_mOhm = std::abs(zT) * 1e3;
    r.ZL_mOhm = std::abs(zL20) * 1e3;
    r.Ik3Trafo_kA = cMax * Un / (kSqrt3 * std::abs(zQ + zT)) / 1e3;
    r.Ik3End_kA = cMax * Un / (kSqrt3 * std::abs(zQ + zT + zL20)) / 1e3;
    // Dyn transformer: Z0T ≈ Z1T, no zero-sequence path to the network ⇒ only 2/3·ZQ in the loop
    r.Ik1End_kA = cMin * U0 / std::abs(zQ * (2.0 / 3.0) + zT + zL80 + zPE80) / 1e3;
    return r;
}

double enclosureSurface(double H, double W, double D, bool wallMounted) {
    if (wallMounted) return 1.4 * W * (H + D) + 1.8 * D * H;
    return 1.8 * H * (W + D) + 1.4 * W * D;
}

}  // namespace elc::industry
