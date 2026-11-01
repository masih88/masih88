#pragma once
// Building services rules taken from the technical books:
// - BAC efficiency factors EN ISO 52120-1 (Siemens "Gebäudeautomation – Anwendung der EN ISO 52120", 09/2022, p. 124–125)
// - escape routes ASR A2.3 (current edition 03/2022, amended 2024; the book contains the 2007 edition)
// - smoke vent of lift shafts (MBO § 39 (3) via ZVEI/VDMA guide 2012; VKF via IG-BSK sheet D_AE_001, 03/2007)
// - KNX TP topology (KNX Association "Grundlagenwissen zum KNX Standard", 2013, p. 8–9)

#include <optional>

namespace elc::bac {

enum class BuildingType { Office, LectureHall, School, Hospital, Hotel, Restaurant, Retail, Other, Residential };
enum class Energy { Thermal, Electric };

/// GA efficiency factor f_BAC for class 'A' … 'D' (class C = 1, reference). NaN for an unknown class.
[[nodiscard]] double factor(BuildingType type, Energy energy, char cls);

}  // namespace elc::bac

namespace elc::escape {

/// Minimum clear widths in m (ASR A2.3:2022 Tab. 1): doors/passages (column B) and main escape route (column C)
struct Width {
    double door = 0;
    double route = 0;
};

/// nullopt above 400 persons (outside the table). Values between 200 and 400 persons are interpolated linearly.
[[nodiscard]] std::optional<Width> minWidth(double persons);

enum class Room {
    Normal,               // no or normal fire hazard
    FireWithExtinguisher, // increased fire hazard with automatic extinguishing system
    FireNoExtinguisher,   // increased fire hazard without automatic extinguishing system
    Explosive,            // hazard from explosive substances
};

/// Maximum main escape route length in m (ASR A2.3:2022 section 5 (2)); walking distance ≤ 1.5 × this value
[[nodiscard]] double maxLength(Room room);

}  // namespace elc::escape

namespace elc::shaft {

/// Germany, MBO § 39 (3): free cross-section ≥ 2.5 % of the shaft floor area, at least 0.10 m²
[[nodiscard]] double openingDE(double shaftArea);
/// Switzerland, VKF (sheet IG-BSK D_AE_001, 03/2007): 5 % of the shaft cross-section, max. 0.16 m² (1600 cm²)
[[nodiscard]] double openingCH(double shaftArea);

}  // namespace elc::shaft

namespace elc::knx {

inline constexpr int kDevicesPerSegment = 64;
inline constexpr int kSegmentsPerLine = 4;  // 3 line repeaters → max. 255 devices
inline constexpr int kMaxPerLine = 255;
inline constexpr int kLinesPerArea = 15;
inline constexpr int kAreas = 15;
inline constexpr double kPsuToDevice = 350;    // m
inline constexpr double kDeviceToDevice = 700; // m
inline constexpr double kSegmentLength = 1000; // m (total cable length of a segment)

struct Topology {
    int lines = 0;
    int areas = 0;
    int segmentsPerLine = 0;
    int devicesPerLine = 0;  // planned devices on the fullest line
};

/// Lines/areas for n devices with at most perLine devices per line (≤ 64 without, ≤ 255 with line repeaters)
[[nodiscard]] Topology plan(int devices, int perLine);

}  // namespace elc::knx
