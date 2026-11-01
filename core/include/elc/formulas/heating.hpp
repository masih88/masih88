#pragma once
// Heating, pumps/fans and heat pumps.
// Sources: Siemens "Regeln und Steuern von Heizungsanlagen" (03/2023, ch. 3.1: steepness, parallel shift),
// Siemens "Einführung in die HLK- und Gebäudetechnik" (08/2022, p. 180: proportionality laws),
// Danfoss "Wissenswertes über Frequenzumrichter" (P ~ n³), Siemens "Kältetechnik" (08/2022, ch. 5.4/7.6),
// QM Holzheizwerke FAQ 37 (minimum controllable output of a control valve).

namespace elc::heating {

/// Steepness of the heating curve s = (T_V(−5 °C) − T_V(+15 °C)) / 20 K
[[nodiscard]] constexpr double steepness(double tvAtMinus5, double tvAtPlus15) noexcept {
    return (tvAtMinus5 - tvAtPlus15) / 20.0;
}
/// Change of the flow temperature for a room set-point change: ΔT_V = ΔT_R · (1 + s)
[[nodiscard]] constexpr double flowShift(double dRoom, double s) noexcept { return dRoom * (1.0 + s); }
/// Straight line through the two support points (−5 °C | T_V(−5)) and (+15 °C | T_V(+15))
[[nodiscard]] constexpr double linearFlow(double tvAtMinus5, double tvAtPlus15, double ta) noexcept {
    return tvAtPlus15 + (15.0 - ta) * steepness(tvAtMinus5, tvAtPlus15);
}

/// Design point of a radiator system
struct Design {
    double ti = 20;  // room temperature at design [°C]
    double ta = -10; // design outdoor temperature [°C]
    double tv = 70;  // flow temperature at design [°C]
    double tr = 50;  // return temperature at design [°C]
    double n = 1.3;  // radiator exponent (EN 442: radiators ≈ 1.3, floor heating ≈ 1.1)
};

/// Flow temperature for outdoor temperature ta and room set point tiSet (constant flow rate,
/// arithmetic mean excess temperature): T_V = T_i + (T_m,a − T_i,a)·q^(1/n) + (T_V,a − T_R,a)·q/2,
/// q = (T_i − T_a)/(T_i,a − T_a,a) ≥ 0
[[nodiscard]] double flowTemperature(const Design& d, double ta, double tiSet);

}  // namespace elc::heating

namespace elc::fans {

/// Operating point of a pump or fan on a purely quadratic system curve
struct Point {
    double flow = 0;
    double pressure = 0;
    double power = 0;
};

/// Proportionality (affinity) laws for the speed ratio n2/n1: V ~ n, Δp ~ n², P ~ n³
[[nodiscard]] constexpr Point affinity(const Point& p1, double ratio) noexcept {
    return {p1.flow * ratio, p1.pressure * ratio * ratio, p1.power * ratio * ratio * ratio};
}

}  // namespace elc::fans

namespace elc::heatpump {

/// Ideal (Carnot) heating COP = T_sink / (T_sink − T_source), temperatures in °C
[[nodiscard]] double carnotHeating(double tSinkC, double tSourceC);
/// Ideal (Carnot) cooling EER = T_evap / (T_cond − T_evap), temperatures in °C
[[nodiscard]] double carnotCooling(double tEvapC, double tCondC);

}  // namespace elc::heatpump

namespace elc::valves {

/// Smallest controllable flow ratio V̇_min/V̇_100 of a valve with authority a_v and rangeability S_v = kvs/kvr:
/// 1 / √(1 + a_v·(S_v² − 1))
[[nodiscard]] double minFlowRatio(double authority, double rangeability);
/// Heat output ratio of a heat exchanger for a flow ratio (characteristic value a_WT, 1 = linear):
/// Q̇/Q̇_100 = 1 / (1 + a_WT·(V̇_100/V̇ − 1))
[[nodiscard]] double heatRatio(double flowRatio, double aHx);

}  // namespace elc::valves
