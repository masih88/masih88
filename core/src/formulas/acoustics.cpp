#include "elc/formulas/acoustics.hpp"

#include <cmath>

namespace elc::acoustics {

double sabine(double volume, double absorption) { return 0.163 * volume / absorption; }

double absorptionFor(double volume, double T) { return 0.163 * volume / T; }

double din18041Target(Use use, double volume) {
    const double lg = std::log10(volume);
    switch (use) {
    case Use::Music: return 0.45 * lg + 0.07;
    case Use::Speech: return 0.37 * lg - 0.14;
    case Use::Teaching: return 0.32 * lg - 0.17;
    case Use::TeachingInclusive: return 0.26 * lg - 0.14;
    }
    return 0;
}

double levelSum(std::span<const double> levels) {
    double sum = 0;
    for (const double l : levels) sum += std::pow(10.0, l / 10.0);
    return 10.0 * std::log10(sum);
}

double distanceLevel(double l1, double r1, double r2) { return l1 - 20.0 * std::log10(r2 / r1); }

}  // namespace elc::acoustics
