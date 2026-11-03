#pragma once
// Outer diameters and weights of common cables (manufacturer data sheets, see source()) for cable tray planning.
// Values are approximate data-sheet values – other manufacturers differ slightly.

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "elc/text.hpp"

namespace elc::cabledata {

enum class Family { NYM, NYY, JYSTY, LiYCY, Cat7 };

struct CableSpec {
    Family family;
    int cores;      // cores, for J-Y(St)Y: pairs
    double size;    // mm², for J-Y(St)Y: conductor diameter in mm, for Cat 7: 0
    double d;       // outer diameter in mm
    double kgPerKm; // weight
    [[nodiscard]] std::string name() const;  // e.g. "NYM-J 5x2,5", "J-Y(St)Y 2x2x0,8"
    [[nodiscard]] bool power() const { return family == Family::NYM || family == Family::NYY; }
};

[[nodiscard]] const std::vector<CableSpec>& all();

/// Finds a cable from free text such as "NYM-J 5x2,5", "nym 5G2.5", "NYY-J 4×95", "J-Y(St)Y 2x2x0,8", "LiYCY 4x0,75", "Cat 7"
[[nodiscard]] std::optional<CableSpec> lookup(std::string_view text);

/// Data sheet the values come from
[[nodiscard]] Text source(Family f);

}  // namespace elc::cabledata
