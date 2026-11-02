#pragma once
// Unit conversion. Each unit is a factor to the SI base unit of its quantity.

#include <span>
#include <string_view>

#include "elc/formulas/common.hpp"
#include "elc/types.hpp"

namespace elc::units {

struct Unit {
    std::string_view id;     // also used as choice value and output id
    std::string_view symbol;
    Text name;
    double toBase;           // value_in_base = value · toBase
};

[[nodiscard]] std::span<const Unit> powerUnits();     // base W
[[nodiscard]] std::span<const Unit> energyUnits();    // base J
[[nodiscard]] std::span<const Unit> pressureUnits();  // base Pa
[[nodiscard]] std::span<const Unit> flowUnits();      // base m³/s
[[nodiscard]] std::span<const Unit> areaUnits();      // base m²
[[nodiscard]] std::span<const Unit> lengthUnits();    // base m

/// Convert between two units of the same table
[[nodiscard]] double convert(double value, const Unit& from, const Unit& to);

/// Temperature conversion via Kelvin
[[nodiscard]] constexpr double celsiusToKelvin(double c) noexcept { return c + 273.15; }
[[nodiscard]] constexpr double celsiusToFahrenheit(double c) noexcept { return c * 9.0 / 5.0 + 32.0; }
[[nodiscard]] constexpr double fahrenheitToCelsius(double f) noexcept { return (f - 32.0) * 5.0 / 9.0; }

/// AWG: d = 0.127 mm · 92^((36 − n)/39); n = 0 for 1/0, −1 for 2/0, …
[[nodiscard]] double awgDiameter(double n);
[[nodiscard]] double awgArea(double n);
/// AWG number (continuous) for a given cross-section in mm²
[[nodiscard]] double awgFromArea(double mm2);

}  // namespace elc::units
