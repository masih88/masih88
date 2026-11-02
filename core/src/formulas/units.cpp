#include "elc/formulas/units.hpp"

#include <array>
#include <cmath>

namespace elc::units {

namespace {

const std::array<Unit, 8> kPower{{
    {"W", "W", {"Watt", "watt"}, 1.0},
    {"kW", "kW", {"Kilowatt", "kilowatt"}, 1e3},
    {"MW", "MW", {"Megawatt", "megawatt"}, 1e6},
    {"PS", "PS", {"Pferdestärke (metrisch)", "metric horsepower"}, 735.49875},
    {"hp", "hp", {"Horsepower (mechanisch)", "mechanical horsepower"}, 745.69987158227022},
    {"BTUh", "BTU/h", {"BTU pro Stunde", "BTU per hour"}, 0.29307107017222},
    {"kcalh", "kcal/h", {"Kilokalorie pro Stunde", "kilocalorie per hour"}, 1.163},
    {"TR", "TR", {"Kältetonne (Tonne of Refrigeration)", "ton of refrigeration"}, 3516.8528420667},
}};

const std::array<Unit, 8> kEnergy{{
    {"J", "J", {"Joule", "joule"}, 1.0},
    {"kJ", "kJ", {"Kilojoule", "kilojoule"}, 1e3},
    {"MJ", "MJ", {"Megajoule", "megajoule"}, 1e6},
    {"Wh", "Wh", {"Wattstunde", "watt-hour"}, 3600.0},
    {"kWh", "kWh", {"Kilowattstunde", "kilowatt-hour"}, 3.6e6},
    {"MWh", "MWh", {"Megawattstunde", "megawatt-hour"}, 3.6e9},
    {"kcal", "kcal", {"Kilokalorie", "kilocalorie"}, 4186.8},
    {"BTU", "BTU", {"British Thermal Unit", "British thermal unit"}, 1055.05585262},
}};

const std::array<Unit, 8> kPressure{{
    {"Pa", "Pa", {"Pascal", "pascal"}, 1.0},
    {"hPa", "hPa", {"Hektopascal", "hectopascal"}, 100.0},
    {"kPa", "kPa", {"Kilopascal", "kilopascal"}, 1e3},
    {"mbar", "mbar", {"Millibar", "millibar"}, 100.0},
    {"bar", "bar", {"Bar", "bar"}, 1e5},
    {"psi", "psi", {"Pound per square inch", "pound per square inch"}, 6894.757293168},
    {"mWS", "mWS", {"Meter Wassersäule", "metre of water column"}, 9806.65},
    {"mmHg", "mmHg", {"Millimeter Quecksilbersäule", "millimetre of mercury"}, 133.322387415},
}};

const std::array<Unit, 7> kFlow{{
    {"m3h", "m³/h", {"Kubikmeter pro Stunde", "cubic metres per hour"}, 1.0 / 3600.0},
    {"m3s", "m³/s", {"Kubikmeter pro Sekunde", "cubic metres per second"}, 1.0},
    {"ls", "l/s", {"Liter pro Sekunde", "litres per second"}, 1e-3},
    {"lmin", "l/min", {"Liter pro Minute", "litres per minute"}, 1e-3 / 60.0},
    {"lh", "l/h", {"Liter pro Stunde", "litres per hour"}, 1e-3 / 3600.0},
    {"cfm", "CFM", {"Cubic feet per minute", "cubic feet per minute"}, 0.028316846592 / 60.0},
    {"gpm", "gpm (US)", {"US-Gallonen pro Minute", "US gallons per minute"}, 0.003785411784 / 60.0},
}};

const std::array<Unit, 7> kArea{{
    {"mm2", "mm²", {"Quadratmillimeter", "square millimetre"}, 1e-6},
    {"cm2", "cm²", {"Quadratzentimeter", "square centimetre"}, 1e-4},
    {"m2", "m²", {"Quadratmeter", "square metre"}, 1.0},
    {"in2", "in²", {"Quadratzoll", "square inch"}, 6.4516e-4},
    {"ft2", "ft²", {"Quadratfuß", "square foot"}, 0.09290304},
    {"kcmil", "kcmil", {"Kilo-Circular-Mil (MCM)", "kilo circular mil (MCM)"}, 5.067074790975e-7},
    {"cmil", "cmil", {"Circular Mil", "circular mil"}, 5.067074790975e-10},
}};

const std::array<Unit, 11> kLength{{
    {"um", "µm", {"Mikrometer", "micrometre"}, 1e-6},
    {"mm", "mm", {"Millimeter", "millimetre"}, 1e-3},
    {"cm", "cm", {"Zentimeter", "centimetre"}, 1e-2},
    {"m", "m", {"Meter", "metre"}, 1.0},
    {"km", "km", {"Kilometer", "kilometre"}, 1e3},
    {"mil", "mil", {"Mil (1/1000 Zoll)", "mil (1/1000 inch)"}, 2.54e-5},
    {"in", "in", {"Zoll", "inch"}, 0.0254},
    {"ft", "ft", {"Fuß", "foot"}, 0.3048},
    {"yd", "yd", {"Yard", "yard"}, 0.9144},
    {"mi", "mi", {"Meile", "mile"}, 1609.344},
    {"nmi", "sm", {"Seemeile", "nautical mile"}, 1852.0},
}};

}  // namespace

std::span<const Unit> areaUnits() { return kArea; }
std::span<const Unit> lengthUnits() { return kLength; }
std::span<const Unit> powerUnits() { return kPower; }
std::span<const Unit> energyUnits() { return kEnergy; }
std::span<const Unit> pressureUnits() { return kPressure; }
std::span<const Unit> flowUnits() { return kFlow; }

double convert(double value, const Unit& from, const Unit& to) { return value * from.toBase / to.toBase; }

double awgDiameter(double n) { return 0.127 * std::pow(92.0, (36.0 - n) / 39.0); }

double awgArea(double n) {
    const double d = awgDiameter(n);
    return kPi / 4.0 * d * d;
}

double awgFromArea(double mm2) {
    const double d = std::sqrt(4.0 * mm2 / kPi);
    return 36.0 - 39.0 * std::log(d / 0.127) / std::log(92.0);
}

}  // namespace elc::units
