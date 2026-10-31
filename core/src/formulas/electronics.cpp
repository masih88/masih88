#include "elc/formulas/electronics.hpp"

#include <array>
#include <cmath>

namespace elc::electronics {

namespace {

constexpr std::array<double, 6> kE6{10, 15, 22, 33, 47, 68};
constexpr std::array<double, 12> kE12{10, 12, 15, 18, 22, 27, 33, 39, 47, 56, 68, 82};
constexpr std::array<double, 24> kE24{10, 11, 12, 13, 15, 16, 18, 20, 22, 24, 27, 30,
                                      33, 36, 39, 43, 47, 51, 56, 62, 68, 75, 82, 91};
constexpr std::array<double, 48> kE48{100, 105, 110, 115, 121, 127, 133, 140, 147, 154, 162, 169,
                                      178, 187, 196, 205, 215, 226, 237, 249, 261, 274, 287, 301,
                                      316, 332, 348, 365, 383, 402, 422, 442, 464, 487, 511, 536,
                                      562, 590, 619, 649, 681, 715, 750, 787, 825, 866, 909, 953};
constexpr std::array<double, 96> kE96{
    100, 102, 105, 107, 110, 113, 115, 118, 121, 124, 127, 130, 133, 137, 140, 143, 147, 150, 154, 158,
    162, 165, 169, 174, 178, 182, 187, 191, 196, 200, 205, 210, 215, 221, 226, 232, 237, 243, 249, 255,
    261, 267, 274, 280, 287, 294, 301, 309, 316, 324, 332, 340, 348, 357, 365, 374, 383, 392, 402, 412,
    422, 432, 442, 453, 464, 475, 487, 499, 511, 523, 536, 549, 562, 576, 590, 604, 619, 634, 649, 665,
    681, 698, 715, 732, 750, 768, 787, 806, 825, 845, 866, 887, 909, 931, 953, 976};

}  // namespace

std::span<const double> eSeriesBase(ESeries s) {
    switch (s) {
        case ESeries::E6: return kE6;
        case ESeries::E12: return kE12;
        case ESeries::E24: return kE24;
        case ESeries::E48: return kE48;
        case ESeries::E96: return kE96;
    }
    return kE12;
}

std::vector<double> eSeriesValues(ESeries s) {
    const auto base = eSeriesBase(s);
    const double baseDecade = base.front();  // 10 or 100
    std::vector<double> out;
    for (int exp = -1; exp <= 9; ++exp) {
        const double f = std::pow(10.0, exp) / baseDecade;
        for (const double b : base) out.push_back(b * f);
    }
    return out;
}

ESeriesMatch eSeriesMatch(ESeries s, double value) {
    const auto values = eSeriesValues(s);
    const double lo = nextAtMost(values, value).value_or(values.front());
    const double hi = nextAtLeast(values, value).value_or(values.back());
    return {lo, hi, nearestLog(values, value)};
}

double cutoffFrequency(double R, double C) { return 1.0 / (2.0 * kPi * R * C); }

double chargingVoltage(double U0, double t, double tau) { return U0 * (1.0 - std::exp(-t / tau)); }

double resonanceFrequency(double L, double C) { return 1.0 / (2.0 * kPi * std::sqrt(L * C)); }

double inductiveReactance(double f, double L) { return 2.0 * kPi * f * L; }

double capacitiveReactance(double f, double C) { return 1.0 / (2.0 * kPi * f * C); }

}  // namespace elc::electronics
