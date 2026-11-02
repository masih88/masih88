#pragma once
// Thermocouples types B, E, J, K, N, R, S, T: reference functions of NIST ITS-90 (= IEC 60584-1 / DIN EN 60584-1).
// EMF in mV for a reference junction at 0 °C.

#include <optional>
#include <string_view>
#include <utility>

namespace elc::thermocouple {

enum class Type { B, E, J, K, N, R, S, T };

/// Thermoelectric voltage E(θ) in mV (reference junction 0 °C); nullopt outside the defined range
[[nodiscard]] std::optional<double> emf(Type t, double celsius);
/// Inverse: temperature in °C for an EMF in mV (reference junction 0 °C); nullopt outside the range
[[nodiscard]] std::optional<double> temperature(Type t, double mV);
/// Defined temperature range in °C
[[nodiscard]] std::pair<double, double> range(Type t);
/// Type from "B", "E", … (default K)
[[nodiscard]] Type parse(std::string_view s);

}  // namespace elc::thermocouple
