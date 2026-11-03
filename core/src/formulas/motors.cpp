#include "elc/formulas/motors.hpp"

#include <cmath>
#include <limits>
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

namespace {
constexpr double kNo = std::numeric_limits<double>::quiet_NaN();
}  // namespace

const std::vector<MotorGuide>& motorGuideTable() {
    // clang-format off
    static const std::vector<MotorGuide> t{
        {0.06, 0.70, 58, 0.37, 2, kNo, 0.21, 2, kNo},     {0.09, 0.70, 60, 0.54, 2, kNo, 0.31, 2, kNo},
        {0.12, 0.70, 60, 0.72, 4, 2, 0.41, 2, kNo},       {0.18, 0.70, 62, 1.04, 4, 2, 0.6, 2, kNo},
        {0.25, 0.70, 62, 1.4, 4, 2, 0.8, 4, 2},           {0.37, 0.72, 66, 2, 6, 4, 1.1, 4, 2},
        {0.55, 0.75, 69, 2.7, 10, 4, 1.5, 4, 2},          {0.75, 0.79, 74, 3.2, 10, 4, 1.9, 6, 4},
        {1.1, 0.81, 74, 4.6, 10, 6, 2.6, 6, 4},           {1.5, 0.81, 74, 6.3, 16, 10, 3.6, 6, 4},
        {2.2, 0.81, 78, 8.7, 20, 10, 5, 10, 6},           {3, 0.82, 80, 11.5, 25, 16, 6.6, 16, 10},
        {4, 0.82, 83, 14.8, 32, 16, 8.5, 20, 10},         {5.5, 0.82, 86, 19.6, 32, 25, 11.3, 25, 16},
        {7.5, 0.82, 87, 26.4, 50, 32, 15.2, 32, 16},      {11, 0.84, 87, 38, 80, 40, 21.7, 40, 25},
        {15, 0.84, 88, 51, 100, 63, 29.3, 63, 32},        {18.5, 0.84, 88, 63, 125, 80, 36, 63, 40},
        {22, 0.84, 92, 71, 125, 80, 41, 80, 50},          {30, 0.85, 92, 96, 200, 100, 55, 100, 63},
        {37, 0.86, 92, 117, 200, 125, 68, 125, 80},       {45, 0.86, 93, 141, 250, 160, 81, 160, 100},
        {55, 0.86, 93, 173, 250, 200, 99, 200, 125},      {75, 0.86, 94, 233, 315, 250, 134, 200, 160},
        {90, 0.86, 94, 279, 400, 315, 161, 250, 200},     {110, 0.86, 94, 342, 500, 400, 196, 315, 200},
        {132, 0.87, 95, 401, 630, 500, 231, 400, 250},    {160, 0.87, 95, 486, 630, 630, 279, 400, 315},
        {200, 0.87, 95, 607, 800, 630, 349, 500, 400},    {250, 0.87, 95, kNo, kNo, kNo, 437, 630, 500},
        {315, 0.87, 96, kNo, kNo, kNo, 544, 800, 630},    {400, 0.88, 96, kNo, kNo, kNo, 683, 1000, 800},
        {450, 0.88, 96, kNo, kNo, kNo, 769, 1000, 800},
    };
    // clang-format on
    return t;
}

std::optional<MotorGuide> motorGuide(double P_kW) {
    for (const auto& m : motorGuideTable()) {
        if (m.P_kW >= P_kW * (1 - 1e-9)) return m;
    }
    return std::nullopt;
}

std::optional<SettingRange> mpcbRange(double Ir) {
    static const std::vector<SettingRange> r{{0.1, 0.16}, {0.16, 0.25}, {0.25, 0.4}, {0.4, 0.63}, {0.63, 1},  {1, 1.6},
                                             {1.6, 2.5},  {2.5, 4},     {4, 6.3},    {6.3, 10},   {8, 12},    {10, 16},
                                             {16, 20},    {20, 25},     {25, 32},    {32, 40},    {40, 50},   {50, 58},
                                             {55, 65}};
    // overlapping ranges: take the one with the smallest upper limit (I_r in the upper part of the scale)
    for (const auto& x : r) {
        if (Ir >= x.lo - 1e-9 && Ir <= x.hi + 1e-9) return x;
    }
    return std::nullopt;
}

int tripClassFor(double runUp_s) {
    if (runUp_s < 4.0) return 10;
    if (runUp_s < 6.0) return 20;
    if (runUp_s < 9.0) return 30;
    return 0;
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
