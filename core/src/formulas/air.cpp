#include "elc/formulas/air.hpp"

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

double co2Airflow(double persons, double g, double target, double outdoor) {
    return persons * g / 1000.0 / ((target - outdoor) * 1e-6);
}

double co2Concentration(double persons, double g, double q, double outdoor) {
    return outdoor + persons * g / 1000.0 / q * 1e6;
}

}  // namespace elc::air
