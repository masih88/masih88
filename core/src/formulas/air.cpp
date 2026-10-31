#include "elc/formulas/air.hpp"

#include <algorithm>
#include <cmath>

namespace elc::air {

namespace {
constexpr double kA = 17.62;
constexpr double kB = 243.12;
constexpr double kE0 = 6.112;
}  // namespace

double saturationPressure(double t) { return kE0 * std::exp(kA * t / (kB + t)); }

double dewPoint(double t, double rh) {
    const double g = std::log(rh / 100.0) + kA * t / (kB + t);
    return kB * g / (kA - g);
}

double relativeHumidity(double t, double td) { return 100.0 * saturationPressure(td) / saturationPressure(t); }

double humidityRatio(double pv, double p) { return 0.622 * pv / (p - pv); }

double enthalpy(double t, double x) { return 1.006 * t + x * (2501.0 + 1.86 * t); }

double density(double t, double pv, double p) {
    const double T = t + 273.15;
    return (p - pv) * 100.0 / (287.06 * T) + pv * 100.0 / (461.5 * T);
}

double pressureAtAltitude(double h) { return 1013.25 * std::pow(1.0 - 0.0065 * h / 288.15, 5.255); }

double saturationRatio(double t, double p) { return humidityRatio(saturationPressure(t), p); }

double ratioFromRh(double t, double rh, double p) { return humidityRatio(saturationPressure(t) * rh / 100.0, p); }

double rhFromRatio(double t, double x, double p) {
    const double pv = x * p / (0.622 + x);
    return 100.0 * pv / saturationPressure(t);
}

double dewPointFromRatio(double x, double p) {
    const double pv = x * p / (0.622 + x);
    // inverse Magnus: τ = B·ln(pv/E0) / (A − ln(pv/E0))
    const double g = std::log(pv / kE0);
    return kB * g / (kA - g);
}

double temperatureFromHX(double h, double x) { return (h - 2501.0 * x) / (1.006 + 1.86 * x); }

namespace {

/// Root of a monotonic function by bisection on [lo, hi] (sign change required)
template <typename F>
double bisect(F f, double lo, double hi) {
    double flo = f(lo);
    for (int i = 0; i < 100; ++i) {
        const double mid = 0.5 * (lo + hi);
        const double fm = f(mid);
        if ((fm < 0) == (flo < 0)) {
            lo = mid;
            flo = fm;
        } else {
            hi = mid;
        }
    }
    return 0.5 * (lo + hi);
}

constexpr double kCw = 4.19;  // specific heat of liquid water [kJ/(kg·K)]

}  // namespace

double wetBulb(double t, double x, double p) {
    const double h = enthalpy(t, x);
    // adiabatic saturation: h + (x_s(tw) − x)·c_w·tw = h_s(tw)
    const auto f = [&](double tw) {
        const double xs = saturationRatio(tw, p);
        return enthalpy(tw, xs) - h - (xs - x) * kCw * tw;
    };
    if (f(t) <= 0) return t;  // saturated (or supersaturated)
    return bisect(f, -60.0, t);
}

AirState coolOrHeat(AirState s, double t2, double p) {
    const double xs = saturationRatio(t2, p);
    return {t2, std::min(s.x, xs)};
}

AirState steamHumidify(AirState s, double rh2, double p) { return {s.t, std::max(s.x, ratioFromRh(s.t, rh2, p))}; }

AirState adiabaticHumidify(AirState s, double rh2, double p) {
    const double h = enthalpy(s.t, s.x);
    if (rhFromRatio(s.t, s.x, p) >= rh2) return s;
    // along h = const: find x with rh(t(h, x), x) = rh2; upper bound is the saturation point on this isenthalp
    const double tSat = bisect([&](double t) { return enthalpy(t, saturationRatio(t, p)) - h; }, -60.0, s.t);
    const double xMax = saturationRatio(tSat, p);
    const double x2 = bisect([&](double x) { return rhFromRatio(temperatureFromHX(h, x), x, p) - rh2; }, s.x, xMax);
    return {temperatureFromHX(h, x2), x2};
}

AirState mix(AirState a, AirState b, double fb) {
    const double x = (1.0 - fb) * a.x + fb * b.x;
    const double h = (1.0 - fb) * enthalpy(a.t, a.x) + fb * enthalpy(b.t, b.x);
    return {temperatureFromHX(h, x), x};
}

double mollierY(double t, double x) { return t * (1.006 + 1.86 * x) / 1.006; }

double mollierT(double y, double x) { return y * 1.006 / (1.006 + 1.86 * x); }

double co2Airflow(double persons, double g, double target, double outdoor) {
    return persons * g / 1000.0 / ((target - outdoor) * 1e-6);
}

double co2Concentration(double persons, double g, double q, double outdoor) {
    return outdoor + persons * g / 1000.0 / q * 1e6;
}

}  // namespace elc::air
