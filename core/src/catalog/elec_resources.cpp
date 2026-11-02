// Area "Elektrotechnik" (v1.8), chapter "Ressourcen": reference tables (fuses, resistor series, cable data,
// enclosures, plugs and sockets of the world, colour codes, device numbers, units, pipes) and drawings
// (time-current curves, earthing systems, electrical symbols).
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <numbers>
#include <numeric>
#include <span>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"
#include "elc/formulas/electrical.hpp"
#include "elc/formulas/electronics.hpp"
#include "elc/formulas/thermocouple.hpp"

namespace elc::catalog {

namespace {

using Row = std::vector<Text>;
using P = Sketch::P;
constexpr const char* kCat = "electrical";

Table make(std::string id, Text title, Text summary, Text description, Row columns, std::vector<Row> rows) {
    Table t;
    t.id = std::move(id);
    t.category = kCat;
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

Text num(double v, int d) { return formatNumber(v, d); }

// ------------------------------------------------------------------ fuses and protection

Table ulFuseClasses() {
    return make("ul_fuse_classes", {"UL/CSA-Sicherungsklassen", "UL/CSA fuse classes"},
                {"Nordamerikanische Niederspannungssicherungen nach UL 248", "North American low-voltage fuses per UL 248"},
                {"UL 248 / CSA C22.2 No. 248. Klassen mit Unverwechselbarkeit (rejection: CC, J, L, R, T) nehmen keine Sicherung "
                 "mit geringerem Schaltvermögen auf. Werte als Übersicht – Herstellerdaten und gültige Normausgabe prüfen.",
                 "UL 248 / CSA C22.2 No. 248. Rejection classes (CC, J, L, R, T) do not accept fuses with lower interrupting rating. "
                 "Overview values – check manufacturer data and the edition in force."},
                {{"Klasse", "Class"}, {"Spannung", "Voltage"}, {"Bemessungsstrom", "Ampere rating"}, {"Schaltvermögen", "Interrupting rating"},
                 {"Merkmal / Einsatz", "Feature / use"}},
                {{"CC", "600 V AC", "0–30 A", "200 kA", {"klein (13/32\" × 1½\"), Steuerstromkreise, Motoren", "small (13/32\" × 1½\"), control circuits, motors"}},
                 {"CD", "600 V AC", "35–60 A", "200 kA", {"wie CC, größere Ströme", "like CC, higher currents"}},
                 {"G", "480–600 V AC", "0–60 A", "100 kA", {"Längen je Strombereich verschieden", "length differs per current range"}},
                 {"H", "250/600 V AC", "0–600 A", "10 kA", {"ältere Bauart, geringes Schaltvermögen", "older design, low interrupting rating"}},
                 {"J", "600 V AC", "0–600 A", "200 kA", {"strombegrenzend, kompakt", "current-limiting, compact"}},
                 {"K", "250/600 V AC", "0–600 A", "50–200 kA", {"Maße wie H, ohne Unverwechselbarkeit", "dimensions as H, no rejection"}},
                 {"L", "600 V AC", "601–6000 A", "200 kA", {"Schraubbefestigung, Einspeisungen", "bolt-in, main feeders"}},
                 {"R (RK1, RK5)", "250/600 V AC", "0–600 A", "200 kA", {"Unverwechselbarkeit durch Nut, Motorabgänge", "rejection groove, motor circuits"}},
                 {"T", "300/600 V AC", "0–1200 A", "200 kA", {"sehr kompakt, sehr flink", "very compact, very fast"}}});
}

Table tripCharacteristics() {
    return make("trip_characteristics", {"Auslösekennlinien: Prüfströme", "Tripping curves: test currents"},
                {"LS-Schalter, Sicherungen gG und RCD", "MCBs, gG fuses and RCDs"},
                {"LS-Schalter IEC 60898-1 (B, C, D) bzw. IEC 60947-2 (K, Z); gG-Sicherungen IEC 60269-2; RCD IEC 61008/61009 "
                 "(Abschaltzeiten bei Bemessungsdifferenzstrom, unverzögert/selektiv S). Verlauf der Kennlinien: Rechner "
                 "„Auslösekennlinien“. Gegen die gültige Ausgabe prüfen.",
                 "MCBs IEC 60898-1 (B, C, D) or IEC 60947-2 (K, Z); gG fuses IEC 60269-2; RCDs IEC 61008/61009 (break times at "
                 "rated residual current, general/selective S). Curves: calculator “Tripping curves”. Check against the edition in force."},
                {{"Gerät", "Device"}, {"Nichtauslösen", "No tripping"}, {"Auslösen", "Tripping"}, {"Hinweis", "Note"}},
                {{{"LS B, C, D (thermisch)", "MCB B, C, D (thermal)"}, {"1,13 · I_n ≥ 1 h", "1.13 · I_n ≥ 1 h"}, {"1,45 · I_n < 1 h", "1.45 · I_n < 1 h"},
                  {"2,55 · I_n: 1–60 s (I_n ≤ 32 A), 1–120 s (> 32 A)", "2.55 · I_n: 1–60 s (I_n ≤ 32 A), 1–120 s (> 32 A)"}},
                 {{"LS B (magnetisch)", "MCB B (magnetic)"}, {"3 · I_n ≥ 0,1 s", "3 · I_n ≥ 0.1 s"}, {"5 · I_n < 0,1 s", "5 · I_n < 0.1 s"}, {"Leitungsschutz", "cable protection"}},
                 {{"LS C (magnetisch)", "MCB C (magnetic)"}, {"5 · I_n ≥ 0,1 s", "5 · I_n ≥ 0.1 s"}, {"10 · I_n < 0,1 s", "10 · I_n < 0.1 s"}, {"Motoren, Leuchten", "motors, luminaires"}},
                 {{"LS D (magnetisch)", "MCB D (magnetic)"}, {"10 · I_n ≥ 0,1 s", "10 · I_n ≥ 0.1 s"}, {"20 · I_n < 0,1 s", "20 · I_n < 0.1 s"}, {"hohe Einschaltströme", "high inrush"}},
                 {{"LS K", "MCB K"}, {"1,05 · I_n; 8 · I_n", "1.05 · I_n; 8 · I_n"}, {"1,2 · I_n; 14 · I_n", "1.2 · I_n; 14 · I_n"}, {"IEC 60947-2, Herstellerangaben", "IEC 60947-2, manufacturer data"}},
                 {{"LS Z", "MCB Z"}, {"1,05 · I_n; 2 · I_n", "1.05 · I_n; 2 · I_n"}, {"1,2 · I_n; 3 · I_n", "1.2 · I_n; 3 · I_n"}, {"Halbleiter, Messkreise", "semiconductors, measuring circuits"}},
                 {{"Sicherung gG (I_n ≥ 16 A)", "Fuse gG (I_n ≥ 16 A)"}, {"1,25 · I_n (I_nf)", "1.25 · I_n (I_nf)"}, {"1,6 · I_n (I_f)", "1.6 · I_n (I_f)"},
                  {"Prüfdauer 1–4 h je nach I_n", "test time 1–4 h depending on I_n"}},
                 {{"RCD unverzögert", "RCD general type"}, {"0,5 · I_Δn", "0.5 · I_Δn"}, {"I_Δn ≤ 300 ms, 2 · I_Δn ≤ 150 ms, 5 · I_Δn ≤ 40 ms", "I_Δn ≤ 300 ms, 2 · I_Δn ≤ 150 ms, 5 · I_Δn ≤ 40 ms"},
                  {"Typ AC, A, F, B je nach Fehlerstromform", "type AC, A, F, B by residual current waveform"}},
                 {{"RCD selektiv (S)", "RCD selective (S)"}, {"I_Δn: ≥ 130 ms", "I_Δn: ≥ 130 ms"}, {"I_Δn ≤ 500 ms, 5 · I_Δn ≤ 150 ms", "I_Δn ≤ 500 ms, 5 · I_Δn ≤ 150 ms"},
                  {"vorgeschaltet für Selektivität", "upstream for selectivity"}}});
}

// ------------------------------------------------------------------ resistor series

Table eSeriesTable() {
    std::vector<Row> rows;
    const auto e24 = electronics::eSeriesBase(electronics::ESeries::E24);
    const auto e12 = electronics::eSeriesBase(electronics::ESeries::E12);
    const auto e6 = electronics::eSeriesBase(electronics::ESeries::E6);
    auto contains = [](std::span<const double> set, double v) {
        return std::any_of(set.begin(), set.end(), [&](double x) { return std::fabs(x - v) < 1e-9; });
    };
    const Text yes{"✓", "✓"};
    const Text no{"–", "–"};
    for (const double v : e24) {
        const bool e3 = std::fabs(v - 10) < 1e-9 || std::fabs(v - 22) < 1e-9 || std::fabs(v - 47) < 1e-9;
        rows.push_back({num(v / 10.0, 1), contains(e12, v) ? yes : no, contains(e6, v) ? yes : no, e3 ? yes : no});
    }
    return make("e_series_table", {"Normwerte von Widerständen (E-Reihen)", "Standard resistor values (E series)"},
                {"E24 mit Zugehörigkeit zu E12, E6, E3", "E24 with membership of E12, E6, E3"},
                {"IEC 60063: Werte je Dekade (× 1, 10, 100 … Ω). Toleranz: E3 > ±20 %, E6 ±20 %, E12 ±10 %, E24 ±5 %; "
                 "E48 ±2 %, E96 ±1 %, E192 ≤ ±0,5 % – E96 siehe Tabelle EIA-96-Code. Rechner „E-Reihe“ findet den nächsten Normwert.",
                 "IEC 60063: values per decade (× 1, 10, 100 … Ω). Tolerance: E3 > ±20 %, E6 ±20 %, E12 ±10 %, E24 ±5 %; "
                 "E48 ±2 %, E96 ±1 %, E192 ≤ ±0.5 % – E96 see table EIA-96 code. Calculator “E series” finds the nearest value."},
                {"E24", "E12", "E6", "E3"}, rows);
}

// ------------------------------------------------------------------ cable data

/// PVC insulation thickness for 0.6/1 kV cables (IEC 60502-1, Table 5), mm
double insulationThickness(double a) {
    if (a <= 2.5) return 0.8;
    if (a <= 16) return 1.0;
    if (a <= 35) return 1.2;
    if (a <= 70) return 1.4;
    if (a <= 120) return 1.6;
    if (a <= 150) return 1.8;
    if (a <= 185) return 2.0;
    if (a <= 240) return 2.2;
    return 2.4;
}

/// X' in mΩ/m for conductor centre distance a and equivalent radius r (round conductor, 50 Hz)
double reactance(double a, double r, double f = 50.0) { return 2.0 * std::numbers::pi * f * 2e-7 * (0.25 + std::log(a / r)) * 1e3; }

Table cableReactance() {
    std::vector<Row> rows;
    for (const double A : cables::kAmpacityAreas) {
        const double r = std::sqrt(A / std::numbers::pi);
        const double t = insulationThickness(A);
        const double aMulti = 2.0 * (r + t);           // cores touching inside a multi-core cable
        const double aTrefoil = 2.0 * (r + t + 1.8);   // single-core cables touching (sheath ≈ 1.8 mm)
        rows.push_back({num(A, 1), num(reactance(aMulti, r), 3), num(reactance(aTrefoil, r), 3), num(reactance(2.0 * aTrefoil, r), 3)});
    }
    return make("cable_reactance_table", {"Reaktanzbeläge von Kabeln", "Cable reactance table"},
                {"X' in mΩ/m bei 50 Hz (berechnet)", "X' in mΩ/m at 50 Hz (calculated)"},
                {"Berechnet je Leiter: X' = 2πf · μ₀/(2π) · (1/4 + ln(a/r)), r = √(A/π), a = Achsabstand der Leiter. "
                 "Mehradrig: a = 2 · (r + Isolierdicke nach IEC 60502-1); Einleiter im Dreieck: zusätzlich 1,8 mm Mantel; "
                 "Einleiter mit Abstand = doppelter Kabeldurchmesser. Näherung (Sektorleiter, Verseilung und Bewehrung nicht "
                 "berücksichtigt) – Herstellerwerte bevorzugen. Üblicher Rechenwert für mehradrige Kabel: 0,08 mΩ/m. Bei 60 Hz × 1,2.",
                 "Calculated per conductor: X' = 2πf · μ₀/(2π) · (1/4 + ln(a/r)), r = √(A/π), a = axial spacing. Multi-core: "
                 "a = 2 · (r + insulation thickness per IEC 60502-1); single cores in trefoil: plus 1.8 mm sheath; spaced single cores: "
                 "spacing = twice the cable diameter. Approximation (sector conductors, lay and armour neglected) – prefer manufacturer "
                 "values. Usual design value for multi-core cables: 0.08 mΩ/m. At 60 Hz × 1.2."},
                {{"A in mm²", "A in mm²"}, {"mehradrig", "multi-core"}, {"Einleiter, Dreieck", "single core, trefoil"},
                 {"Einleiter, Abstand", "single core, spaced"}},
                rows);
}

Table resistivityTable() {
    struct M {
        Text name;
        double rho;
        double alpha;
    };
    const std::vector<M> m{
        {{"Silber", "Silver"}, 0.0159, 0.0038},          {{"Kupfer (IACS, weich)", "Copper (IACS, annealed)"}, 0.017241, 0.00393},
        {{"Gold", "Gold"}, 0.0244, 0.0034},              {{"Aluminium (Leiter)", "Aluminium (conductor)"}, 0.028264, 0.00403},
        {{"Wolfram", "Tungsten"}, 0.055, 0.0045},        {{"Zink", "Zinc"}, 0.059, 0.0037},
        {{"Nickel", "Nickel"}, 0.069, 0.006},            {{"Messing (CuZn37)", "Brass (CuZn37)"}, 0.067, 0.0015},
        {{"Eisen (rein)", "Iron (pure)"}, 0.10, 0.0065}, {{"Platin", "Platinum"}, 0.106, 0.00385},
        {{"Zinn", "Tin"}, 0.115, 0.0045},                {{"Stahl (unlegiert)", "Steel (carbon)"}, 0.14, 0.005},
        {{"Blei", "Lead"}, 0.208, 0.0039},               {{"Edelstahl 1.4301", "Stainless steel 1.4301"}, 0.73, 0.001},
        {{"Manganin", "Manganin"}, 0.43, 0.00001},       {{"Konstantan", "Constantan"}, 0.49, 0.00001},
        {{"Quecksilber", "Mercury"}, 0.958, 0.0009},     {{"Nickelchrom (NiCr 80/20)", "Nichrome (NiCr 80/20)"}, 1.10, 0.0001},
    };
    std::vector<Row> rows;
    for (const auto& x : m) rows.push_back({x.name, num(x.rho, 4), num(1.0 / x.rho, 2), num(x.alpha * 1000.0, 2)});
    return make("resistivity_table", {"Spezifischer Widerstand und Leitfähigkeit", "Resistivity and conductivity"},
                {"ρ, κ und α bei 20 °C", "ρ, κ and α at 20 °C"},
                {"Typische Werte bei 20 °C (Tabellenbuch); je nach Reinheit/Legierung abweichend. Kupfer und Aluminium für Leiter "
                 "nach IEC 60028 bzw. IEC 60889. R = ρ · L / A, κ = 1/ρ, R(θ) = R₂₀ · (1 + α · (θ − 20 °C)).",
                 "Typical values at 20 °C (handbook); vary with purity/alloy. Copper and aluminium conductors per IEC 60028 and "
                 "IEC 60889. R = ρ · L / A, κ = 1/ρ, R(θ) = R₂₀ · (1 + α · (θ − 20 °C))."},
                {{"Werkstoff", "Material"}, {"ρ in Ω·mm²/m", "ρ in Ω·mm²/m"}, {"κ in m/(Ω·mm²)", "κ in m/(Ω·mm²)"}, {"α in 10⁻³/K", "α in 10⁻³/K"}}, rows);
}

Table vdropPerLength() {
    std::vector<Row> rows;
    const double rho = cables::resistivity(cables::Material::Cu, 70.0);
    const double x = 0.08e-3;
    for (const double A : cables::kAmpacityAreas) {
        auto mv = [&](System s, double c) { return num(cables::voltageDrop(s, 1.0, 1.0, A, rho, c, x) * 1e3, 2); };
        rows.push_back({num(A, 1), mv(System::AC1, 1.0), mv(System::AC1, 0.8), mv(System::AC3, 1.0), mv(System::AC3, 0.8)});
    }
    return make("vdrop_per_length", {"Spannungsfall je Ampere und Meter", "Voltage drop per ampere and metre"},
                {"ΔU in mV/(A·m), Kupfer, 70 °C", "ΔU in mV/(A·m), copper, 70 °C"},
                {"ΔU = k · (ρ/A · cos φ + X' · sin φ) mit k = 2 (1~, Hin- und Rückleiter) bzw. √3 (3~), ρ bei 70 °C (PVC), "
                 "X' = 0,08 mΩ/m. Spannungsfall = Tabellenwert · I · L / 1000 in V. Rechner „Spannungsfall“ rechnet genauer.",
                 "ΔU = k · (ρ/A · cos φ + X' · sin φ) with k = 2 (1~, go and return) or √3 (3~), ρ at 70 °C (PVC), X' = 0.08 mΩ/m. "
                 "Voltage drop = table value · I · L / 1000 in V. Calculator “Voltage drop” is more exact."},
                {{"A in mm²", "A in mm²"}, {"1~ cos φ = 1", "1~ cos φ = 1"}, {"1~ cos φ = 0,8", "1~ cos φ = 0.8"}, {"3~ cos φ = 1", "3~ cos φ = 1"},
                 {"3~ cos φ = 0,8", "3~ cos φ = 0.8"}},
                rows);
}

Table conductorData() {
    // IEC 60228 max. DC resistance at 20 °C, plain annealed copper, class 1/2 (Ω/km)
    const std::vector<std::pair<double, double>> r{{1.5, 12.1},  {2.5, 7.41},  {4, 4.61},    {6, 3.08},    {10, 1.83},   {16, 1.15},
                                                   {25, 0.727},  {35, 0.524},  {50, 0.387},  {70, 0.268},  {95, 0.193},  {120, 0.153},
                                                   {150, 0.124}, {185, 0.0991}, {240, 0.0754}, {300, 0.0601}};
    std::vector<Row> rows;
    for (const auto& [A, rmax] : r) {
        const double d = std::sqrt(4.0 * A / std::numbers::pi);
        rows.push_back({num(A, 1), num(d, 2), num(rmax, rmax < 0.1 ? 4 : 3), num(A * 8.89, 1), num(A * 2.70, 1), num(insulationThickness(A), 1)});
    }
    return make("conductor_data", {"Abmessungen und Gewicht von Leitern", "Dimensions and weight of conductors"},
                {"Durchmesser, Höchstwiderstand, Metallgewicht", "Diameter, maximum resistance, metal weight"},
                {"d = Durchmesser eines massiven Rundleiters; R₂₀ max. nach IEC 60228 (Kupfer blank, Klasse 1/2); Metallgewicht je "
                 "Leiter mit Dichte Cu 8,89 und Al 2,70 kg/dm³ (Kabelgewicht = Metall · Aderzahl + Isolierung und Mantel, siehe "
                 "Herstellerdatenblatt); Isolierwanddicke PVC 0,6/1 kV nach IEC 60502-1. Außendurchmesser sind herstellerabhängig.",
                 "d = diameter of a solid round conductor; R₂₀ max. per IEC 60228 (plain copper, class 1/2); metal weight per "
                 "conductor with density Cu 8.89 and Al 2.70 kg/dm³ (cable weight = metal · cores + insulation and sheath, see data "
                 "sheet); PVC insulation thickness 0.6/1 kV per IEC 60502-1. Outer diameters depend on the manufacturer."},
                {{"A in mm²", "A in mm²"}, {"d in mm", "d in mm"}, {"R₂₀ max. in Ω/km", "R₂₀ max. in Ω/km"}, {"Cu in kg/km", "Cu in kg/km"},
                 {"Al in kg/km", "Al in kg/km"}, {"Isolierung in mm", "Insulation in mm"}},
                rows);
}

// ------------------------------------------------------------------ enclosures

Table ikCodes() {
    return make("ik_codes", {"IK-Code (Stoßfestigkeit)", "IK code (impact protection)"}, {"Schlagenergie nach EN 62262", "Impact energy per EN 62262"},
                {"IEC 62262 / EN 62262: Schutz von Gehäusen gegen mechanische Stöße. Prüfung mit Fallhammer (Masse, Fallhöhe).",
                 "IEC 62262 / EN 62262: protection of enclosures against mechanical impacts. Test with a falling hammer (mass, height)."},
                {"IK", {"Energie", "Energy"}, {"Masse", "Mass"}, {"Fallhöhe", "Drop height"}},
                {{"IK00", {"kein Schutz", "no protection"}, "–", "–"}, {"IK01", "0,14 J", "0,25 kg", "56 mm"}, {"IK02", "0,2 J", "0,25 kg", "80 mm"},
                 {"IK03", "0,35 J", "0,25 kg", "140 mm"}, {"IK04", "0,5 J", "0,25 kg", "200 mm"}, {"IK05", "0,7 J", "0,25 kg", "280 mm"},
                 {"IK06", "1 J", "0,25 kg", "400 mm"}, {"IK07", "2 J", "0,5 kg", "400 mm"}, {"IK08", "5 J", "1,7 kg", "300 mm"},
                 {"IK09", "10 J", "5 kg", "200 mm"}, {"IK10", "20 J", "5 kg", "400 mm"}});
}

Table nemaEnclosures() {
    return make("nema_enclosures", {"NEMA-Gehäusetypen", "NEMA enclosure types"}, {"NEMA 250 mit ungefährer IP-Entsprechung", "NEMA 250 with approximate IP equivalent"},
                {"NEMA 250 (Nordamerika). Die IP-Angabe ist die Mindestentsprechung nach NEMA-Umrechnungstabelle – NEMA-Typen "
                 "verlangen zusätzliche Prüfungen (Korrosion, Eis, Öl); eine Umrechnung von IP nach NEMA ist nicht zulässig.",
                 "NEMA 250 (North America). The IP code is the minimum equivalent per the NEMA conversion table – NEMA types require "
                 "additional tests (corrosion, ice, oil); converting IP to NEMA is not permitted."},
                {{"Typ", "Type"}, {"Einsatz / Schutz", "Use / protection"}, {"≈ IP", "≈ IP"}},
                {{"1", {"innen, Berührung, fallender Schmutz", "indoor, contact, falling dirt"}, "IP10"},
                 {"2", {"innen, Tropfwasser", "indoor, dripping water"}, "IP11"},
                 {"3", {"außen, Regen, Schnee, Staub, Eis", "outdoor, rain, sleet, windblown dust, ice"}, "IP54"},
                 {"3R", {"außen, Regen, Schnee, Eis (nicht Staub)", "outdoor, rain, sleet, ice (not dust)"}, "IP14"},
                 {"3S", {"wie 3, bedienbar bei Vereisung", "as 3, operable when ice-laden"}, "IP54"},
                 {"4", {"innen/außen, Strahlwasser", "indoor/outdoor, hose-directed water"}, "IP56"},
                 {"4X", {"wie 4 + korrosionsbeständig", "as 4 + corrosion resistant"}, "IP56"},
                 {"5", {"innen, Staub, Tropfwasser", "indoor, dust, dripping water"}, "IP52"},
                 {"6", {"zeitweises Untertauchen", "temporary submersion"}, "IP67"},
                 {"6P", {"längeres Untertauchen", "prolonged submersion"}, "IP67"},
                 {"7, 8, 9, 10", {"explosionsgefährdete Bereiche / Bergbau", "hazardous locations / mining"}, "–"},
                 {"12, 12K", {"innen, Industrie: Staub, Tropföl, Kühlmittel", "indoor industrial: dust, dripping oil, coolant"}, "IP52"},
                 {"13", {"innen, Staub, Spritzöl und Kühlmittel", "indoor, dust, spraying oil and coolant"}, "IP54"}});
}

Table cctvResolutions() {
    struct R {
        const char* name;
        int w, h;
    };
    const std::vector<R> r{{"QCIF", 176, 144},   {"CIF", 352, 288},      {"4CIF", 704, 576},     {"D1 (PAL)", 720, 576},
                           {"960H", 960, 576},   {"720p (HD)", 1280, 720}, {"960p (1,3 MP)", 1280, 960}, {"1080p (Full HD, 2 MP)", 1920, 1080},
                           {"3 MP", 2048, 1536}, {"4 MP", 2560, 1440},   {"5 MP", 2592, 1944},   {"6 MP", 3072, 2048},
                           {"4K UHD (8 MP)", 3840, 2160}, {"12 MP", 4000, 3000}};
    std::vector<Row> rows;
    for (const auto& x : r) {
        const std::string px = std::to_string(x.w) + " × " + std::to_string(x.h);
        const double g = std::gcd(x.w, x.h);
        const std::string ratio = std::to_string(static_cast<int>(x.w / g)) + ":" + std::to_string(static_cast<int>(x.h / g));
        rows.push_back({{x.name, x.name}, {px, px}, num(x.w * static_cast<double>(x.h) / 1e6, 2), {ratio, ratio}});
    }
    return make("cctv_resolutions", {"CCTV-Auflösungen", "CCTV resolutions"}, {"Pixelformate von analogen und IP-Kameras", "Pixel formats of analogue and IP cameras"},
                {"Übliche Auflösungen von Überwachungskameras (analog: CIF/D1/960H, IP: HD bis 4K). Speicherbedarf siehe Rechner "
                 "„CCTV Speicherplatz“. Bezeichnungen „MP“ sind herstellerüblich, die genaue Pixelzahl kann abweichen.",
                 "Common resolutions of surveillance cameras (analogue: CIF/D1/960H, IP: HD to 4K). Storage see calculator “CCTV "
                 "storage”. “MP” names are vendor conventions, exact pixel counts may differ."},
                {{"Bezeichnung", "Name"}, {"Pixel", "Pixels"}, {"Megapixel", "Megapixels"}, {"Format", "Aspect"}}, rows);
}

Table thermocoupleTable() {
    struct T {
        const char* type;
        Text pair;
        const char* iec;
        Text ansi;
    };
    const std::vector<T> t{
        {"K", {"NiCr – NiAl", "NiCr – NiAl"}, "grün / weiß", {"gelb / rot, Mantel gelb", "yellow / red, jacket yellow"}},
        {"J", {"Fe – CuNi", "Fe – CuNi"}, "schwarz / weiß", {"weiß / rot, Mantel schwarz", "white / red, jacket black"}},
        {"T", {"Cu – CuNi", "Cu – CuNi"}, "braun / weiß", {"blau / rot, Mantel blau", "blue / red, jacket blue"}},
        {"E", {"NiCr – CuNi", "NiCr – CuNi"}, "violett / weiß", {"violett / rot, Mantel violett", "purple / red, jacket purple"}},
        {"N", {"NiCrSi – NiSi", "NiCrSi – NiSi"}, "rosa / weiß", {"orange / rot, Mantel orange", "orange / red, jacket orange"}},
        {"R", {"Pt13Rh – Pt", "Pt13Rh – Pt"}, "orange / weiß", {"schwarz / rot, Mantel grün", "black / red, jacket green"}},
        {"S", {"Pt10Rh – Pt", "Pt10Rh – Pt"}, "orange / weiß", {"schwarz / rot, Mantel grün", "black / red, jacket green"}},
        {"B", {"Pt30Rh – Pt6Rh", "Pt30Rh – Pt6Rh"}, "grau / weiß", {"grau / rot, Mantel grau", "grey / red, jacket grey"}},
    };
    std::vector<Row> rows;
    for (const auto& x : t) {
        const auto tt = thermocouple::parse(x.type);
        const auto [lo, hi] = thermocouple::range(tt);
        const double e100 = thermocouple::emf(tt, 100.0).value_or(0.0);
        const std::string range = formatNumber(lo, 0, Lang::De) + " … " + formatNumber(hi, 0, Lang::De) + " °C";
        const std::string rangeEn = formatNumber(lo, 0, Lang::En) + " … " + formatNumber(hi, 0, Lang::En) + " °C";
        const std::string iecEn = std::string(x.iec) == "grün / weiß" ? "green / white" : std::string(x.iec) == "schwarz / weiß" ? "black / white"
                                  : std::string(x.iec) == "braun / weiß" ? "brown / white" : std::string(x.iec) == "violett / weiß" ? "violet / white"
                                  : std::string(x.iec) == "rosa / weiß" ? "pink / white" : std::string(x.iec) == "orange / weiß" ? "orange / white"
                                                                                            : "grey / white";
        rows.push_back({{x.type, x.type}, x.pair, {range, rangeEn}, num(e100, 3), {x.iec, iecEn}, x.ansi});
    }
    return make("thermocouple_table", {"Thermoelemente: Farben und Daten", "Thermocouples: colours and data"},
                {"Typen K, J, T, E, N, R, S, B", "Types K, J, T, E, N, R, S, B"},
                {"Paarung (Plus – Minus), Definitionsbereich der Grundwerte (IEC 60584-1 / NIST ITS-90), Thermospannung bei 100 °C "
                 "(Vergleichsstelle 0 °C), Aderfarben Plus/Minus der Ausgleichsleitung nach IEC 60584-3 (Mantel wie Plusader) und "
                 "ANSI/ASTM MC96.1 (Minus immer rot). Einsatzbereich je nach Drahtstärke kleiner. Rechner „Thermoelemente“.",
                 "Pair (plus – minus), defined range of the reference values (IEC 60584-1 / NIST ITS-90), EMF at 100 °C (reference "
                 "junction 0 °C), colours plus/minus of extension cable per IEC 60584-3 (sheath as plus) and ANSI/ASTM MC96.1 (minus "
                 "always red). Usable range smaller depending on wire size. Calculator “Thermocouples”."},
                {{"Typ", "Type"}, {"Paarung", "Pair"}, {"Bereich", "Range"}, {"E(100 °C) in mV", "E(100 °C) in mV"}, {"IEC 60584-3", "IEC 60584-3"},
                 {"ANSI MC96.1", "ANSI MC96.1"}},
                rows);
}

// ------------------------------------------------------------------ designations

Table ansiDeviceNumbers() {
    const std::vector<std::array<const char*, 3>> d{
        {"2", "Zeitrelais (Anlauf/Verzögerung)", "time-delay starting or closing relay"},
        {"21", "Distanzschutz", "distance relay"},
        {"25", "Synchronisierungs-/Synchroncheck-Relais", "synchronising or synchronism-check device"},
        {"27", "Unterspannungsschutz", "undervoltage relay"},
        {"32", "Rückleistungs-/Richtungsleistungsschutz", "directional power relay"},
        {"37", "Unterstrom-/Unterleistungsschutz", "undercurrent or underpower relay"},
        {"40", "Erregungsverlustschutz", "field (loss of excitation) relay"},
        {"46", "Schieflastschutz (Gegensystem)", "negative-sequence / phase-balance current relay"},
        {"47", "Phasenfolge-/Spannungsasymmetrieschutz", "phase-sequence voltage relay"},
        {"49", "thermischer Überlastschutz", "thermal relay"},
        {"50", "unverzögerter Überstromschutz", "instantaneous overcurrent relay"},
        {"51", "abhängiger/verzögerter Überstromschutz", "AC time overcurrent relay"},
        {"52", "Leistungsschalter (AC)", "AC circuit breaker"},
        {"59", "Überspannungsschutz", "overvoltage relay"},
        {"63", "Druck-/Buchholzschutz", "pressure switch (e.g. Buchholz)"},
        {"64", "Erdschlussschutz", "ground detector relay"},
        {"67", "gerichteter Überstromschutz", "AC directional overcurrent relay"},
        {"79", "Wiedereinschaltrelais (AWE)", "AC reclosing relay"},
        {"81", "Frequenzschutz (81O/81U)", "frequency relay (81O/81U)"},
        {"86", "Verriegelungs-/Lockout-Relais", "lockout relay"},
        {"87", "Differentialschutz", "differential protective relay"},
        {"89", "Trennschalter", "line switch / disconnector"},
        {"94", "Auslöserelais", "tripping relay"},
    };
    std::vector<Row> rows;
    for (const auto& x : d) rows.push_back({{x[0], x[0]}, {x[1], x[2]}});
    return make("ansi_device_numbers", {"ANSI-Gerätenummern (IEEE C37.2)", "ANSI device numbers (IEEE C37.2)"},
                {"Schutzfunktionen in Schutzgeräten und Plänen", "Protection functions in relays and drawings"},
                {"IEEE C37.2 (ANSI): Nummern für Schutz- und Schaltfunktionen, z. B. 50/51 für Überstromzeitschutz. Zusätze: "
                 "N = Neutralleiter/Erde (berechnet), G = Erde (gemessen), P = Phase, V = spannungsabhängig (51V), T = Transformator (87T).",
                 "IEEE C37.2 (ANSI): numbers for protection and switching functions, e.g. 50/51 for overcurrent protection. Suffixes: "
                 "N = neutral/ground (residual), G = ground (measured), P = phase, V = voltage restrained (51V), T = transformer (87T)."},
                {{"Nr.", "No."}, {"Funktion", "Function"}}, rows);
}

Table deviceAcronyms() {
    const std::vector<std::array<const char*, 3>> d{
        {"MCB", "Leitungsschutzschalter (LS)", "miniature circuit breaker"},
        {"MCCB", "Kompaktleistungsschalter", "moulded-case circuit breaker"},
        {"ACB", "offener Leistungsschalter", "air circuit breaker"},
        {"MPCB", "Motorschutzschalter", "motor protection circuit breaker"},
        {"RCD", "Fehlerstrom-Schutzeinrichtung", "residual current device"},
        {"RCCB", "FI-Schutzschalter (ohne Überstromauslöser)", "residual current circuit breaker"},
        {"RCBO", "FI/LS-Schalter", "RCCB with overcurrent protection"},
        {"AFDD", "Brandschutzschalter", "arc fault detection device"},
        {"SPD", "Überspannungsschutzgerät", "surge protective device"},
        {"IMD", "Isolationsüberwachungsgerät", "insulation monitoring device"},
        {"RCM", "Differenzstromüberwachung", "residual current monitor"},
        {"OLR", "Überlastrelais (Motorschutzrelais)", "overload relay"},
        {"VFD / VSD", "Frequenzumrichter (FU)", "variable frequency / speed drive"},
        {"UPS", "unterbrechungsfreie Stromversorgung (USV)", "uninterruptible power supply"},
        {"ATS", "automatischer Netzumschalter", "automatic transfer switch"},
        {"CT / VT", "Strom-/Spannungswandler", "current / voltage transformer"},
        {"PLC", "speicherprogrammierbare Steuerung (SPS)", "programmable logic controller"},
        {"HMI", "Bedien- und Beobachtungsgerät", "human-machine interface"},
        {"SELV / PELV", "Schutzkleinspannung / Funktionskleinspannung mit Erdung", "safety / protective extra-low voltage"},
        {"MV / LV", "Mittelspannung / Niederspannung (MS/NS)", "medium / low voltage"},
        {"EMC", "elektromagnetische Verträglichkeit (EMV)", "electromagnetic compatibility"},
        {"LOTO", "Freischalten und Sichern (Lockout-Tagout)", "lockout-tagout"},
    };
    std::vector<Row> rows;
    for (const auto& x : d) rows.push_back({{x[0], x[0]}, {x[1], x[1]}, {x[2], x[2]}});
    return make("device_acronyms", {"Geräteabkürzungen", "Device acronyms"}, {"Englische Kürzel und deutsche Bezeichnung", "English acronyms and German names"},
                {"Häufige Abkürzungen aus Datenblättern und Normen (IEC/IEV) mit deutscher Bezeichnung.",
                 "Common acronyms in data sheets and standards (IEC/IEV) with the German name."},
                {{"Kürzel", "Acronym"}, {"Deutsch", "German"}, {"Englisch", "English"}}, rows);
}

// ------------------------------------------------------------------ world, plugs and sockets

Table worldElectricity() {
    struct C {
        const char* de;
        const char* en;
        const char* v;
        const char* hz;
        const char* plugs;
    };
    const std::vector<C> c{
        {"Afghanistan", "Afghanistan", "220", "50", "C, F"},          {"Ägypten", "Egypt", "220", "50", "C, F"},
        {"Argentinien", "Argentina", "220", "50", "C, I"},            {"Australien", "Australia", "230", "50", "I"},
        {"Belgien", "Belgium", "230", "50", "C, E"},                  {"Brasilien", "Brazil", "127 / 220", "60", "C, N"},
        {"Chile", "Chile", "220", "50", "C, L"},                      {"China", "China", "220", "50", "A, C, I"},
        {"Dänemark", "Denmark", "230", "50", "C, E, F, K"},           {"Deutschland", "Germany", "230", "50", "C, F"},
        {"Finnland", "Finland", "230", "50", "C, F"},                 {"Frankreich", "France", "230", "50", "C, E"},
        {"Griechenland", "Greece", "230", "50", "C, F"},              {"Großbritannien", "United Kingdom", "230", "50", "G"},
        {"Indien", "India", "230", "50", "C, D, M"},                  {"Indonesien", "Indonesia", "230", "50", "C, F"},
        {"Iran", "Iran", "230", "50", "C, F"},                        {"Irland", "Ireland", "230", "50", "G"},
        {"Israel", "Israel", "230", "50", "C, H, M"},                 {"Italien", "Italy", "230", "50", "C, F, L"},
        {"Japan", "Japan", "100", "50 / 60", "A, B"},                 {"Kanada", "Canada", "120", "60", "A, B"},
        {"Kenia", "Kenya", "240", "50", "G"},                         {"Kroatien", "Croatia", "230", "50", "C, F"},
        {"Marokko", "Morocco", "220", "50", "C, E"},                  {"Mexiko", "Mexico", "127", "60", "A, B"},
        {"Neuseeland", "New Zealand", "230", "50", "I"},              {"Niederlande", "Netherlands", "230", "50", "C, F"},
        {"Nigeria", "Nigeria", "230", "50", "D, G"},                  {"Norwegen", "Norway", "230", "50", "C, F"},
        {"Österreich", "Austria", "230", "50", "C, F"},               {"Pakistan", "Pakistan", "230", "50", "C, D"},
        {"Philippinen", "Philippines", "220", "60", "A, B, C"},       {"Polen", "Poland", "230", "50", "C, E"},
        {"Portugal", "Portugal", "230", "50", "C, F"},                {"Rumänien", "Romania", "230", "50", "C, F"},
        {"Russland", "Russia", "230", "50", "C, F"},                  {"Saudi-Arabien", "Saudi Arabia", "230", "60", "G"},
        {"Schweden", "Sweden", "230", "50", "C, F"},                  {"Schweiz", "Switzerland", "230", "50", "C, J"},
        {"Singapur", "Singapore", "230", "50", "G"},                  {"Spanien", "Spain", "230", "50", "C, F"},
        {"Südafrika", "South Africa", "230", "50", "C, D, M, N"},     {"Südkorea", "South Korea", "220", "60", "C, F"},
        {"Taiwan", "Taiwan", "110", "60", "A, B"},                    {"Thailand", "Thailand", "230", "50", "A, B, C, O"},
        {"Tschechien", "Czech Republic", "230", "50", "C, E"},        {"Türkei", "Turkey", "230", "50", "C, F"},
        {"Ukraine", "Ukraine", "230", "50", "C, F"},                  {"Ungarn", "Hungary", "230", "50", "C, F"},
        {"USA", "USA", "120", "60", "A, B"},                          {"Vereinigte Arabische Emirate", "United Arab Emirates", "230", "50", "G"},
        {"Vietnam", "Vietnam", "220", "50", "A, C"},
    };
    std::vector<Row> rows;
    for (const auto& x : c) rows.push_back({{x.de, x.en}, {x.v, x.v}, {x.hz, x.hz}, {x.plugs, x.plugs}});
    return make("world_electricity", {"Netzspannung und Stecker weltweit", "Electricity around the world"},
                {"Spannung, Frequenz und Steckertypen je Land", "Voltage, frequency and plug types per country"},
                {"Haushalts-Nennspannung (Außenleiter–Neutralleiter) und Frequenz, übliche Steckertypen (A–O, siehe Tabelle "
                 "Steckertypen). Nach IEC World Plugs; ohne Gewähr – vor Ort und am Gerät prüfen. Drehstrom meist √3 · U "
                 "(z. B. 230/400 V, 120/208 V, 127/220 V).",
                 "Domestic nominal voltage (line–neutral) and frequency, usual plug types (A–O, see table plug types). After IEC "
                 "World Plugs; without guarantee – check locally and on the appliance. Three-phase usually √3 · U (e.g. 230/400 V, "
                 "120/208 V, 127/220 V)."},
                {{"Land", "Country"}, {"Spannung in V", "Voltage in V"}, {"Frequenz in Hz", "Frequency in Hz"}, {"Stecker", "Plugs"}}, rows);
}

Table plugTypes() {
    return make("plug_types", {"Steckertypen A–O", "Plug types A–O"}, {"Haushaltsstecker weltweit (IEC-Bezeichnung)", "Domestic plugs worldwide (IEC letter)"},
                {"Typbezeichnung nach IEC World Plugs (IEC TR 60083). Bemessungsströme typisch; Länderzuordnung siehe Tabelle "
                 "„Netzspannung und Stecker weltweit“.",
                 "Type letters per IEC World Plugs (IEC TR 60083). Typical ratings; countries see table “Electricity around the world”."},
                {{"Typ", "Type"}, {"Aufbau", "Design"}, {"Bemessung", "Rating"}, {"Norm / Verbreitung", "Standard / regions"}},
                {{"A", {"2 Flachstifte", "2 flat pins"}, "15 A 125 V", {"NEMA 1-15; USA, Kanada, Japan", "NEMA 1-15; USA, Canada, Japan"}},
                 {"B", {"2 Flachstifte + Rund-Erde", "2 flat pins + round earth"}, "15 A 125 V", {"NEMA 5-15; Nordamerika", "NEMA 5-15; North America"}},
                 {"C", {"2 Rundstifte 4 mm (Eurostecker)", "2 round pins 4 mm (Europlug)"}, "2,5 A 250 V", {"CEE 7/16; Europa", "CEE 7/16; Europe"}},
                 {"D", {"3 Rundstifte (Dreieck)", "3 round pins (triangle)"}, "5 A 250 V", {"BS 546; Indien", "BS 546; India"}},
                 {"E", {"2 Rundstifte, Erdstift in der Dose", "2 round pins, earth pin in socket"}, "16 A 250 V", {"CEE 7/5; Frankreich, Belgien", "CEE 7/5; France, Belgium"}},
                 {"F", {"2 Rundstifte 4,8 mm, seitliche Erdbügel (Schuko)", "2 round pins 4.8 mm, side earth clips (Schuko)"}, "16 A 250 V",
                  {"CEE 7/3, 7/4; Deutschland, Europa", "CEE 7/3, 7/4; Germany, Europe"}},
                 {"G", {"3 Rechteckstifte, Sicherung im Stecker", "3 rectangular pins, fused plug"}, "13 A 250 V", {"BS 1363; UK, Irland", "BS 1363; UK, Ireland"}},
                 {"H", {"3 Stifte (V-Form bzw. rund)", "3 pins (V shape or round)"}, "16 A 250 V", {"SI 32; Israel", "SI 32; Israel"}},
                 {"I", {"2 schräge Flachstifte + Erde", "2 angled flat pins + earth"}, "10 A 250 V", {"AS/NZS 3112; Australien, China", "AS/NZS 3112; Australia, China"}},
                 {"J", {"3 Rundstifte (versetzt)", "3 round pins (offset)"}, "10 A 250 V", {"SN 441011 (T12/T13); Schweiz", "SN 441011 (T12/T13); Switzerland"}},
                 {"K", {"2 Rundstifte + halbrunde Erde", "2 round pins + half-round earth"}, "13 A 250 V", {"DS 60884-2-D1; Dänemark", "DS 60884-2-D1; Denmark"}},
                 {"L", {"3 Rundstifte in einer Reihe", "3 round pins in line"}, "10 / 16 A 250 V", {"CEI 23-50; Italien", "CEI 23-50; Italy"}},
                 {"M", {"3 dicke Rundstifte (Dreieck)", "3 thick round pins (triangle)"}, "15 A 250 V", {"BS 546; Südafrika, Indien", "BS 546; South Africa, India"}},
                 {"N", {"3 Rundstifte (kompakt)", "3 round pins (compact)"}, "10 / 20 A 250 V", {"IEC 60906-1, NBR 14136; Brasilien", "IEC 60906-1, NBR 14136; Brazil"}},
                 {"O", {"3 Rundstifte", "3 round pins"}, "16 A 250 V", {"TIS 166-2549; Thailand", "TIS 166-2549; Thailand"}}});
}

Table iec60320() {
    return make("iec60320_connectors", {"Gerätesteckverbinder IEC 60320", "IEC 60320 appliance couplers"},
                {"Kaltgeräte- und Warmgerätestecker C1 … C24", "Appliance couplers C1 … C24"},
                {"IEC 60320-1: ungerade Nummer = Kupplung (Buchse am Kabel), gerade = Gerätestecker (am Gerät). Bemessungsströme "
                 "für IEC-Länder; in Nordamerika teils höher (z. B. C13 15 A). Höchsttemperatur am Stift: 70 °C (kalt), 120 °C "
                 "(warm), 155 °C (heiß).",
                 "IEC 60320-1: odd number = connector (socket on the cable), even = appliance inlet. Ratings for IEC countries; "
                 "partly higher in North America (e.g. C13 15 A). Pin temperature: 70 °C (cold), 120 °C (hot), 155 °C (very hot)."},
                {{"Kupplung / Stecker", "Connector / inlet"}, {"Bemessung", "Rating"}, {"Schutzleiter", "Earth"}, {"Bezeichnung / Einsatz", "Name / use"}},
                {{"C1 / C2", "0,2 A 250 V", {"nein", "no"}, {"Rasierer, Kleingeräte", "shavers, small devices"}},
                 {"C5 / C6", "2,5 A 250 V", {"ja", "yes"}, {"„Mickey-Mouse“, Notebook-Netzteile", "“Mickey Mouse”, notebook supplies"}},
                 {"C7 / C8", "2,5 A 250 V", {"nein", "no"}, {"„Achter“, Radio, Ladegeräte", "“figure 8”, radios, chargers"}},
                 {"C13 / C14", "10 A 250 V", {"ja", "yes"}, {"Kaltgerätestecker: PC, Monitor, IT", "cold appliance: PC, monitor, IT"}},
                 {"C15 / C16", "10 A 250 V", {"ja", "yes"}, {"Warmgerät bis 120 °C (Wasserkocher)", "hot appliance up to 120 °C (kettle)"}},
                 {"C15A / C16A", "10 A 250 V", {"ja", "yes"}, {"Heißgerät bis 155 °C", "very hot up to 155 °C"}},
                 {"C17 / C18", "10 A 250 V", {"nein", "no"}, {"wie C13 ohne Erde", "as C13 without earth"}},
                 {"C19 / C20", "16 A 250 V", {"ja", "yes"}, {"Server, USV, Rechenzentrum", "servers, UPS, data centre"}},
                 {"C21 / C22", "16 A 250 V", {"ja", "yes"}, {"wie C19 bis 155 °C", "as C19 up to 155 °C"}},
                 {"C23 / C24", "16 A 250 V", {"nein", "no"}, {"wie C19 ohne Erde", "as C19 without earth"}}});
}

Table iec60309() {
    return make("iec60309_sockets", {"CEE-Steckvorrichtungen (IEC 60309)", "CEE industrial plugs (IEC 60309)"},
                {"Kennfarben, Polzahl und Ströme", "Colour codes, poles and currents"},
                {"IEC 60309-2 (CEE): Farbe = Spannungsbereich, Lage der Schutzkontaktbuchse (Uhrzeit, Nase bei 6 h) codiert "
                 "Spannung/Frequenz; Standard 50/60 Hz: 6 h (z. B. blau 230 V, rot 400 V). Bemessungsströme 16, 32, 63, 125 A; "
                 "IP44 (spritzwassergeschützt) oder IP67 (wasserdicht). Polzahl 2P+E (3-polig), 3P+E (4-polig), 3P+N+E (5-polig).",
                 "IEC 60309-2 (CEE): colour = voltage range, position of the earth contact (clock hour, key at 6 h) codes voltage/"
                 "frequency; standard 50/60 Hz: 6 h (e.g. blue 230 V, red 400 V). Ratings 16, 32, 63, 125 A; IP44 (splash-proof) "
                 "or IP67 (watertight). Poles 2P+E (3-pin), 3P+E (4-pin), 3P+N+E (5-pin)."},
                {{"Farbe", "Colour"}, {"Spannung", "Voltage"}, {"Typischer Einsatz", "Typical use"}},
                {{{"violett", "violet"}, "20–25 V", {"Kleinspannung", "extra-low voltage"}},
                 {{"weiß", "white"}, "40–50 V", {"Kleinspannung", "extra-low voltage"}},
                 {{"gelb", "yellow"}, "100–130 V", {"Baustellen (UK 110 V)", "construction sites (UK 110 V)"}},
                 {{"blau", "blue"}, "200–250 V", {"1~ 230 V: Camping, Baustelle, Marina", "1~ 230 V: camping, site, marina"}},
                 {{"rot", "red"}, "380–480 V", {"3~ 400 V: Maschinen, Baustromverteiler", "3~ 400 V: machines, site distribution"}},
                 {{"schwarz", "black"}, "500–690 V", {"Industrie, Bergbau", "industry, mining"}},
                 {{"grün", "green"}, {"> 60–500 Hz", "> 60–500 Hz"}, {"Sonderfrequenzen", "special frequencies"}}});
}

Table nemaConnectors() {
    return make("nema_connectors", {"NEMA-Steckverbinder", "NEMA connectors"}, {"Nordamerikanische Stecker nach NEMA WD 6", "North American plugs per NEMA WD 6"},
                {"NEMA WD 6: Nummer = Spannungs-/Polkonfiguration, Zahl nach dem Strich = Strom (z. B. 5-15: 125 V, 15 A); "
                 "L = Verriegelung (Twist-Lock). P = Pole, W = Leiter (inkl. Schutzleiter).",
                 "NEMA WD 6: number = voltage/pole configuration, number after the dash = current (e.g. 5-15: 125 V, 15 A); "
                 "L = locking (twist-lock). P = poles, W = wires (incl. ground)."},
                {{"Typ", "Type"}, {"Spannung", "Voltage"}, {"Strom", "Current"}, {"Pole / Leiter", "Poles / wires"}, {"Einsatz", "Use"}},
                {{"1-15", "125 V", "15 A", "2P 2W", {"ohne Erde (Altbestand)", "ungrounded (legacy)"}},
                 {"5-15", "125 V", "15 A", "2P 3W", {"Standardsteckdose", "standard receptacle"}},
                 {"5-20", "125 V", "20 A", "2P 3W", {"Küche, Werkstatt", "kitchen, workshop"}},
                 {"6-15 / 6-20", "250 V", "15 / 20 A", "2P 3W", {"Klimageräte, Werkzeuge", "air conditioners, tools"}},
                 {"6-30 / 6-50", "250 V", "30 / 50 A", "2P 3W", {"Schweißgeräte", "welders"}},
                 {"10-30 / 10-50", "125/250 V", "30 / 50 A", "3P 3W", {"alte Trockner/Herde (ohne Erde)", "old dryers/ranges (no ground)"}},
                 {"14-30", "125/250 V", "30 A", "3P 4W", {"Wäschetrockner", "clothes dryers"}},
                 {"14-50", "125/250 V", "50 A", "3P 4W", {"Herde, Wohnmobile, E-Auto-Laden", "ranges, RVs, EV charging"}},
                 {"L5-20 / L5-30", "125 V", "20 / 30 A", "2P 3W", {"verriegelbar, Industrie", "locking, industrial"}},
                 {"L6-20 / L6-30", "250 V", "20 / 30 A", "2P 3W", {"verriegelbar, USV, Server", "locking, UPS, servers"}},
                 {"L14-20 / L14-30", "125/250 V", "20 / 30 A", "3P 4W", {"Generatoren", "generators"}},
                 {"L15-20 / L15-30", {"3~ 250 V", "3~ 250 V"}, "20 / 30 A", "3P 4W", {"Drehstrom", "three-phase"}},
                 {"L21-20 / L21-30", {"3~ 120/208 V", "3~ 120/208 V"}, "20 / 30 A", "4P 5W", {"Drehstrom mit N, Rechenzentren", "three-phase with N, data centres"}}});
}

Table evPlugs() {
    return make("ev_plugs", {"Ladestecker für Elektrofahrzeuge", "EV charging plugs"}, {"Steckertypen und Lademodi", "Plug types and charging modes"},
                {"IEC 62196 (Stecker), IEC 61851-1 (Lademodi): Mode 1 Haushaltssteckdose ohne Kommunikation (in DE/CH nicht üblich), "
                 "Mode 2 Haushalts-/CEE-Steckdose mit Kabel-Schutzeinrichtung (IC-CPD), Mode 3 Ladestation AC (Wallbox), Mode 4 "
                 "DC-Schnellladung. Leistungen als Richtwerte; Fahrzeug und Ladestation begrenzen.",
                 "IEC 62196 (plugs), IEC 61851-1 (charging modes): mode 1 household socket without communication (uncommon in DE/CH), "
                 "mode 2 household/CEE socket with in-cable protection (IC-CPD), mode 3 AC charging station (wallbox), mode 4 DC fast "
                 "charging. Powers as guide values; vehicle and charger limit."},
                {{"Stecker", "Plug"}, {"Norm", "Standard"}, {"Strom / Leistung", "Current / power"}, {"Region", "Region"}},
                {{{"Typ 1", "Type 1"}, "SAE J1772, IEC 62196-2", {"AC 1~, bis 32 A (7,4 kW)", "AC 1~, up to 32 A (7.4 kW)"}, {"Nordamerika, Japan", "North America, Japan"}},
                 {{"Typ 2 (Mennekes)", "Type 2 (Mennekes)"}, "IEC 62196-2", {"AC 1~/3~, bis 63 A (43 kW), üblich 11/22 kW", "AC 1~/3~, up to 63 A (43 kW), usually 11/22 kW"},
                  {"Europa (EU-Standard)", "Europe (EU standard)"}},
                 {"CCS Combo 1", "IEC 62196-3", {"DC bis ca. 350 kW", "DC up to approx. 350 kW"}, {"Nordamerika", "North America"}},
                 {"CCS Combo 2", "IEC 62196-3", {"DC bis ca. 400 kW (1000 V, 500 A)", "DC up to approx. 400 kW (1000 V, 500 A)"}, {"Europa", "Europe"}},
                 {"CHAdeMO", "IEC 62196-3", {"DC bis 62,5 kW (v1), höher in v2", "DC up to 62.5 kW (v1), higher in v2"}, {"Japan", "Japan"}},
                 {"GB/T", "GB/T 20234", {"AC und DC (getrennte Stecker)", "AC and DC (separate plugs)"}, {"China", "China"}},
                 {"NACS", "SAE J3400", {"AC und DC über einen Stecker", "AC and DC through one plug"}, {"Nordamerika", "North America"}},
                 {{"Schuko / CEE (Mode 2)", "Schuko / CEE (mode 2)"}, "IEC 61851-1", {"1~ 8–10 A bzw. CEE 16/32 A", "1~ 8–10 A or CEE 16/32 A"},
                  {"Notladen, mobil", "emergency, mobile"}}});
}

Table wiringColoursIntl() {
    return make("wiring_colours_intl", {"Aderfarben (Europa und international)", "Conductor colours (Europe and worldwide)"},
                {"L1, L2, L3, N, PE je Land; PEN und Maschinenverdrahtung", "L1, L2, L3, N, PE per country; PEN and machine wiring"},
                {"Europa: IEC 60445 / HD 308 S2 (DIN EN 60445 / VDE 0197, seit 2004 in DE, UK nach 2004). PEN-Leiter: grün-gelb mit "
                 "blauer Markierung an den Enden (oder blau mit grün-gelber Markierung). Maschinen nach IEC 60204-1 Abschn. 13.2 "
                 "(Empfehlung): Hauptstromkreise AC/DC schwarz, AC-Steuerstromkreise rot, DC-Steuerstromkreise blau, Fremdspannung "
                 "(bleibt bei Hauptschalter AUS unter Spannung) orange. USA/Kanada: NEC/CEC schreiben nur N (weiß/grau) und Erde "
                 "(grün, grün-gelb oder blank) vor, Außenleiterfarben sind übliche Praxis. Ohne Gewähr – vor Arbeiten immer messen.",
                 "Europe: IEC 60445 / HD 308 S2 (since 2004 in DE, UK after 2004). PEN conductor: green-yellow with blue marking at the "
                 "ends (or blue with green-yellow marking). Machines per IEC 60204-1 clause 13.2 (recommendation): AC/DC power circuits "
                 "black, AC control circuits red, DC control circuits blue, external supply (live with main switch OFF) orange. "
                 "USA/Canada: NEC/CEC only mandate N (white/grey) and ground (green, green-yellow or bare), line colours are customary. "
                 "Without guarantee – always measure before work."},
                {{"Land / System", "Country / system"}, "L1", "L2", "L3", "N", "PE"},
                {{{"Europa (IEC 60445)", "Europe (IEC 60445)"}, {"braun", "brown"}, {"schwarz", "black"}, {"grau", "grey"}, {"blau", "blue"}, {"grün-gelb", "green-yellow"}},
                 {{"UK vor 2004", "UK before 2004"}, {"rot", "red"}, {"gelb", "yellow"}, {"blau", "blue"}, {"schwarz", "black"}, {"grün-gelb", "green-yellow"}},
                 {{"USA 120/208 V", "USA 120/208 V"}, {"schwarz", "black"}, {"rot", "red"}, {"blau", "blue"}, {"weiß", "white"}, {"grün", "green"}},
                 {{"USA 277/480 V", "USA 277/480 V"}, {"braun", "brown"}, {"orange", "orange"}, {"gelb", "yellow"}, {"grau", "grey"}, {"grün", "green"}},
                 {{"Kanada", "Canada"}, {"rot", "red"}, {"schwarz", "black"}, {"blau", "blue"}, {"weiß", "white"}, {"grün", "green"}},
                 {{"Australien / Neuseeland", "Australia / New Zealand"}, {"rot", "red"}, {"weiß", "white"}, {"dunkelblau", "dark blue"}, {"schwarz", "black"}, {"grün-gelb", "green-yellow"}},
                 {{"China", "China"}, {"gelb", "yellow"}, {"grün", "green"}, {"rot", "red"}, {"hellblau", "light blue"}, {"grün-gelb", "green-yellow"}}});
}

Table unitsTable() {
    const std::vector<std::array<const char*, 5>> u{
        {"Stromstärke", "current", "Ampere", "A", "SI-Basiseinheit"},
        {"Spannung", "voltage", "Volt", "V", "1 V = 1 W/A"},
        {"Widerstand", "resistance", "Ohm", "Ω", "1 Ω = 1 V/A"},
        {"Leitwert", "conductance", "Siemens", "S", "1 S = 1/Ω"},
        {"Ladung", "charge", "Coulomb", "C", "1 C = 1 A·s (1 Ah = 3600 C)"},
        {"Kapazität", "capacitance", "Farad", "F", "1 F = 1 C/V"},
        {"Induktivität", "inductance", "Henry", "H", "1 H = 1 V·s/A"},
        {"Magnetischer Fluss", "magnetic flux", "Weber", "Wb", "1 Wb = 1 V·s"},
        {"Flussdichte", "flux density", "Tesla", "T", "1 T = 1 Wb/m²"},
        {"Frequenz", "frequency", "Hertz", "Hz", "1 Hz = 1/s"},
        {"Wirkleistung", "active power", "Watt", "W", "1 W = 1 J/s = 1 V·A"},
        {"Scheinleistung", "apparent power", "Voltampere", "VA", "S = U · I"},
        {"Blindleistung", "reactive power", "Var", "var", "Q = U · I · sin φ"},
        {"Energie", "energy", "Joule", "J", "1 J = 1 W·s; 1 kWh = 3,6 MJ"},
        {"Elektrische Feldstärke", "electric field strength", "Volt pro Meter", "V/m", "E = U / d"},
        {"Magnetische Feldstärke", "magnetic field strength", "Ampere pro Meter", "A/m", "H = I · N / l"},
        {"Lichtstärke", "luminous intensity", "Candela", "cd", "SI-Basiseinheit"},
        {"Lichtstrom", "luminous flux", "Lumen", "lm", "1 lm = 1 cd·sr"},
        {"Beleuchtungsstärke", "illuminance", "Lux", "lx", "1 lx = 1 lm/m²"},
        {"Temperatur", "temperature", "Kelvin", "K", "SI-Basiseinheit; θ [°C] = T − 273,15"},
        {"Kraft", "force", "Newton", "N", "1 N = 1 kg·m/s²"},
        {"Druck", "pressure", "Pascal", "Pa", "1 Pa = 1 N/m²; 1 bar = 10⁵ Pa"},
    };
    std::vector<Row> rows;
    for (const auto& x : u) {
        std::string en = x[4];
        if (en == "SI-Basiseinheit") en = "SI base unit";
        else if (en == "SI-Basiseinheit; θ [°C] = T − 273,15") en = "SI base unit; θ [°C] = T − 273.15";
        else if (en == "1 J = 1 W·s; 1 kWh = 3,6 MJ") en = "1 J = 1 W·s; 1 kWh = 3.6 MJ";
        rows.push_back({{x[0], x[1]}, {x[2], x[2]}, {x[3], x[3]}, {x[4], en}});
    }
    return make("units_table", {"Maßeinheiten der Elektrotechnik", "Units of measurement"}, {"SI-Einheiten mit Definition", "SI units with definition"},
                {"Internationales Einheitensystem (SI, ISO 80000 / IEC 80000). Vorsätze siehe Tabelle SI-Vorsätze, Umrechnungen siehe Umrechner.",
                 "International System of Units (SI, ISO 80000 / IEC 80000). Prefixes see table SI prefixes, conversions see converters."},
                {{"Größe", "Quantity"}, {"Einheit", "Unit"}, {"Zeichen", "Symbol"}, {"Definition", "Definition"}}, rows);
}

Table pipeDimensions() {
    const std::vector<std::array<const char*, 4>> p{
        {"6", "1/8\"", "10,2", "10.2"},   {"8", "1/4\"", "13,5", "13.5"},   {"10", "3/8\"", "17,2", "17.2"},  {"15", "1/2\"", "21,3", "21.3"},
        {"20", "3/4\"", "26,9", "26.9"},  {"25", "1\"", "33,7", "33.7"},    {"32", "1 1/4\"", "42,4", "42.4"}, {"40", "1 1/2\"", "48,3", "48.3"},
        {"50", "2\"", "60,3", "60.3"},    {"65", "2 1/2\"", "76,1", "76.1"}, {"80", "3\"", "88,9", "88.9"},    {"100", "4\"", "114,3", "114.3"},
        {"125", "5\"", "139,7", "139.7"}, {"150", "6\"", "165,1 / 168,3", "165.1 / 168.3"},
    };
    std::vector<Row> rows;
    for (const auto& x : p) rows.push_back({{std::string("DN ") + x[0], std::string("DN ") + x[0]}, {x[1], x[1]}, {x[2], x[3]}});
    return make("pipe_dimensions", {"Rohrabmessungen (Stahlrohr)", "Pipe dimensions (steel)"}, {"DN, Zollgröße und Außendurchmesser", "DN, inch size and outer diameter"},
                {"Gewinderohre EN 10255 / ISO 65 bzw. nahtlose/geschweißte Rohre EN 10220 (Außendurchmesser in mm). Die Zollgröße "
                 "ist eine Nenngröße, nicht der Durchmesser. Wanddicke je Reihe (mittel/schwer) verschieden.",
                 "Threaded pipes EN 10255 / ISO 65 or seamless/welded pipes EN 10220 (outer diameter in mm). The inch size is a "
                 "nominal size, not the diameter. Wall thickness depends on the series (medium/heavy)."},
                {{"Nennweite", "Nominal size"}, {"Zoll", "Inch"}, {"Außen-Ø in mm", "Outer Ø in mm"}}, rows);
}

Table cableGlands() {
    const std::vector<std::array<const char*, 3>> m{{"M12 × 1,5", "12", "3–7"},     {"M16 × 1,5", "16", "4,5–10"}, {"M20 × 1,5", "20", "6–13"},
                                                    {"M25 × 1,5", "25", "9–17"},    {"M32 × 1,5", "32", "13–21"},  {"M40 × 1,5", "40", "16–28"},
                                                    {"M50 × 1,5", "50", "21–35"},   {"M63 × 1,5", "63", "34–48"}};
    const std::vector<std::array<const char*, 2>> pg{{"PG 7", "12,5"}, {"PG 9", "15,2"},  {"PG 11", "18,6"}, {"PG 13,5", "20,4"}, {"PG 16", "22,5"},
                                                     {"PG 21", "28,3"}, {"PG 29", "37,0"}, {"PG 36", "47,0"}, {"PG 42", "54,0"},   {"PG 48", "59,3"}};
    std::vector<Row> rows;
    auto en = [](std::string s) {
        for (auto& ch : s) {
            if (ch == ',') ch = '.';
        }
        return s;
    };
    for (const auto& x : m) rows.push_back({{x[0], en(x[0])}, {x[1], x[1]}, {x[2], en(x[2])}});
    for (const auto& x : pg) rows.push_back({{x[0], en(x[0])}, {x[1], en(x[1])}, {"–", "–"}});
    return make("cable_glands", {"Kabelverschraubungen und Installationsrohre", "Cable glands and conduits"},
                {"Metrische Gewinde (M) und Panzerrohrgewinde (PG)", "Metric (M) and PG threads"},
                {"Metrisch EN 62444 / EN 50262: Gewinde-Außen-Ø = Kennzahl (= Außendurchmesser der Installationsrohre M16–M63 nach "
                 "EN 61386); Klemmbereich typisch, herstellerabhängig. PG (DIN 40430, nicht mehr genormt): Gewinde-Außen-Ø.",
                 "Metric EN 62444 / EN 50262: thread outer Ø = size number (= outer diameter of conduits M16–M63 per EN 61386); "
                 "clamping range typical, depends on manufacturer. PG (DIN 40430, withdrawn): thread outer Ø."},
                {{"Gewinde", "Thread"}, {"Gewinde-Ø in mm", "Thread Ø in mm"}, {"Klemmbereich in mm", "Clamping range in mm"}}, rows);
}

// ------------------------------------------------------------------ drawings

/// Time-current curves of MCBs per IEC 60898-1 test points (schematic between them)
Calculator tripCurves() {
    Calculator c;
    c.id = "trip_curves";
    c.category = kCat;
    c.title = {"Auslösekennlinien", "Tripping curves"};
    c.summary = {"Zeit-Strom-Bereich von LS-Schaltern B, C, D (IEC 60898-1)", "Time-current zone of MCBs B, C, D (IEC 60898-1)"};
    c.description = {
        "Auslösebereich zwischen Nichtauslöse- und Auslösekennlinie nach den Prüfpunkten der IEC 60898-1: 1,13 · I_n ≥ 1 h, "
        "1,45 · I_n < 1 h, 2,55 · I_n 1 … 60 s (I_n ≤ 32 A), magnetisch B 3–5, C 5–10, D 10–20 · I_n (< 0,1 s). Zwischen den "
        "Prüfpunkten schematisch (im log-log-Diagramm gerade). Genaue Kennlinien liefert der Hersteller.\n"
        "Mit Kurzschlussstrom: Auslösezeit-Bereich am Fehlerort (Abschaltbedingung: Auslösung im magnetischen Bereich).",
        "Tripping zone between no-trip and trip curve from the IEC 60898-1 test points: 1.13 · I_n ≥ 1 h, 1.45 · I_n < 1 h, "
        "2.55 · I_n 1 … 60 s (I_n ≤ 32 A), magnetic B 3–5, C 5–10, D 10–20 · I_n (< 0.1 s). Between the test points schematic "
        "(straight in the log-log chart). Exact curves from the manufacturer.\n"
        "With a fault current: tripping time zone at the fault (disconnection: trip in the magnetic range)."};
    c.inputs = {Num::choice("type", {"Charakteristik", "Characteristic"}, {{"B", "B"}, {"C", "C"}, {"D", "D"}}, "B"),
                Num::number("In", {"Bemessungsstrom I_n", "Rated current I_n"}, "A", 16).positive(),
                Num::number("Ik", {"Fehler-/Kurzschlussstrom", "Fault current"}, "A", 200).positive().opt()};
    c.outputs = {Out::number("Ia", {"Sicher magnetisch ab", "Magnetic trip guaranteed from"}, "A", 0).primary(),
                 Out::number("Inot", {"Keine Schnellauslösung bis", "No instantaneous trip up to"}, "A", 0),
                 Out::number("ratio", {"I_k / I_n", "I_k / I_n"}, "", 1),
                 Out::text("zone", {"Bereich bei I_k", "Zone at I_k"})};
    c.keywords = {"auslösekennlinie", "kennlinie", "ls-schalter", "b16", "c16", "tripping curve", "time current"};
    auto limits = [](const std::string& t) -> std::pair<double, double> {
        if (t == "C") return {5, 10};
        if (t == "D") return {10, 20};
        return {3, 5};
    };
    c.compute = [limits](const Inputs& in, const Context&) {
        Result r;
        const auto [lo, hi] = limits(in.choice("type"));
        const double In = in.num("In");
        r.set("Ia", hi * In);
        r.set("Inot", lo * In);
        if (const auto ik = in.maybe("Ik")) {
            const double x = *ik / In;
            r.set("ratio", x);
            if (x >= hi) {
                r.setText("zone", {"magnetisch: Auslösung < 0,1 s", "magnetic: trips < 0.1 s"});
                r.ok({"Abschaltung im magnetischen Bereich sichergestellt.", "Disconnection in the magnetic range ensured."});
            } else if (x >= lo) {
                r.setText("zone", {"Übergangsbereich: Auslösung unsicher < 0,1 s", "transition zone: < 0.1 s not guaranteed"});
                r.warn({"I_k liegt im Streubereich des Magnetauslösers – Abschaltzeit nicht sicher.", "I_k is in the tolerance band of the magnetic release – break time not guaranteed."});
            } else if (x >= 1.13) {
                r.setText("zone", {"thermisch: Auslösung nach Sekunden bis Minuten", "thermal: trips after seconds to minutes"});
                r.error({"Nur thermische Auslösung – Abschaltbedingung (0,4 s/5 s) nicht erfüllt.", "Thermal tripping only – disconnection time (0.4 s/5 s) not met."});
            } else {
                r.setText("zone", {"keine Auslösung", "no tripping"});
            }
        }
        return r;
    };
    c.chart = [limits](const Inputs& in, const Result&, const Context&) {
        Chart ch;
        ch.x = {{"Vielfaches des Bemessungsstroms I/I_n", "Multiple of rated current I/I_n"}, 1, 100, 1};
        ch.y = {{"Auslösezeit t in s", "Tripping time t in s"}, 0.001, 10000, 1};
        ch.xLog = true;
        ch.yLog = true;
        const auto [lo, hi] = limits(in.choice("type"));
        // thermal no-trip line through (1.13, 3600) and (2.55, 1); trip line through (1.45, 3600) and (2.55, 60)
        auto thermal = [](double x, double x0, double t0, double x1, double t1) {
            const double k = std::log(t1 / t0) / std::log(x1 / x0);
            return t0 * std::pow(x / x0, k);
        };
        auto curve = [&](double x0, double t0, double x1, double t1, double xMag, double tEnd) {
            std::vector<std::array<double, 2>> pts{{x0, 10000}, {x0, t0}};
            double x = x0;
            for (int i = 1; i <= 60; ++i) {
                x = x0 * std::pow(xMag / x0, i / 60.0);
                const double t = thermal(x, x0, t0, x1, t1);
                if (t <= 0.1) {  // thermal branch reaches 0.1 s before the magnetic threshold
                    const double xr = x0 * std::pow(0.1 / t0, std::log(x1 / x0) / std::log(t1 / t0));
                    pts.push_back({xr, 0.1});
                    pts.push_back({xMag, 0.1});
                    break;
                }
                pts.push_back({x, t});
            }
            pts.push_back({xMag, tEnd});
            return pts;
        };
        ChartSeries lower{"aux", curve(1.13, 3600, 2.55, 1, lo, 0.001)};
        auto up = curve(1.45, 3600, 2.55, 60, hi, 0.01);
        up.push_back({100, 0.004});
        ChartSeries upper{"process", up};
        ch.series = {lower, upper};
        ch.labels.push_back({lo * 0.97, 0.003, {"Nichtauslösen", "no trip"}, "aux"});
        ch.labels.push_back({std::min(95.0, hi * 3.2), 0.02, {"Auslösen", "trip"}, "process"});
        if (const auto ik = in.maybe("Ik")) {
            const double x = std::clamp(*ik / in.num("In"), 1.0, 100.0);
            ch.markers.push_back({x, 0.05, "I_k", "state"});
        }
        ch.note = {"Bereich zwischen den Kurven = Streuband der Auslösung (I_n ≤ 32 A). Gestrichelt: darf nicht auslösen; durchgezogen: muss ausgelöst haben.",
                   "Zone between the curves = tripping band (I_n ≤ 32 A). Dashed: must not trip; solid: must have tripped."};
        return ch;
    };
    return c;
}

/// Earthing systems TN-S, TN-C, TN-C-S, TT, IT as single drawing per system
Calculator earthingSystems() {
    Calculator c;
    c.id = "earthing_systems";
    c.category = kCat;
    c.title = {"Netzsysteme TN, TT, IT (Schaltbild)", "Earthing systems TN, TT, IT (diagram)"};
    c.summary = {"Aufbau von TN-S, TN-C, TN-C-S, TT und IT mit Zeichnung", "Layout of TN-S, TN-C, TN-C-S, TT and IT with drawing"};
    c.description = {"IEC 60364-1 / DIN VDE 0100-100: 1. Buchstabe Erdung der Stromquelle (T = direkt geerdet, I = isoliert oder "
                     "hochohmig), 2. Buchstabe Erdung der Körper (T = eigener Erder, N = über den Schutzleiter mit dem Sternpunkt). "
                     "S = N und PE getrennt, C = kombiniert als PEN. Schutzmaßnahmen siehe Rechner Erdung/RCD und Abschaltbedingung.",
                     "IEC 60364-1: 1st letter earthing of the source (T = directly earthed, I = isolated or high impedance), 2nd letter "
                     "earthing of exposed parts (T = own earth electrode, N = via the protective conductor to the star point). "
                     "S = N and PE separate, C = combined as PEN. Protection see calculators earthing/RCD and disconnection."};
    c.inputs = {Num::choice("sys", {"Netzsystem", "System"},
                            {{"tns", "TN-S"}, {"tnc", "TN-C"}, {"tncs", "TN-C-S"}, {"tt", "TT"}, {"it", "IT"}}, "tns")};
    c.outputs = {Out::text("src", {"Stromquelle", "Source"}).primary(), Out::text("body", {"Körper (Gehäuse)", "Exposed parts"}),
                 Out::text("protection", {"Fehlerschutz", "Fault protection"}), Out::text("note", {"Hinweis", "Note"})};
    c.keywords = {"tn-s", "tn-c", "tt", "it", "netzsystem", "netzform", "earthing system", "pen"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& s = in.choice("sys");
        if (s == "tt") {
            r.setText("src", {"Sternpunkt geerdet (Betriebserder R_B)", "star point earthed (system earth R_B)"});
            r.setText("body", {"eigener Anlagenerder R_A", "own earth electrode R_A"});
            r.setText("protection", {"RCD, R_A · I_Δn ≤ 50 V", "RCD, R_A · I_Δn ≤ 50 V"});
            r.setText("note", {"Fehlerstrom fließt über das Erdreich zurück – klein, daher RCD nötig.", "Fault current returns through earth – small, hence RCD needed."});
        } else if (s == "it") {
            r.setText("src", {"isoliert oder hochohmig geerdet", "isolated or earthed via high impedance"});
            r.setText("body", {"geerdet (einzeln oder gemeinsam)", "earthed (individually or together)"});
            r.setText("protection", {"Isolationsüberwachung (IMD); Abschaltung beim 2. Fehler", "insulation monitoring (IMD); disconnection on 2nd fault"});
            r.setText("note", {"Betrieb nach dem 1. Fehler möglich (Medizin, Industrie, Schiffe).", "Operation continues after the 1st fault (medical, industry, ships)."});
        } else {
            r.setText("src", {"Sternpunkt direkt geerdet", "star point directly earthed"});
            r.setText("body", {"über Schutzleiter mit dem Sternpunkt verbunden", "connected to the star point via the protective conductor"});
            r.setText("protection", {"Überstrom-Schutzeinrichtung (Z_s · I_a ≤ U₀) oder RCD", "overcurrent device (Z_s · I_a ≤ U₀) or RCD"});
            if (s == "tnc") r.setText("note", {"PEN-Leiter ≥ 10 mm² Cu / 16 mm² Al; keine RCD möglich; EMV-ungünstig.", "PEN ≥ 10 mm² Cu / 16 mm² Al; no RCD possible; poor EMC."});
            else if (s == "tncs") r.setText("note", {"PEN-Aufteilung einmal (meist im Hausanschluss), danach N und PE nie wieder verbinden.", "PEN split once (usually at the service), afterwards never reconnect N and PE."});
            else r.setText("note", {"N und PE ab der Quelle getrennt – Standard für Neuanlagen, EMV-günstig.", "N and PE separate from the source – standard for new installations, good EMC."});
        }
        return r;
    };
    c.chart = [](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 66, 0.66);
        const auto& sys = in.choice("sys");
        const char* pe = "line:#43A047";
        const char* nl = "line:#1E88E5";
        const std::array<double, 3> yl{60, 55, 50};
        const std::array<const char*, 3> L{"L1", "L2", "L3"};
        // source
        s.rect("bgfill", 3, 28, 19, 63);
        s.label(11, 47, {"Quelle", "Source"}, "cb");
        for (std::size_t i = 0; i < 3; ++i) {
            s.line("ink", {{19, yl[i]}, {97, yl[i]}});
            s.label(22, yl[i] + 2.2, L[i], "l");
        }
        // consumer with exposed conductive part
        s.rect("bgfill", 60, 8, 86, 28);
        s.label(73, 18, {"Verbraucher", "Load"}, "c");
        for (std::size_t i = 0; i < 3; ++i) s.line("ink", {{64.0 + 4.0 * static_cast<double>(i), yl[i]}, {64.0 + 4.0 * static_cast<double>(i), 28}});
        auto earth = [&](double x, double y, const char* name) {
            s.line("ink", {{x, y}, {x, y - 4}});
            s.line("ink", {{x - 3.5, y - 4}, {x + 3.5, y - 4}});
            s.line("ink", {{x - 2.3, y - 5.4}, {x + 2.3, y - 5.4}});
            s.line("ink", {{x - 1.1, y - 6.8}, {x + 1.1, y - 6.8}});
            s.label(x + 3, y - 2, name, "l");
        };
        // star point of the source
        if (sys == "it") {
            s.line("sig", {{11, 28}, {11, 20}});
            s.rect("bgfill", 8.5, 14, 13.5, 20);
            s.label(11, 17, "Z", "c");
            s.line("ink", {{11, 14}, {11, 12}});
            earth(11, 12, "");
            s.label(16, 23, {"isoliert / Z", "isolated / Z"}, "l");
        } else {
            s.line("ink", {{11, 28}, {11, 14}});
            earth(11, 14, "R_B");
        }
        const double yN = 43, yPE = 36;
        if (sys == "tns") {
            s.line(nl, {{19, yN}, {97, yN}});
            s.line(pe, {{11, 24}, {26, 24}, {26, yPE}, {97, yPE}});
            s.label(29, yN + 2.2, "N", "l");
            s.label(29, yPE + 2.2, "PE", "l");
            s.line(nl, {{78, yN}, {78, 28}});
            s.line(pe, {{90, yPE}, {90, 18}, {86, 18}});
        } else if (sys == "tnc") {
            s.line(pe, {{11, 24}, {26, 24}, {26, 40}, {97, 40}});
            s.label(29, 42.2, "PEN", "l");
            s.line(pe, {{78, 40}, {78, 28}});
            s.line(pe, {{90, 40}, {90, 18}, {86, 18}});
        } else if (sys == "tncs") {
            s.line(pe, {{11, 24}, {26, 24}, {26, 40}, {44, 40}});
            s.label(29, 42.2, "PEN", "l");
            s.line(nl, {{44, 40}, {44, yN}, {97, yN}});
            s.line(pe, {{44, 40}, {44, yPE}, {97, yPE}});
            s.circle("inkfill", 44, 40, 1.1, 12);
            s.label(50, yN + 2.2, "N", "l");
            s.label(50, yPE - 2.4, "PE", "l");
            s.line(nl, {{78, yN}, {78, 28}});
            s.line(pe, {{90, yPE}, {90, 18}, {86, 18}});
        } else {
            // TT: neutral from the source; IT: usually without neutral (IEC 60364-4-41 recommends not distributing it);
            // exposed part to its own earth electrode
            if (sys == "tt") {
                s.line(nl, {{19, yN}, {97, yN}});
                s.label(22, yN + 2.2, "N", "l");
                s.line(nl, {{78, yN}, {78, 28}});
            }
            s.line(pe, {{86, 18}, {92, 18}, {92, 14}});
            earth(92, 14, "R_A");
            if (sys == "it") {
                s.rect("bgfill", 30, 12, 42, 20);
                s.label(36, 16, "IMD", "cb");
                s.line("sig", {{36, 20}, {36, yl[2]}});
            }
        }
        s.ch.note = {"Grün: Schutzleiter PE/PEN, blau: Neutralleiter N. Körper = leitfähiges Gehäuse des Verbrauchers.",
                     "Green: protective conductor PE/PEN, blue: neutral N. Exposed part = conductive enclosure of the load."};
        return s.ch;
    };
    return c;
}

// ---- electrical symbols (IEC 60617, simplified)

void lead(Sketch& s, double cx, double cy, double half, double len = 12) {
    s.line("ink", {{cx - len, cy}, {cx - half, cy}});
    s.line("ink", {{cx + half, cy}, {cx + len, cy}});
}

void arrowHead(Sketch& s, P from, P to, double size = 2.2) {
    const double dx = to[0] - from[0], dy = to[1] - from[1];
    const double l = std::hypot(dx, dy);
    const double ux = dx / l, uy = dy / l;
    s.closed("inkfill", {to, {to[0] - ux * size - uy * size * 0.5, to[1] - uy * size + ux * size * 0.5},
                         {to[0] - ux * size + uy * size * 0.5, to[1] - uy * size - ux * size * 0.5}});
}

void arrow(Sketch& s, P from, P to) {
    s.line("ink", {from, to});
    arrowHead(s, from, to);
}

void diodeBody(Sketch& s, double cx, double cy) {
    lead(s, cx, cy, 4);
    s.closed("inkfill", {{cx - 4, cy + 4}, {cx - 4, cy - 4}, {cx + 3, cy}});
}

void transistor(Sketch& s, double cx, double cy, bool npn) {
    s.circle("bgfill", cx + 1, cy, 7.5, 32);
    s.line("ink", {{cx - 12, cy}, {cx - 3, cy}});
    s.line("ink", {{cx - 3, cy - 4.5}, {cx - 3, cy + 4.5}});
    s.line("ink", {{cx - 3, cy + 2}, {cx + 4, cy + 6}, {cx + 4, cy + 11}});
    s.line("ink", {{cx - 3, cy - 2}, {cx + 4, cy - 6}, {cx + 4, cy - 11}});
    if (npn) arrowHead(s, {cx - 3, cy - 2}, {cx + 3, cy - 5.4});
    else arrowHead(s, {cx + 4, cy - 6}, {cx - 2, cy - 2.6});
}

void earthSymbol(Sketch& s, double cx, double cy) {
    s.line("ink", {{cx, cy + 7}, {cx, cy}});
    s.line("ink", {{cx - 5, cy}, {cx + 5, cy}});
    s.line("ink", {{cx - 3.2, cy - 2}, {cx + 3.2, cy - 2}});
    s.line("ink", {{cx - 1.4, cy - 4}, {cx + 1.4, cy - 4}});
}

void contact(Sketch& s, double cx, double cy, int kind) {
    // kind 0 = make (NO), 1 = break (NC), 2 = changeover
    s.line("ink", {{cx - 12, cy}, {cx - 4, cy}});
    if (kind == 0) {
        s.line("ink", {{cx - 4, cy}, {cx + 4.5, cy + 4.5}});
        s.line("ink", {{cx + 4, cy}, {cx + 12, cy}});
    } else if (kind == 1) {
        s.line("ink", {{cx + 4, cy}, {cx + 12, cy}});
        s.line("ink", {{cx + 4, cy}, {cx + 4, cy - 3}});
        s.line("ink", {{cx - 4, cy}, {cx + 6, cy - 3.6}});
    } else {
        s.line("ink", {{cx + 4, cy + 4}, {cx + 12, cy + 4}});
        s.line("ink", {{cx + 4, cy - 4}, {cx + 12, cy - 4}});
        s.line("ink", {{cx + 4, cy - 4}, {cx + 4, cy - 6}});
        s.line("ink", {{cx - 4, cy}, {cx + 6, cy - 6.4}});
    }
}

void lettered(Sketch& s, double cx, double cy, const char* letter) {
    lead(s, cx, cy, 6.5);
    s.circle("bgfill", cx, cy, 6.5, 32);
    s.label(cx, cy, letter, "cb");
}

struct Symbol {
    Text name;
    std::function<void(Sketch&, double, double)> draw;
};

std::vector<Symbol> symbolPage(const std::string& page) {
    if (page == "semi") {
        return {
            {{"Diode", "Diode"}, [](Sketch& s, double x, double y) { diodeBody(s, x, y); s.line("ink", {{x + 3, y - 4}, {x + 3, y + 4}}); }},
            {{"Z-Diode", "Zener diode"}, [](Sketch& s, double x, double y) {
                 diodeBody(s, x, y);
                 s.line("ink", {{x + 1.5, y - 5}, {x + 3, y - 4}, {x + 3, y + 4}, {x + 4.5, y + 5}});
             }},
            {{"Schottky-Diode", "Schottky diode"}, [](Sketch& s, double x, double y) {
                 diodeBody(s, x, y);
                 s.line("ink", {{x + 1.5, y - 2.5}, {x + 1.5, y - 4}, {x + 3, y - 4}, {x + 3, y + 4}, {x + 4.5, y + 4}, {x + 4.5, y + 2.5}});
             }},
            {{"Leuchtdiode (LED)", "Light-emitting diode"}, [](Sketch& s, double x, double y) {
                 diodeBody(s, x, y);
                 s.line("ink", {{x + 3, y - 4}, {x + 3, y + 4}});
                 arrow(s, {x - 1, y + 5}, {x + 3, y + 9.5});
                 arrow(s, {x + 2, y + 5}, {x + 6, y + 9.5});
             }},
            {{"Fotodiode", "Photodiode"}, [](Sketch& s, double x, double y) {
                 diodeBody(s, x, y);
                 s.line("ink", {{x + 3, y - 4}, {x + 3, y + 4}});
                 arrow(s, {x + 3, y + 10}, {x - 1, y + 5.5});
                 arrow(s, {x + 6, y + 10}, {x + 2, y + 5.5});
             }},
            {{"Thyristor", "Thyristor"}, [](Sketch& s, double x, double y) {
                 diodeBody(s, x, y);
                 s.line("ink", {{x + 3, y - 4}, {x + 3, y + 4}});
                 s.line("ink", {{x + 3, y - 1.5}, {x + 7, y - 7}});
             }},
            {{"Transistor NPN", "Transistor NPN"}, [](Sketch& s, double x, double y) { transistor(s, x, y, true); }},
            {{"Transistor PNP", "Transistor PNP"}, [](Sketch& s, double x, double y) { transistor(s, x, y, false); }},
        };
    }
    if (page == "switch") {
        return {
            {{"Schließer", "Make contact (NO)"}, [](Sketch& s, double x, double y) { contact(s, x, y, 0); }},
            {{"Öffner", "Break contact (NC)"}, [](Sketch& s, double x, double y) { contact(s, x, y, 1); }},
            {{"Wechsler", "Changeover contact"}, [](Sketch& s, double x, double y) { contact(s, x, y, 2); }},
            {{"Taster (Schließer)", "Push button (NO)"}, [](Sketch& s, double x, double y) {
                 contact(s, x, y, 0);
                 s.line("sig", {{x, y + 2.3}, {x, y + 8}});
                 s.line("ink", {{x - 3, y + 6.5}, {x - 3, y + 8}, {x + 3, y + 8}, {x + 3, y + 6.5}});
             }},
            {{"Schütz-/Relaisspule", "Contactor/relay coil"}, [](Sketch& s, double x, double y) {
                 lead(s, x, y, 6);
                 s.rect("bgfill", x - 6, y - 4, x + 6, y + 4);
             }},
            {{"Leistungsschalter", "Circuit breaker"}, [](Sketch& s, double x, double y) {
                 contact(s, x, y, 0);
                 s.line("ink", {{x + 2.6, y - 1.4}, {x + 5.4, y + 1.4}});
                 s.line("ink", {{x + 2.6, y + 1.4}, {x + 5.4, y - 1.4}});
             }},
            {{"Thermischer Auslöser", "Thermal release"}, [](Sketch& s, double x, double y) {
                 lead(s, x, y, 6);
                 s.rect("bgfill", x - 6, y - 4, x + 6, y + 4);
                 s.line("ink", {{x - 3, y - 2}, {x - 3, y + 2}, {x + 3, y + 2}, {x + 3, y - 2}});
             }},
            {{"Erde", "Earth"}, [](Sketch& s, double x, double y) { earthSymbol(s, x, y - 2); }},
            {{"Schutzerde (PE)", "Protective earth (PE)"}, [](Sketch& s, double x, double y) {
                 s.circle("bgfill", x, y - 1, 7, 32);
                 s.line("ink", {{x, y + 3}, {x, y}});
                 s.line("ink", {{x - 3.6, y}, {x + 3.6, y}});
                 s.line("ink", {{x - 2.2, y - 1.8}, {x + 2.2, y - 1.8}});
                 s.line("ink", {{x - 0.9, y - 3.6}, {x + 0.9, y - 3.6}});
             }},
        };
    }
    if (page == "source") {
        return {
            {{"Spannungsquelle", "Voltage source"}, [](Sketch& s, double x, double y) {
                 lead(s, x, y, 6.5);
                 s.circle("bgfill", x, y, 6.5, 32);
                 s.line("ink", {{x - 6.5, y}, {x + 6.5, y}});
             }},
            {{"Stromquelle", "Current source"}, [](Sketch& s, double x, double y) {
                 lead(s, x, y, 6.5);
                 s.circle("bgfill", x, y, 6.5, 32);
                 s.line("ink", {{x, y - 6.5}, {x, y + 6.5}});
             }},
            {{"Wechselspannungsquelle", "AC source"}, [](Sketch& s, double x, double y) { lettered(s, x, y, "~"); }},
            {{"Strommesser", "Ammeter"}, [](Sketch& s, double x, double y) { lettered(s, x, y, "A"); }},
            {{"Spannungsmesser", "Voltmeter"}, [](Sketch& s, double x, double y) { lettered(s, x, y, "V"); }},
            {{"Leistungsmesser", "Wattmeter"}, [](Sketch& s, double x, double y) { lettered(s, x, y, "W"); }},
            {{"Lampe / Leuchtmelder", "Lamp / indicator"}, [](Sketch& s, double x, double y) {
                 lead(s, x, y, 6.5);
                 s.circle("bgfill", x, y, 6.5, 32);
                 s.line("ink", {{x - 4.6, y - 4.6}, {x + 4.6, y + 4.6}});
                 s.line("ink", {{x - 4.6, y + 4.6}, {x + 4.6, y - 4.6}});
             }},
            {{"Motor", "Motor"}, [](Sketch& s, double x, double y) { lettered(s, x, y, "M"); }},
            {{"Generator", "Generator"}, [](Sketch& s, double x, double y) { lettered(s, x, y, "G"); }},
        };
    }
    if (page == "install") {
        return {
            {{"Steckdose", "Socket outlet"}, [](Sketch& s, double x, double y) {
                 std::vector<P> arc;
                 for (int i = 0; i <= 16; ++i) {
                     const double a = std::numbers::pi * i / 16.0;
                     arc.push_back({x + 5 * std::cos(a), y - 1 + 5 * std::sin(a)});
                 }
                 s.line("ink", arc);
                 s.line("ink", {{x - 5, y - 1}, {x + 5, y - 1}});
                 s.line("ink", {{x, y - 1}, {x, y - 7}});
             }},
            {{"Schutzkontaktsteckdose", "Socket with earthing contact"}, [](Sketch& s, double x, double y) {
                 std::vector<P> arc;
                 for (int i = 0; i <= 16; ++i) {
                     const double a = std::numbers::pi * i / 16.0;
                     arc.push_back({x + 5 * std::cos(a), y - 1 + 5 * std::sin(a)});
                 }
                 s.line("ink", arc);
                 s.line("ink", {{x - 5, y - 1}, {x + 5, y - 1}});
                 s.line("ink", {{x, y - 1}, {x, y - 7}});
                 s.line("ink", {{x - 5, y + 4}, {x + 5, y + 4}});
             }},
            {{"Ausschalter", "One-way switch"}, [](Sketch& s, double x, double y) {
                 s.circle("bgfill", x - 4, y - 4, 1.6, 16);
                 s.line("ink", {{x - 2.9, y - 2.9}, {x + 5, y + 5}, {x + 7, y + 3}});
             }},
            {{"Serienschalter", "Two-circuit switch"}, [](Sketch& s, double x, double y) {
                 s.circle("bgfill", x - 4, y - 4, 1.6, 16);
                 s.line("ink", {{x - 2.9, y - 2.9}, {x + 5, y + 5}, {x + 7, y + 3}});
                 s.line("ink", {{x + 2.4, y + 2.4}, {x + 4.4, y + 0.4}});
             }},
            {{"Wechselschalter", "Two-way switch"}, [](Sketch& s, double x, double y) {
                 s.circle("bgfill", x, y, 1.6, 16);
                 s.line("ink", {{x + 1.1, y + 1.1}, {x + 6, y + 6}, {x + 8, y + 4}});
                 s.line("ink", {{x - 1.1, y - 1.1}, {x - 6, y - 6}, {x - 8, y - 4}});
             }},
            {{"Taster", "Push button"}, [](Sketch& s, double x, double y) {
                 s.circle("bgfill", x, y, 3.5, 24);
                 s.circle("inkfill", x, y, 1.2, 12);
             }},
            {{"Leuchte", "Luminaire"}, [](Sketch& s, double x, double y) {
                 s.line("ink", {{x, y - 9}, {x, y}});
                 s.line("ink", {{x - 4.5, y - 4.5}, {x + 4.5, y + 4.5}});
                 s.line("ink", {{x - 4.5, y + 4.5}, {x + 4.5, y - 4.5}});
             }},
            {{"Abzweigdose", "Junction box"}, [](Sketch& s, double x, double y) {
                 s.line("ink", {{x - 10, y}, {x + 10, y}});
                 s.line("ink", {{x, y}, {x, y - 8}});
                 s.circle("inkfill", x, y, 2.2, 16);
             }},
            {{"Verteiler", "Distribution board"}, [](Sketch& s, double x, double y) {
                 s.rect("bgfill", x - 7, y - 4, x + 7, y + 4);
                 s.closed("inkfill", {{x - 7, y - 4}, {x + 7, y - 4}, {x + 7, y + 4}});
             }},
        };
    }
    // passive components
    return {
        {{"Widerstand", "Resistor"}, [](Sketch& s, double x, double y) { lead(s, x, y, 6); s.rect("bgfill", x - 6, y - 2.5, x + 6, y + 2.5); }},
        {{"Einstellbarer Widerstand", "Variable resistor"}, [](Sketch& s, double x, double y) {
             lead(s, x, y, 6);
             s.rect("bgfill", x - 6, y - 2.5, x + 6, y + 2.5);
             arrow(s, {x - 6, y - 6}, {x + 6.5, y + 6.5});
         }},
        {{"Potentiometer", "Potentiometer"}, [](Sketch& s, double x, double y) {
             lead(s, x, y, 6);
             s.rect("bgfill", x - 6, y - 2.5, x + 6, y + 2.5);
             s.line("ink", {{x, y + 9}, {x, y + 3}});
             arrowHead(s, {x, y + 9}, {x, y + 2.6});
         }},
        {{"Kondensator", "Capacitor"}, [](Sketch& s, double x, double y) {
             lead(s, x, y, 1.3);
             s.line("ink", {{x - 1.3, y - 5}, {x - 1.3, y + 5}});
             s.line("ink", {{x + 1.3, y - 5}, {x + 1.3, y + 5}});
         }},
        {{"Elektrolytkondensator", "Electrolytic capacitor"}, [](Sketch& s, double x, double y) {
             lead(s, x, y, 1.3);
             s.rect("bgfill", x - 2.6, y - 5, x - 1.3, y + 5);
             s.line("ink", {{x + 1.3, y - 5}, {x + 1.3, y + 5}});
             s.label(x - 5.5, y + 5, "+", "cb");
         }},
        {{"Spule / Induktivität", "Inductor"}, [](Sketch& s, double x, double y) {
             s.line("ink", {{x - 12, y}, {x - 8, y}});
             s.line("ink", {{x + 8, y}, {x + 12, y}});
             std::vector<P> pts;
             for (int k = 0; k < 4; ++k) {
                 for (int i = 0; i <= 10; ++i) {
                     const double a = std::numbers::pi * (1.0 - i / 10.0);
                     pts.push_back({x - 6 + 4 * k + 2 * std::cos(a), y + 2.5 * std::sin(a)});
                 }
             }
             s.line("ink", pts);
         }},
        {{"Transformator", "Transformer"}, [](Sketch& s, double x, double y) {
             s.line("ink", {{x - 12, y}, {x - 8.2, y}});
             s.line("ink", {{x + 8.2, y}, {x + 12, y}});
             s.circle("ink", x - 3, y, 5.2, 32);
             s.circle("ink", x + 3, y, 5.2, 32);
         }},
        {{"Sicherung", "Fuse"}, [](Sketch& s, double x, double y) {
             lead(s, x, y, 6);
             s.rect("bgfill", x - 6, y - 2.5, x + 6, y + 2.5);
             s.line("ink", {{x - 6, y}, {x + 6, y}});
         }},
        {{"Batterie / Zelle", "Battery / cell"}, [](Sketch& s, double x, double y) {
             lead(s, x, y, 1.5);
             s.line("ink", {{x - 1.5, y - 6}, {x - 1.5, y + 6}});
             s.rect("inkfill", x + 0.9, y - 3, x + 2.1, y + 3);
             s.label(x - 5, y + 5.5, "+", "cb");
         }},
    };
}

Calculator electricalSymbols() {
    Calculator c;
    c.id = "electrical_symbols";
    c.category = kCat;
    c.title = {"Schaltzeichen", "Electrical symbols"};
    c.summary = {"Grafische Symbole nach IEC 60617 (vereinfacht)", "Graphical symbols per IEC 60617 (simplified)"};
    c.description = {"Schaltzeichen nach IEC 60617 / DIN EN 60617 (Stromlaufpläne) und Installationssymbole nach DIN EN 60617-11 "
                     "(Installationspläne), vereinfacht gezeichnet. Bauteilkennbuchstaben nach IEC 81346-2 (z. B. R Widerstand, "
                     "C Kondensator, Q Schaltgerät, K Relais, F Schutzgerät, M Motor, P Messgerät, E Leuchte).",
                     "Symbols per IEC 60617 (circuit diagrams) and installation symbols per IEC 60617-11 (installation plans), simplified. "
                     "Reference letters per IEC 81346-2 (e.g. R resistor, C capacitor, Q switching device, K relay, F protective "
                     "device, M motor, P meter, E luminaire)."};
    c.inputs = {Num::choice("page", {"Gruppe", "Group"},
                            {{"passive", {"Passive Bauteile", "Passive components"}}, {"semi", {"Halbleiter", "Semiconductors"}},
                             {"switch", {"Schalter und Schutz", "Switches and protection"}},
                             {"source", {"Quellen, Messgeräte, Verbraucher", "Sources, meters, loads"}},
                             {"install", {"Installationspläne", "Installation plans"}}},
                            "passive")};
    c.outputs = {Out::text("list", {"Symbole", "Symbols"}).primary()};
    c.keywords = {"schaltzeichen", "symbol", "iec 60617", "schaltplan", "symbole", "electrical symbols"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        Text list{"", ""};
        for (const auto& sym : symbolPage(in.choice("page"))) {
            list.de += (list.de.empty() ? "" : ", ") + sym.name.de;
            list.en += (list.en.empty() ? "" : ", ") + sym.name.en;
        }
        r.setText("list", list);
        return r;
    };
    c.chart = [](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 96, 0.96);
        const auto page = symbolPage(in.choice("page"));
        const std::array<double, 3> xs{17, 50, 83};
        const std::array<double, 3> ys{80, 49, 18};
        for (std::size_t i = 0; i < page.size() && i < 9; ++i) {
            const double x = xs[i % 3], y = ys[i / 3];
            page[i].draw(s, x, y);
            s.label(x, y - 13, page[i].name);
        }
        s.ch.note = {"Vereinfachte Darstellung nach IEC 60617.", "Simplified drawing per IEC 60617."};
        return s.ch;
    };
    return c;
}

}  // namespace

void addElectricalResources(std::vector<Calculator>& out) {
    out.push_back(tripCurves());
    out.push_back(earthingSystems());
    out.push_back(electricalSymbols());
}

void addElectricalResourceTables(std::vector<Table>& out) {
    out.push_back(ulFuseClasses());
    out.push_back(tripCharacteristics());
    out.push_back(eSeriesTable());
    out.push_back(cableReactance());
    out.push_back(resistivityTable());
    out.push_back(vdropPerLength());
    out.push_back(conductorData());
    out.push_back(ikCodes());
    out.push_back(nemaEnclosures());
    out.push_back(cctvResolutions());
    out.push_back(thermocoupleTable());
    out.push_back(ansiDeviceNumbers());
    out.push_back(deviceAcronyms());
    out.push_back(worldElectricity());
    out.push_back(plugTypes());
    out.push_back(iec60320());
    out.push_back(iec60309());
    out.push_back(nemaConnectors());
    out.push_back(evPlugs());
    out.push_back(wiringColoursIntl());
    out.push_back(unitsTable());
    out.push_back(pipeDimensions());
    out.push_back(cableGlands());
}

}  // namespace elc::catalog
