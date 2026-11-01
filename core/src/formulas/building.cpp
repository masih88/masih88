#include "elc/formulas/building.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "elc/constants.hpp"

namespace elc::bac {

namespace {

// [type][D, C, B, A]; EN ISO 52120-1 via Siemens BT_0127 p. 124 (thermal) and p. 125 (electric)
constexpr std::array<std::array<double, 4>, 9> kThermal{{
    {1.51, 1.0, 0.80, 0.70},  // offices
    {1.24, 1.0, 0.75, 0.50},  // lecture halls (A depends strongly on ventilation demand)
    {1.20, 1.0, 0.88, 0.80},  // schools
    {1.31, 1.0, 0.91, 0.86},  // hospitals
    {1.31, 1.0, 0.85, 0.68},  // hotels
    {1.23, 1.0, 0.77, 0.68},  // restaurants
    {1.56, 1.0, 0.73, 0.60},  // wholesale/retail (A depends strongly on ventilation demand)
    {1.0, 1.0, 1.0, 1.0},     // sports, storage, industry, …
    {1.10, 1.0, 0.88, 0.81},  // residential
}};
constexpr std::array<std::array<double, 4>, 9> kElectric{{
    {1.10, 1.0, 0.93, 0.87},
    {1.06, 1.0, 0.94, 0.89},
    {1.07, 1.0, 0.93, 0.86},
    {1.05, 1.0, 0.98, 0.96},
    {1.07, 1.0, 0.95, 0.90},
    {1.04, 1.0, 0.96, 0.92},
    {1.08, 1.0, 0.95, 0.91},
    {1.0, 1.0, 1.0, 1.0},
    {1.08, 1.0, 0.93, 0.92},
}};

}  // namespace

double factor(BuildingType type, Energy energy, char cls) {
    int col = -1;
    switch (cls) {
    case 'D': col = 0; break;
    case 'C': col = 1; break;
    case 'B': col = 2; break;
    case 'A': col = 3; break;
    default: return kNaN;
    }
    const auto row = static_cast<std::size_t>(type);
    return (energy == Energy::Thermal ? kThermal : kElectric)[row][static_cast<std::size_t>(col)];
}

}  // namespace elc::bac

namespace elc::escape {

namespace {
struct Row {
    double persons;
    double door;
    double route;
};
// ASR A2.3 (03/2022) Tab. 1, rows 1–7
constexpr std::array<Row, 7> kRows{{
    {5, 0.80, 0.90},
    {20, 0.90, 1.00},
    {50, 0.90, 1.20},
    {100, 1.00, 1.20},
    {200, 1.05, 1.20},
    {300, 1.65, 1.80},
    {400, 2.25, 2.40},
}};
}  // namespace

std::optional<Width> minWidth(double persons) {
    if (!(persons > 0) || persons > 400) return std::nullopt;
    for (std::size_t i = 0; i < kRows.size(); ++i) {
        if (persons > kRows[i].persons) continue;
        // above 200 persons intermediate values may be interpolated linearly
        if (i > 0 && kRows[i - 1].persons >= 200) {
            const auto& a = kRows[i - 1];
            const auto& b = kRows[i];
            const double f = (persons - a.persons) / (b.persons - a.persons);
            return Width{a.door + f * (b.door - a.door), a.route + f * (b.route - a.route)};
        }
        return Width{kRows[i].door, kRows[i].route};
    }
    return std::nullopt;
}

double maxLength(Room room) {
    switch (room) {
    case Room::Normal:
    case Room::FireWithExtinguisher: return 35;
    case Room::FireNoExtinguisher: return 25;
    case Room::Explosive: return 10;
    }
    return 35;
}

}  // namespace elc::escape

namespace elc::shaft {

double openingDE(double shaftArea) { return std::max(0.025 * shaftArea, 0.10); }
double openingCH(double shaftArea) { return std::min(0.05 * shaftArea, 0.16); }

}  // namespace elc::shaft

namespace elc::knx {

Topology plan(int devices, int perLine) {
    Topology t;
    perLine = std::clamp(perLine, 1, kMaxPerLine);
    if (devices <= 0) return t;
    t.lines = (devices + perLine - 1) / perLine;
    t.devicesPerLine = (devices + t.lines - 1) / t.lines;
    t.segmentsPerLine = (t.devicesPerLine + kDevicesPerSegment - 1) / kDevicesPerSegment;
    t.areas = (t.lines + kLinesPerArea - 1) / kLinesPerArea;
    return t;
}

}  // namespace elc::knx
