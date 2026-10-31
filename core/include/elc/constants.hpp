#pragma once

#include <limits>
#include <numbers>

namespace elc {

inline constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
inline constexpr double kInf = std::numeric_limits<double>::infinity();
inline constexpr double kPi = std::numbers::pi;
inline constexpr double kSqrt3 = std::numbers::sqrt3;

}  // namespace elc
