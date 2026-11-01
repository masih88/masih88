#include "elc/formulas/control.hpp"

namespace elc::control {

Difficulty classify(double S) noexcept {
    if (S < 0.1) return Difficulty::Easy;
    if (S <= 0.3) return Difficulty::Medium;
    return Difficulty::Hard;
}

namespace {

/// Ziegler/Nichols, Fig. 6-15 (lower table, Kp form)
Tuning zieglerNichols(Controller c, double ks, double tt, double T) {
    const double base = T / (ks * tt);
    switch (c) {
    case Controller::P: return {base, kNaN, kNaN};
    case Controller::PI: return {0.9 * base, 3.3 * tt, kNaN};
    case Controller::PD: return {1.2 * base, kNaN, 0.25 * tt};
    case Controller::PID: return {1.2 * base, 2.0 * tt, 0.42 * tt};
    }
    return {};
}

struct ChrRow {
    double v0;          // V0 = factor · Tg/Tu
    double tnFactor;    // Tn = tnFactor · (tnOnTg ? Tg : Tu)
    bool tnOnTg;
    double tvFactor;    // Tv = tvFactor · Tu
};

/// Chien/Hrones/Reswick, Fig. 6-18: [rule][P, PI, PID]
std::optional<ChrRow> chrRow(Rule rule, Controller c) {
    if (c == Controller::PD) return std::nullopt;
    const int i = c == Controller::P ? 0 : (c == Controller::PI ? 1 : 2);
    static constexpr ChrRow aperDist[3] = {{0.3, 0, false, 0}, {0.6, 4.0, false, 0}, {0.95, 2.4, false, 0.42}};
    static constexpr ChrRow aperSet[3] = {{0.3, 0, false, 0}, {0.35, 1.2, true, 0}, {0.6, 1.0, true, 0.5}};
    static constexpr ChrRow os20Dist[3] = {{0.7, 0, false, 0}, {0.7, 2.3, false, 0}, {1.2, 2.0, false, 0.42}};
    static constexpr ChrRow os20Set[3] = {{0.7, 0, false, 0}, {0.6, 1.0, true, 0}, {0.95, 1.35, true, 0.47}};
    switch (rule) {
    case Rule::ChrAperiodicDisturbance: return aperDist[i];
    case Rule::ChrAperiodicSetpoint: return aperSet[i];
    case Rule::Chr20Disturbance: return os20Dist[i];
    case Rule::Chr20Setpoint: return os20Set[i];
    case Rule::ZieglerNichols: break;
    }
    return std::nullopt;
}

}  // namespace

std::optional<Tuning> tune(Rule rule, Controller c, double ks, double tu, double tg) {
    if (!(ks > 0) || !(tu > 0) || !(tg > 0)) return std::nullopt;
    if (rule == Rule::ZieglerNichols) return zieglerNichols(c, ks, tu, tg);
    const auto row = chrRow(rule, c);
    if (!row) return std::nullopt;
    Tuning t;
    t.kp = row->v0 * tg / tu / ks;
    if (c != Controller::P) t.tn = row->tnFactor * (row->tnOnTg ? tg : tu);
    if (c == Controller::PID) t.tv = row->tvFactor * tu;
    return t;
}

}  // namespace elc::control
