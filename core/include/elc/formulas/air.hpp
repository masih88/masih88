#pragma once
// Moist air (psychrometrics) and outdoor air demand.

#include "elc/formulas/common.hpp"

namespace elc::air {

/// Saturation vapour pressure over water, Magnus formula (WMO, −45…60 °C) [hPa]
[[nodiscard]] double saturationPressure(double tC);
/// Dew point from temperature and relative humidity [°C]
[[nodiscard]] double dewPoint(double tC, double rhPercent);
/// Relative humidity from temperature and dew point [%]
[[nodiscard]] double relativeHumidity(double tC, double dewPointC);
/// Humidity ratio x = 0.622 · p_v / (p − p_v) [kg/kg]
[[nodiscard]] double humidityRatio(double vapourPressure_hPa, double pressure_hPa);
/// Specific enthalpy h = 1.006·t + x·(2501 + 1.86·t) [kJ/kg dry air]
[[nodiscard]] double enthalpy(double tC, double x);
/// Density of moist air [kg/m³]
[[nodiscard]] double density(double tC, double vapourPressure_hPa, double pressure_hPa);

/// Steady-state CO₂ balance: required airflow Q = n·G / (C_target − C_outdoor) [m³/h]
/// G in l/h per person, concentrations in ppm
[[nodiscard]] double co2Airflow(double persons, double generation_lph, double targetPpm, double outdoorPpm);
/// Resulting steady-state CO₂ concentration for a given airflow [ppm]
[[nodiscard]] double co2Concentration(double persons, double generation_lph, double airflow_m3h, double outdoorPpm);

}  // namespace elc::air
