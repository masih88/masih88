#include "elc/formulas/pv.hpp"

#include <cmath>

namespace elc::pv {

StringLimits stringLimits(const StringInput& in) {
    StringLimits r{};
    r.vocMin = atTemperature(in.voc, in.betaVoc, in.tMin);
    r.vmpMin = atTemperature(in.vmp, in.betaVmp, in.tMin);
    r.vmpMax = atTemperature(in.vmp, in.betaVmp, in.tMax);
    r.maxByVoltage = static_cast<int>(std::floor(in.udcMax / r.vocMin + 1e-9));
    r.maxByMpp = static_cast<int>(std::floor(in.umppMax / r.vmpMin + 1e-9));
    r.minByMpp = static_cast<int>(std::ceil(in.umppMin / r.vmpMax - 1e-9));
    return r;
}

bool stringFuseRequired(int parallelStrings, double isc, double reverseCurrentRating) {
    return (parallelStrings - 1) * 1.25 * isc > reverseCurrentRating;
}

}  // namespace elc::pv
