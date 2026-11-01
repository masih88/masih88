#include "elc/formulas/heating.hpp"

#include <algorithm>
#include <cmath>

namespace elc::heating {

double flowTemperature(const Design& d, double ta, double tiSet) {
    const double q = std::max(0.0, (tiSet - ta) / (d.ti - d.ta));
    const double tmExcess = 0.5 * (d.tv + d.tr) - d.ti;
    return tiSet + tmExcess * std::pow(q, 1.0 / d.n) + 0.5 * (d.tv - d.tr) * q;
}

}  // namespace elc::heating

namespace elc::heatpump {

namespace {
constexpr double kKelvin = 273.15;
}

double carnotHeating(double tSinkC, double tSourceC) {
    const double hot = tSinkC + kKelvin;
    return hot / (tSinkC - tSourceC);
}

double carnotCooling(double tEvapC, double tCondC) {
    const double cold = tEvapC + kKelvin;
    return cold / (tCondC - tEvapC);
}

}  // namespace elc::heatpump

namespace elc::valves {

double minFlowRatio(double authority, double rangeability) {
    return 1.0 / std::sqrt(1.0 + authority * (rangeability * rangeability - 1.0));
}

double heatRatio(double flowRatio, double aHx) {
    if (flowRatio <= 0) return 0;
    return 1.0 / (1.0 + aHx * (1.0 / flowRatio - 1.0));
}

}  // namespace elc::valves
