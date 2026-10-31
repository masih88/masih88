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

// ---- h,x-diagram (Mollier) -------------------------------------------------------------
// x always in kg water per kg dry air, t in °C, p in hPa, h in kJ/kg dry air.

/// Standard atmosphere: p = 1013.25 · (1 − 0.0065 · H / 288.15)^5.255 [hPa]
[[nodiscard]] double pressureAtAltitude(double altitude_m);
/// Humidity ratio at saturation [kg/kg]
[[nodiscard]] double saturationRatio(double tC, double pressure_hPa);
/// Humidity ratio from temperature and relative humidity [kg/kg]
[[nodiscard]] double ratioFromRh(double tC, double rhPercent, double pressure_hPa);
/// Relative humidity of a state (t, x) [%] (> 100 % means fog / supersaturated)
[[nodiscard]] double rhFromRatio(double tC, double x, double pressure_hPa);
/// Dew point of a humidity ratio [°C]
[[nodiscard]] double dewPointFromRatio(double x, double pressure_hPa);
/// Temperature from enthalpy and humidity ratio: t = (h − 2501·x) / (1.006 + 1.86·x)
[[nodiscard]] double temperatureFromHX(double h, double x);
/// Wet-bulb (adiabatic saturation) temperature [°C]
[[nodiscard]] double wetBulb(double tC, double x, double pressure_hPa);

struct AirState {
    double t = 0;  // °C
    double x = 0;  // kg/kg
};

/// Heating or cooling to t2; below the dew point the air leaves saturated (ideal dehumidification)
[[nodiscard]] AirState coolOrHeat(AirState s, double t2, double pressure_hPa);
/// Steam humidification at constant temperature up to rh2
[[nodiscard]] AirState steamHumidify(AirState s, double rh2, double pressure_hPa);
/// Adiabatic (evaporative) humidification along h = const up to rh2
[[nodiscard]] AirState adiabaticHumidify(AirState s, double rh2, double pressure_hPa);
/// Mixing of two air flows, fractionB = mass share of b (0…1)
[[nodiscard]] AirState mix(AirState a, AirState b, double fractionB);

/// Mollier diagram ordinate: isotherms are straight lines y = t · (1.006 + 1.86·x) / 1.006
/// (oblique h,x-coordinates, 0 °C isotherm horizontal; at x = 0, y equals t)
[[nodiscard]] double mollierY(double tC, double x);
/// Inverse of mollierY
[[nodiscard]] double mollierT(double y, double x);

/// Steady-state CO₂ balance: required airflow Q = n·G / (C_target − C_outdoor) [m³/h]
/// G in l/h per person, concentrations in ppm
[[nodiscard]] double co2Airflow(double persons, double generation_lph, double targetPpm, double outdoorPpm);
/// Resulting steady-state CO₂ concentration for a given airflow [ppm]
[[nodiscard]] double co2Concentration(double persons, double generation_lph, double airflow_m3h, double outdoorPpm);

}  // namespace elc::air
