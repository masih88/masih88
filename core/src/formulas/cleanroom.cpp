#include "elc/formulas/cleanroom.hpp"

#include <array>
#include <cmath>

namespace elc::cleanroom {

double classLimitExact(double isoClass, double particleSize) {
    return std::pow(10.0, isoClass) * std::pow(0.1 / particleSize, 2.08);
}

double classLimit(double isoClass, double particleSize) {
    const double whole = std::round(classLimitExact(isoClass, particleSize));
    if (whole < 1000.0) return whole;
    const double magnitude = std::pow(10.0, std::floor(std::log10(whole)) - 2.0);  // keep 3 significant figures
    return std::round(whole / magnitude) * magnitude;
}

double classFromConcentration(double concentration, double particleSize) {
    return std::log10(concentration) + 2.08 * std::log10(particleSize / 0.1);
}

bool classLimitTabulated(int isoClass, double particleSize) {
    // Table 1 of ISO 14644-1:2015: sizes 0.1, 0.2, 0.3, 0.5, 1, 5 µm
    const auto idx = [&]() -> int {
        constexpr std::array<double, 6> sizes{0.1, 0.2, 0.3, 0.5, 1.0, 5.0};
        for (int i = 0; i < 6; ++i) {
            if (std::fabs(sizes[static_cast<std::size_t>(i)] - particleSize) < 1e-9) return i;
        }
        return -1;
    }();
    if (idx < 0 || isoClass < 1 || isoClass > 9) return false;
    switch (isoClass) {
        case 1: return idx <= 1;
        case 2: return idx <= 3;
        case 3: return idx <= 4;
        case 4:
        case 5: return idx <= 4;
        case 6: return true;
        default: return idx >= 3;  // ISO 7–9: 0.5 µm and larger
    }
}

int samplingLocations(double area) {
    struct Step {
        double maxArea;
        int locations;
    };
    static constexpr std::array<Step, 28> kTable{{
        {2, 1},    {4, 2},    {6, 3},    {8, 4},    {10, 5},   {24, 6},   {28, 7},
        {32, 8},   {36, 9},   {52, 10},  {56, 11},  {64, 12},  {68, 13},  {72, 14},
        {76, 15},  {104, 16}, {108, 17}, {116, 18}, {148, 19}, {156, 20}, {192, 21},
        {232, 22}, {276, 23}, {352, 24}, {436, 25}, {636, 26}, {1000, 27}, {1000, 27},
    }};
    for (const auto& s : kTable) {
        if (area <= s.maxArea) return s.locations;
    }
    return static_cast<int>(std::ceil(27.0 * area / 1000.0));
}

double sampleVolumeLitres(double classLimitLargestSize) {
    return std::max(2.0, 20.0 / classLimitLargestSize * 1000.0);
}

double recoveryTimeHours(double n, double factor) { return std::log(factor) / n; }

double leakageFlow(double area, double dp, double cd, double rho) { return cd * area * std::sqrt(2.0 * dp / rho); }

}  // namespace elc::cleanroom
