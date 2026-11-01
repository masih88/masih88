#pragma once
// Room acoustics and sound levels.
// Sabine's formula; target reverberation times DIN 18041 (2004 values quoted in BAuA Fb 1030 "Lärm in
// Bildungsstätten", p. 19–20; group A4 added in DIN 18041:2016).

#include <span>

namespace elc::acoustics {

/// Reverberation time T = 0,163 s/m · V / A  (V in m³, equivalent absorption area A in m²)
[[nodiscard]] double sabine(double volume, double absorption);
/// Absorption area needed for reverberation time T
[[nodiscard]] double absorptionFor(double volume, double T);

enum class Use {
    Music,              // A1: T = 0,45·lg V + 0,07
    Speech,             // A2: T = 0,37·lg V − 0,14
    Teaching,           // A3: T = 0,32·lg V − 0,17
    TeachingInclusive,  // A4: T = 0,26·lg V − 0,14 (DIN 18041:2016)
};

/// Target reverberation time T_soll in s for the occupied room
[[nodiscard]] double din18041Target(Use use, double volume);

/// Energetic sum of sound pressure levels: 10·lg Σ 10^(L/10)
[[nodiscard]] double levelSum(std::span<const double> levels);
/// Point source in the free field: L2 = L1 − 20·lg(r2/r1)
[[nodiscard]] double distanceLevel(double l1, double r1, double r2);

}  // namespace elc::acoustics
