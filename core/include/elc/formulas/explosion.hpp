#pragma once
// Explosion protection (IEC 60079-0 / -14, ATEX 2014/34/EU, 1999/92/EC).

#include <string_view>

namespace elc::ex {

/// Temperature class 1…6 a gas or vapour belongs to (T1: ignition temperature > 450 °C … T6: > 85 °C);
/// 0 if the ignition temperature is ≤ 85 °C (no class sufficient)
[[nodiscard]] int temperatureClass(double ignitionTempC);
/// Maximum surface temperature of a temperature class [°C]: T1 450, T2 300, T3 200, T4 135, T5 100, T6 85
[[nodiscard]] double classLimit(int cls);
/// Dust (IEC 60079-14): T_max = min(2/3 · T_cloud, T_5mm − 75 K) for layers up to 5 mm
[[nodiscard]] double dustMaxSurface(double cloudIgnitionC, double layerGlow5mmC);
/// Equipment group covers the required group (IIC ⊃ IIB ⊃ IIA, IIIC ⊃ IIIB ⊃ IIIA)
[[nodiscard]] bool groupCovers(std::string_view device, std::string_view required);

}  // namespace elc::ex
