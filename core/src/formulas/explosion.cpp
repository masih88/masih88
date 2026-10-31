#include "elc/formulas/explosion.hpp"

#include <algorithm>
#include <array>

namespace elc::ex {

namespace {
constexpr std::array<double, 6> kLimits{450, 300, 200, 135, 100, 85};

int rank(std::string_view g) {
    if (g == "IIA" || g == "IIIA") return 1;
    if (g == "IIB" || g == "IIIB") return 2;
    if (g == "IIC" || g == "IIIC") return 3;
    return 0;
}
}  // namespace

int temperatureClass(double t) {
    for (int c = 1; c <= 6; ++c) {
        if (t > kLimits[static_cast<std::size_t>(c - 1)]) return c;
    }
    return 0;
}

double classLimit(int cls) { return cls >= 1 && cls <= 6 ? kLimits[static_cast<std::size_t>(cls - 1)] : 0.0; }

double dustMaxSurface(double cloud, double layer) { return std::min(2.0 / 3.0 * cloud, layer - 75.0); }

bool groupCovers(std::string_view device, std::string_view required) {
    const bool devDust = device.size() == 4;
    const bool reqDust = required.size() == 4;
    return devDust == reqDust && rank(device) >= rank(required) && rank(required) > 0;
}

}  // namespace elc::ex
