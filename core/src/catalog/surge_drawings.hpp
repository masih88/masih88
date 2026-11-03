#pragma once
// Pictures of the area "Überspannungs- & Blitzschutz" (v1.11): lightning protection zones, protection chain, SPD
// connection schemes, overvoltage categories, data centre and Ex i loop after DEHN WPX035/WPX050, and the drawings of
// the surge calculators. All schematic – labels in the ink colour, zones as coloured outlines (light and dark mode).

#include <string>

#include "elc/chart.hpp"
#include "elc/text.hpp"

namespace elc::catalog::surgepics {

Chart zones();         // LPZ 0A/0B/1/2/3 with T1/T2/T3 and D1/C2 at the boundaries
Chart coordination();  // feed → HV (type 1+2) → UV (type 2) → device (type 3)
Chart schemes();       // TN-S 4+0 and TT 3+1 side by side
/// One connection scheme for the earthing system `net` ("tns", "tnc", "tt", "it"); `type` e.g. "Typ 1+2"
Chart scheme(const std::string& net, bool three, const Text& type);
Chart categories();  // overvoltage categories IV … I with U_W
Chart datacenter();  // data centre with the numbered protection points 1–7 (WPX050 Bild 11)
/// Ex i loop: control room (LPZ 2) – SPD – field cable – SPD – transmitter on the tank (WPX035 Bild 4/5)
Chart exCircuit(double len, bool lineOk, const Text& zone);
/// I&C line with arresters: outside = field device outdoors (LPZ 0A → 1), otherwise inside the building
Chart signalRoute(bool outside, const Text& arrester);
/// Installation loop next to a down conductor: U = M · Δi/Δt (WPX050 Bild 15)
Chart loop(double M, double didt, double U);
/// SPD – cable – equipment with U_p,f, U_W, l and l_po (lpo < 0: not limited)
Chart distance(double upf, double uw, double l, double lpo);

}  // namespace elc::catalog::surgepics
