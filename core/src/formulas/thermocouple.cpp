#include "elc/formulas/thermocouple.hpp"

#include <cmath>

#include "thermocouple_nist.hpp"

namespace elc::thermocouple {

namespace {

const std::vector<detail::Range>& rangesOf(Type t) {
    switch (t) {
        case Type::B: return detail::rangesB();
        case Type::E: return detail::rangesE();
        case Type::J: return detail::rangesJ();
        case Type::N: return detail::rangesN();
        case Type::R: return detail::rangesR();
        case Type::S: return detail::rangesS();
        case Type::T: return detail::rangesT();
        case Type::K: break;
    }
    return detail::rangesK();
}

double evaluate(const detail::Range& r, double T) {
    double e = 0.0;
    for (auto it = r.c.rbegin(); it != r.c.rend(); ++it) e = e * T + *it;  // Horner, c[0] = constant term
    if (r.gauss) e += r.a0 * std::exp(r.a1 * (T - r.a2) * (T - r.a2));
    return e;
}

}  // namespace

std::pair<double, double> range(Type t) {
    const auto& r = rangesOf(t);
    return {r.front().tMin, r.back().tMax};
}

std::optional<double> emf(Type t, double celsius) {
    const auto& ranges = rangesOf(t);
    if (!std::isfinite(celsius) || celsius < ranges.front().tMin || celsius > ranges.back().tMax) return std::nullopt;
    for (const auto& r : ranges) {
        if (celsius <= r.tMax) return evaluate(r, celsius);
    }
    return evaluate(ranges.back(), celsius);
}

std::optional<double> temperature(Type t, double mV) {
    // E(θ) rises monotonically over each defined range (type B only above ≈ 21 °C, where E ≈ 0): bisection.
    auto [lo, hi] = range(t);
    if (t == Type::B) lo = 21.0;
    const double eLo = *emf(t, lo);
    const double eHi = *emf(t, hi);
    if (!std::isfinite(mV) || mV < eLo || mV > eHi) return std::nullopt;
    for (int i = 0; i < 200 && hi - lo > 1e-9; ++i) {
        const double mid = 0.5 * (lo + hi);
        if (*emf(t, mid) < mV) lo = mid;
        else hi = mid;
    }
    return 0.5 * (lo + hi);
}

Type parse(std::string_view s) {
    if (s == "B") return Type::B;
    if (s == "E") return Type::E;
    if (s == "J") return Type::J;
    if (s == "N") return Type::N;
    if (s == "R") return Type::R;
    if (s == "S") return Type::S;
    if (s == "T") return Type::T;
    return Type::K;
}

}  // namespace elc::thermocouple
