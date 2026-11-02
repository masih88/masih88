// Area "Elektrotechnik" (v1.7): colour bands from a value (drawing), SMD/capacitor/inductor codes, current divider,
// Zener stabiliser, series resistor, transformer windings, antennas, CCTV storage, copper RTD, thermocouples.
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"
#include "elc/formulas/electrical.hpp"
#include "elc/formulas/electronics.hpp"
#include "elc/formulas/thermocouple.hpp"

namespace elc::catalog {

using namespace elc::electrical;

namespace {

constexpr const char* kCat = "electrical";

struct BandColour {
    Text name;
    const char* hex;
};
const std::array<BandColour, 12>& bandColours() {
    static const std::array<BandColour, 12> c{{
        {{"Schwarz", "Black"}, "#1A1A1A"}, {{"Braun", "Brown"}, "#8B4513"},  {{"Rot", "Red"}, "#E53935"},
        {{"Orange", "Orange"}, "#FB8C00"}, {{"Gelb", "Yellow"}, "#FDD835"},  {{"Grün", "Green"}, "#43A047"},
        {{"Blau", "Blue"}, "#1E88E5"},     {{"Violett", "Violet"}, "#8E24AA"}, {{"Grau", "Grey"}, "#9E9E9E"},
        {{"Weiß", "White"}, "#FFFFFF"},    {{"Gold", "Gold"}, "#C9A227"},    {{"Silber", "Silver"}, "#C0C0C0"},
    }};
    return c;
}

/// colour index for a multiplier exponent: 0…9 = black…white, −1 = gold, −2 = silver
int multiplierColour(int exp) { return exp >= 0 ? exp : exp == -1 ? 10 : 11; }

/// Ω, kΩ, MΩ with a readable value
std::pair<double, std::string> ohmPrefix(double v) {
    if (v >= 1e6) return {v / 1e6, "MΩ"};
    if (v >= 1e3) return {v / 1e3, "kΩ"};
    if (v > 0 && v < 1) return {v * 1e3, "mΩ"};
    return {v, "Ω"};
}

std::pair<double, std::string> faradPrefix(double pF) {
    if (pF >= 1e6) return {pF / 1e6, "µF"};
    if (pF >= 1e3) return {pF / 1e3, "nF"};
    return {pF, "pF"};
}

// ------------------------------------------------------------------ colour bands from a value

Calculator colourFromValue() {
    Calculator c;
    c.id = "color_from_value";
    c.category = kCat;
    c.title = {"Widerstandsfarbcode aus Wert bestimmen", "Resistor colour code from value"};
    c.summary = {"Wert → Farbringe (4 oder 5 Ringe) mit Zeichnung", "Value → colour bands (4 or 5 bands) with drawing"};
    c.description = {"IEC 60062: 4 Ringe = 2 Ziffern, Multiplikator, Toleranz (E6–E24); 5 Ringe = 3 Ziffern, Multiplikator, "
                     "Toleranz (E48–E192). Multiplikator Gold = ×0,1, Silber = ×0,01. Der Wert wird auf 2 bzw. 3 gültige "
                     "Ziffern gerundet; geprüft wird, ob er in der E24- bzw. E96-Reihe liegt.",
                     "IEC 60062: 4 bands = 2 digits, multiplier, tolerance (E6–E24); 5 bands = 3 digits, multiplier, "
                     "tolerance (E48–E192). Multiplier gold = ×0.1, silver = ×0.01. The value is rounded to 2 or 3 significant "
                     "digits and checked against the E24 or E96 series."};
    c.inputs = {Num::number("R", {"Widerstandswert", "Resistance"}, "", 4.7).positive(),
                Num::choice("unit", {"Einheit", "Unit"}, {{"ohm", "Ω"}, {"k", "kΩ"}, {"M", "MΩ"}}, "k"),
                Num::choice("bands", {"Anzahl Ringe", "Number of bands"}, {{"4", {"4 Ringe", "4 bands"}}, {"5", {"5 Ringe", "5 bands"}}}, "4"),
                Num::choice("tol", {"Toleranz", "Tolerance"},
                            {{"10", {"±1 % – Braun", "±1 % – Brown"}}, {"20", {"±2 % – Rot", "±2 % – Red"}},
                             {"05", {"±0,5 % – Grün", "±0.5 % – Green"}}, {"025", {"±0,25 % – Blau", "±0.25 % – Blue"}},
                             {"01", {"±0,1 % – Violett", "±0.1 % – Violet"}}, {"50", {"±5 % – Gold", "±5 % – Gold"}},
                             {"100", {"±10 % – Silber", "±10 % – Silver"}}},
                            "50")};
    c.outputs = {Out::text("colours", {"Farbringe", "Colour bands"}).primary(),
                 Out::number("value", {"Codierter Wert", "Coded value"}, "Ω", 3),
                 Out::number("nearest", {"Nächster Normwert", "Nearest standard value"}, "Ω", 3),
                 Out::text("series", {"Normreihe", "Standard series"})};
    c.keywords = {"farbcode", "farbringe", "widerstand", "colour code", "color code", "bands"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double mult = in.choice("unit") == "M" ? 1e6 : in.choice("unit") == "k" ? 1e3 : 1.0;
        const double R = in.num("R") * mult;
        const bool five = in.choice("bands") == "5";
        const auto b = bandsFor(R, five ? 3 : 2);
        if (!b) throw CalcError({"Wert mit Farbringen nicht darstellbar (0,1 Ω … 99 GΩ).", "Value cannot be shown with colour bands (0.1 Ω … 99 GΩ)."});
        Text names{"", ""};
        for (const int d : b->digits) {
            names.de += bandColours()[static_cast<std::size_t>(d)].name.de + " – ";
            names.en += bandColours()[static_cast<std::size_t>(d)].name.en + " – ";
        }
        const auto& m = bandColours()[static_cast<std::size_t>(multiplierColour(b->exponent))];
        const std::string tol = in.choice("tol");
        const int tolIdx = tol == "10" ? 1 : tol == "20" ? 2 : tol == "05" ? 5 : tol == "025" ? 6 : tol == "01" ? 7 : tol == "100" ? 11 : 10;
        const auto& t = bandColours()[static_cast<std::size_t>(tolIdx)];
        names.de += m.name.de + " – " + t.name.de;
        names.en += m.name.en + " – " + t.name.en;
        r.setText("colours", names);
        const auto [v, unit] = ohmPrefix(b->value);
        r.set("value", v, unit);
        const auto series = five ? electronics::ESeries::E96 : electronics::ESeries::E24;
        const auto match = electronics::eSeriesMatch(series, b->value);
        const auto [nv, nunit] = ohmPrefix(match.nearest);
        r.set("nearest", nv, nunit);
        const bool exact = std::fabs(match.nearest - b->value) <= 1e-6 * b->value;
        r.setText("series", exact ? Text{five ? "E96 ✓" : "E24 ✓", five ? "E96 ✓" : "E24 ✓"}
                                  : Text{five ? "kein E96-Wert" : "kein E24-Wert", five ? "not an E96 value" : "not an E24 value"});
        if (std::fabs(b->value - R) > 1e-9 * R)
            r.warn({"Wert auf die darstellbaren Ziffern gerundet.", "Value rounded to the digits the bands can show."});
        return r;
    };
    c.chart = [](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 40, 0.4);
        const double mult = in.choice("unit") == "M" ? 1e6 : in.choice("unit") == "k" ? 1e3 : 1.0;
        const bool five = in.choice("bands") == "5";
        const auto b = bandsFor(in.num("R") * mult, five ? 3 : 2);
        s.line("ink", {{2, 20}, {22, 20}});
        s.line("ink", {{78, 20}, {98, 20}});
        s.rect(five ? "fill:#A9CCE3" : "fill:#E8D5A9", 22, 9, 78, 31);
        if (!b) return s.ch;
        std::vector<int> idx(b->digits.begin(), b->digits.end());
        idx.push_back(multiplierColour(b->exponent));
        const std::string tol = in.choice("tol");
        idx.push_back(tol == "10" ? 1 : tol == "20" ? 2 : tol == "05" ? 5 : tol == "025" ? 6 : tol == "01" ? 7 : tol == "100" ? 11 : 10);
        const std::vector<double> xs = five ? std::vector<double>{28, 35, 42, 49, 68} : std::vector<double>{30, 38, 46, 68};
        const std::vector<const char*> tags = five ? std::vector<const char*>{"1", "2", "3", "×", "±"}
                                                   : std::vector<const char*>{"1", "2", "×", "±"};
        for (std::size_t i = 0; i < xs.size(); ++i) {
            s.rect(std::string("fill:") + bandColours()[static_cast<std::size_t>(idx[i])].hex, xs[i] - 2.2, 9, xs[i] + 2.2, 31);
            s.label(xs[i], 4, tags[i]);
        }
        s.ch.note = {"Ringe von links lesen; der Toleranzring hat den größeren Abstand.",
                     "Read the bands from the left; the tolerance band has the larger gap."};
        return s.ch;
    };
    return c;
}

// ------------------------------------------------------------------ codes

Calculator smdCode() {
    Calculator c;
    c.id = "smd_resistor";
    c.category = kCat;
    c.title = {"SMD-Widerstandscode", "SMD resistor code"};
    c.summary = {"3- und 4-stellig, R-Schreibweise und EIA-96", "3 and 4 digits, R notation and EIA-96"};
    c.description = {"3-stellig (E24): zwei Ziffern + Zehnerpotenz, z. B. 472 = 47 · 10² = 4,7 kΩ\n"
                     "4-stellig (E96, 1 %): drei Ziffern + Zehnerpotenz, z. B. 4702 = 47 kΩ\n"
                     "R = Dezimalkomma: 4R7 = 4,7 Ω, R47 = 0,47 Ω; 0 / 000 = 0-Ω-Brücke\n"
                     "EIA-96 (1 %): zwei Ziffern = Index der E96-Reihe (01 = 100 … 96 = 976) + Buchstabe: "
                     "Z ×0,001, Y/R ×0,01, X/S ×0,1, A ×1, B/H ×10, C ×100, D ×1000, E ×10⁴, F ×10⁵. Beispiel 01C = 10 kΩ.",
                     "3 digits (E24): two digits + power of ten, e.g. 472 = 47 · 10² = 4.7 kΩ\n"
                     "4 digits (E96, 1 %): three digits + power of ten, e.g. 4702 = 47 kΩ\n"
                     "R = decimal point: 4R7 = 4.7 Ω, R47 = 0.47 Ω; 0 / 000 = 0 Ω jumper\n"
                     "EIA-96 (1 %): two digits = index of the E96 series (01 = 100 … 96 = 976) + letter: "
                     "Z ×0.001, Y/R ×0.01, X/S ×0.1, A ×1, B/H ×10, C ×100, D ×1000, E ×10⁴, F ×10⁵. Example 01C = 10 kΩ."};
    c.inputs = {InputDef::text("code", {"Aufdruck", "Marking"}, "472")};
    c.outputs = {Out::number("R", {"Widerstand", "Resistance"}, "Ω", 3).primary(), Out::text("kind", {"Codierung", "Code type"})};
    c.keywords = {"smd", "widerstand", "code", "eia-96", "resistor marking"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto v = smdResistor(in.text("code"));
        if (!v) throw CalcError({"Code nicht erkannt (Beispiele: 472, 4702, 4R7, 01C).", "Code not recognised (examples: 472, 4702, 4R7, 01C)."});
        const auto [val, unit] = ohmPrefix(v->ohms);
        r.set("R", val, v->ohms == 0 ? "Ω" : unit);
        if (v->kind == "3") r.setText("kind", {"3-stellig (±5 %, E24)", "3 digits (±5 %, E24)"});
        else if (v->kind == "4") r.setText("kind", {"4-stellig (±1 %, E96)", "4 digits (±1 %, E96)"});
        else if (v->kind == "EIA-96") r.setText("kind", {"EIA-96 (±1 %)", "EIA-96 (±1 %)"});
        else if (v->kind == "0") r.setText("kind", {"0-Ω-Brücke", "0 Ω jumper"});
        else r.setText("kind", {"R-Schreibweise (R = Komma)", "R notation (R = decimal point)"});
        return r;
    };
    return c;
}

Calculator capacitorCodeCalc() {
    Calculator c;
    c.id = "capacitor_code";
    c.category = kCat;
    c.title = {"Kondensator-Code", "Capacitor code"};
    c.summary = {"Aufdruck → Kapazität, Toleranz, Spannung", "Marking → capacitance, tolerance, voltage"};
    c.description = {"3 Ziffern (pF): zwei Ziffern + Zehnerpotenz, 104 = 10 · 10⁴ pF = 100 nF; 8 = ×0,01, 9 = ×0,1.\n"
                     "Buchstabe als Komma: 4n7 = 4,7 nF, 47p = 47 pF, 2u2 = 2,2 µF, 1R5 = 1,5 pF; Dezimalzahl (0.1, .047) = µF.\n"
                     "Toleranz: B ±0,1 pF, C ±0,25 pF, D ±0,5 pF, F ±1 %, G ±2 %, J ±5 %, K ±10 %, M ±20 %, Z +80/−20 %.\n"
                     "Spannung (EIA, vorangestellt): z. B. 1H = 50 V, 2A = 100 V, 2J = 630 V.",
                     "3 digits (pF): two digits + power of ten, 104 = 10 · 10⁴ pF = 100 nF; 8 = ×0.01, 9 = ×0.1.\n"
                     "Letter as decimal point: 4n7 = 4.7 nF, 47p = 47 pF, 2u2 = 2.2 µF, 1R5 = 1.5 pF; decimal number (0.1, .047) = µF.\n"
                     "Tolerance: B ±0.1 pF, C ±0.25 pF, D ±0.5 pF, F ±1 %, G ±2 %, J ±5 %, K ±10 %, M ±20 %, Z +80/−20 %.\n"
                     "Voltage (EIA, prefixed): e.g. 1H = 50 V, 2A = 100 V, 2J = 630 V."};
    c.inputs = {InputDef::text("code", {"Aufdruck", "Marking"}, "2A104K")};
    c.outputs = {Out::number("C", {"Kapazität", "Capacitance"}, "nF", 3).primary(),
                 Out::number("pF", {"Kapazität in pF", "Capacitance in pF"}, "pF", 1),
                 Out::text("tol", {"Toleranz", "Tolerance"}),
                 Out::number("U", {"Nennspannung", "Rated voltage"}, "V", 1)};
    c.keywords = {"kondensator", "code", "104", "pf", "capacitor marking"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto v = capacitorCode(in.text("code"));
        if (!v) throw CalcError({"Code nicht erkannt (Beispiele: 104K, 4n7, 2A473J, 0.1).", "Code not recognised (examples: 104K, 4n7, 2A473J, 0.1)."});
        const auto [val, unit] = faradPrefix(v->pF);
        r.set("C", val, unit);
        r.set("pF", v->pF);
        if (!v->toleranceText.empty()) {
            std::string en = v->toleranceText;
            for (auto& ch : en) {
                if (ch == ',') ch = '.';
            }
            r.setText("tol", {v->toleranceText, en});
        }
        if (v->voltage) r.set("U", *v->voltage);
        return r;
    };
    return c;
}

Calculator inductorCodeCalc() {
    Calculator c;
    c.id = "inductor_code";
    c.category = kCat;
    c.title = {"Alphanumerischer Induktivitätscode", "Alphanumeric inductor code"};
    c.summary = {"101 = 100 µH, 4R7 = 4,7 µH, 47N = 47 nH", "101 = 100 µH, 4R7 = 4.7 µH, 47N = 47 nH"};
    c.description = {"Grundeinheit µH: zwei Ziffern + Zehnerpotenz (101 = 10 · 10¹ µH = 100 µH); R = Komma in µH (4R7 = 4,7 µH, "
                     "R10 = 0,1 µH); N = Komma in nH (47N = 47 nH, 4N7 = 4,7 nH). Nachgestellt Toleranz J ±5 %, K ±10 %, M ±20 %.",
                     "Base unit µH: two digits + power of ten (101 = 10 · 10¹ µH = 100 µH); R = decimal point in µH (4R7 = 4.7 µH, "
                     "R10 = 0.1 µH); N = decimal point in nH (47N = 47 nH, 4N7 = 4.7 nH). Suffix tolerance J ±5 %, K ±10 %, M ±20 %."};
    c.inputs = {InputDef::text("code", {"Aufdruck", "Marking"}, "4R7")};
    c.outputs = {Out::number("L", {"Induktivität", "Inductance"}, "µH", 3).primary(),
                 Out::number("nH", {"Induktivität in nH", "Inductance in nH"}, "nH", 1)};
    c.keywords = {"induktivität", "spule", "drossel", "code", "inductor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto v = inductorCode(in.text("code"));
        if (!v) throw CalcError({"Code nicht erkannt (Beispiele: 101, 4R7, 47N).", "Code not recognised (examples: 101, 4R7, 47N)."});
        if (*v >= 1000) r.set("L", *v / 1000.0, "mH");
        else if (*v < 1) r.set("L", *v * 1000.0, "nH");
        else r.set("L", *v);
        r.set("nH", *v * 1000.0);
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ circuits

Calculator currentDivider() {
    Calculator c;
    c.id = "current_divider";
    c.category = kCat;
    c.title = {"Stromteiler", "Current divider"};
    c.summary = {"Aufteilung eines Stroms auf parallele Widerstände", "Splitting a current across parallel resistors"};
    c.description = {"Parallelschaltung: U = I · R_ges, 1/R_ges = Σ 1/R_i    I_i = U / R_i\nZwei Widerstände: I₁ = I · R₂ / (R₁ + R₂).",
                     "Parallel circuit: U = I · R_tot, 1/R_tot = Σ 1/R_i    I_i = U / R_i\nTwo resistors: I₁ = I · R₂ / (R₁ + R₂)."};
    c.inputs = {Num::number("I", {"Gesamtstrom I", "Total current I"}, "A", 1).positive(),
                Num::number("R1", {"R₁", "R₁"}, "Ω", 100).positive(),
                Num::number("R2", {"R₂", "R₂"}, "Ω", 220).positive(),
                Num::number("R3", {"R₃ (optional)", "R₃ (optional)"}, "Ω", 470).positive().opt()};
    c.outputs = {Out::number("I1", {"Strom durch R₁", "Current through R₁"}, "A", 4).primary(),
                 Out::number("I2", {"Strom durch R₂", "Current through R₂"}, "A", 4),
                 Out::number("I3", {"Strom durch R₃", "Current through R₃"}, "A", 4),
                 Out::number("U", {"Spannung U", "Voltage U"}, "V", 3),
                 Out::number("R", {"Ersatzwiderstand R_ges", "Equivalent resistance R_tot"}, "Ω", 3)};
    c.keywords = {"stromteiler", "parallel", "current divider"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        double g = 1.0 / in.num("R1") + 1.0 / in.num("R2");
        const auto R3 = in.maybe("R3");
        if (R3) g += 1.0 / *R3;
        const double U = in.num("I") / g;
        r.set("R", 1.0 / g);
        r.set("U", U);
        r.set("I1", U / in.num("R1"));
        r.set("I2", U / in.num("R2"));
        if (R3) r.set("I3", U / *R3);
        return r;
    };
    return c;
}

Calculator zenerCalc() {
    Calculator c;
    c.id = "zener_regulator";
    c.category = kCat;
    c.title = {"Zener-Diode als Spannungsstabilisator", "Zener diode as voltage regulator"};
    c.summary = {"Vorwiderstand und Belastung der Z-Diode", "Series resistor and Zener diode dissipation"};
    c.description = {"R_v = (U_e,min − U_Z) / (I_Z,min + I_L,max)  →  nächstkleinerer E12-Wert\n"
                     "I_Z,max = (U_e,max − U_Z) / R_v − I_L,min    P_Z = U_Z · I_Z,max    P_R = (U_e,max − U_Z)² / R_v\n"
                     "I_Z,min typ. 5 mA (bzw. 10 % von I_Z,max). Z-Diode nach P_Z mit Reserve wählen (z. B. 0,5 W, 1,3 W).",
                     "R_v = (U_in,min − U_Z) / (I_Z,min + I_L,max)  →  next lower E12 value\n"
                     "I_Z,max = (U_in,max − U_Z) / R_v − I_L,min    P_Z = U_Z · I_Z,max    P_R = (U_in,max − U_Z)² / R_v\n"
                     "I_Z,min typ. 5 mA (or 10 % of I_Z,max). Choose the Zener by P_Z with margin (e.g. 0.5 W, 1.3 W)."};
    c.inputs = {Num::number("UeMin", {"Eingangsspannung min.", "Input voltage min."}, "V", 12).positive(),
                Num::number("UeMax", {"Eingangsspannung max.", "Input voltage max."}, "V", 15).positive(),
                Num::number("Uz", {"Zenerspannung U_Z", "Zener voltage U_Z"}, "V", 5.1).positive(),
                Num::number("IzMin", {"Min. Zenerstrom I_Z,min", "Min. Zener current I_Z,min"}, "mA", 5).positive(),
                Num::number("ILmin", {"Laststrom min.", "Load current min."}, "mA", 0).nonNegative(),
                Num::number("ILmax", {"Laststrom max.", "Load current max."}, "mA", 20).nonNegative(),
                Num::number("Pz", {"Belastbarkeit der Z-Diode (optional)", "Zener power rating (optional)"}, "W", 0.5).positive().opt()};
    c.outputs = {Out::number("Rv", {"Vorwiderstand (E12)", "Series resistor (E12)"}, "Ω", 0).primary(),
                 Out::number("Rcalc", {"Vorwiderstand rechnerisch", "Series resistor calculated"}, "Ω", 1),
                 Out::number("PR", {"Verlustleistung R_v", "Power in R_v"}, "W", 3),
                 Out::number("IzMax", {"Max. Zenerstrom", "Max. Zener current"}, "mA", 1),
                 Out::number("PzMax", {"Max. Verlustleistung Z-Diode", "Max. Zener dissipation"}, "W", 3)};
    c.keywords = {"zener", "z-diode", "stabilisierung", "regulator"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double Uz = in.num("Uz");
        if (in.num("UeMin") <= Uz) throw CalcError({"U_e,min muss größer als U_Z sein.", "U_in,min must exceed U_Z."});
        if (in.num("UeMax") < in.num("UeMin")) throw CalcError({"U_e,max < U_e,min.", "U_in,max < U_in,min."});
        const auto calc = zenerDesign(in.num("UeMin"), in.num("UeMax"), Uz, in.num("IzMin") / 1e3, in.num("ILmin") / 1e3, in.num("ILmax") / 1e3);
        const auto e12 = electronics::eSeriesValues(electronics::ESeries::E12);
        const double Rv = nextAtMost(e12, calc.Rv).value_or(calc.Rv);
        const double IzMax = (in.num("UeMax") - Uz) / Rv - in.num("ILmin") / 1e3;
        r.set("Rcalc", calc.Rv);
        r.set("Rv", Rv);
        r.set("PR", (in.num("UeMax") - Uz) * (in.num("UeMax") - Uz) / Rv);
        r.set("IzMax", IzMax * 1e3);
        r.set("PzMax", Uz * IzMax);
        if (const auto pz = in.maybe("Pz")) {
            if (Uz * IzMax > *pz) r.error({"Z-Diode überlastet (Leerlauf bei U_e,max).", "Zener diode overloaded (no load at U_in,max)."});
            else r.ok({"Z-Diode ausreichend belastbar.", "Zener diode rating sufficient."});
        }
        return r;
    };
    return c;
}

Calculator seriesResistor() {
    Calculator c;
    c.id = "series_resistor";
    c.category = kCat;
    c.title = {"Vorwiderstand zur Spannungsreduzierung", "Series resistor for voltage reduction"};
    c.summary = {"R = (U − U_L) / I, Leistung und Normwert", "R = (U − U_L) / I, power and standard value"};
    c.description = {"R_v = (U − U_L) / I_L    P_R = (U − U_L) · I_L    Wirkungsgrad η = U_L / U\n"
                     "Nur für konstante Last geeignet – bei schwankendem Strom ändert sich U_L (dann Spannungsregler verwenden).",
                     "R_v = (U − U_L) / I_L    P_R = (U − U_L) · I_L    Efficiency η = U_L / U\n"
                     "Only for a constant load – with varying current U_L changes (use a voltage regulator instead)."};
    c.inputs = {Num::number("U", {"Versorgungsspannung U", "Supply voltage U"}, "V", 24).positive(),
                Num::number("UL", {"Spannung am Verbraucher U_L", "Voltage at the load U_L"}, "V", 12).positive(),
                Num::number("I", {"Strom des Verbrauchers I_L", "Load current I_L"}, "mA", 100).positive()};
    c.outputs = {Out::number("R", {"Vorwiderstand R_v", "Series resistor R_v"}, "Ω", 1).primary(),
                 Out::number("Rn", {"Nächster E12-Wert", "Nearest E12 value"}, "Ω", 1),
                 Out::number("ULn", {"U_L mit E12-Wert", "U_L with E12 value"}, "V", 2),
                 Out::number("P", {"Verlustleistung P_R", "Power P_R"}, "W", 3),
                 Out::number("eta", {"Wirkungsgrad", "Efficiency"}, "%", 1)};
    c.keywords = {"vorwiderstand", "spannung reduzieren", "series resistor", "dropping resistor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double U = in.num("U"), UL = in.num("UL"), I = in.num("I") / 1e3;
        if (UL >= U) throw CalcError({"U_L muss kleiner als U sein.", "U_L must be smaller than U."});
        const double R = (U - UL) / I;
        const double Rn = nearestLog(electronics::eSeriesValues(electronics::ESeries::E12), R);
        const double Rload = UL / I;
        r.set("R", R);
        r.set("Rn", Rn);
        r.set("ULn", U * Rload / (Rload + Rn));
        r.set("P", (U - UL) * I);
        r.set("eta", UL / U * 100.0);
        r.info({"Belastbarkeit des Widerstands mit Reserve wählen (≥ 2 · P_R).", "Choose the resistor rating with margin (≥ 2 · P_R)."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ transformer, antenna, CCTV

Calculator transformerWindings() {
    Calculator c;
    c.id = "transformer_windings";
    c.category = kCat;
    c.title = {"Primär-/Sekundärwicklung von Transformatoren", "Primary/secondary windings of transformers"};
    c.summary = {"Übersetzung, Ströme und Windungszahlen (Kleintransformator)", "Ratio, currents and number of turns (small transformer)"};
    c.description = {"ü = U₁/U₂ = N₁/N₂ = I₂/I₁    Windungen je Volt w = 1 / (4,44 · f · B · A_Fe)\n"
                     "N₁ = w · U₁    N₂ = w · U₂ · (1 + Zuschlag) (Ausgleich der Spannungsabfälle, Kleintrafos 5–10 %)\n"
                     "B: Dynamoblech 1,0–1,4 T (Kernblech EI). Ohne Angabe A_Fe ≈ √S cm² (Faustformel für EI-Kerne).",
                     "ratio = U₁/U₂ = N₁/N₂ = I₂/I₁    Turns per volt w = 1 / (4.44 · f · B · A_core)\n"
                     "N₁ = w · U₁    N₂ = w · U₂ · (1 + allowance) (compensates voltage drops, small transformers 5–10 %)\n"
                     "B: electrical steel 1.0–1.4 T (EI laminations). If not given A_core ≈ √S cm² (rule of thumb for EI cores)."};
    c.inputs = {Num::number("S", {"Bemessungsleistung S", "Rated power S"}, "VA", 100).positive(),
                Num::number("U1", {"Primärspannung U₁", "Primary voltage U₁"}, "V", 230).positive(),
                Num::number("U2", {"Sekundärspannung U₂", "Secondary voltage U₂"}, "V", 24).positive(),
                Num::choice("f", {"Frequenz", "Frequency"}, {{"50", "50 Hz"}, {"60", "60 Hz"}}, "50"),
                Num::number("B", {"Flussdichte B", "Flux density B"}, "T", 1.2).rangeExclusiveMin(0, 2),
                Num::number("A", {"Eisenquerschnitt A_Fe (optional)", "Core cross-section A_core (optional)"}, "cm²", 10).positive().opt(),
                Num::number("add", {"Zuschlag Sekundärwindungen", "Allowance secondary turns"}, "%", 5).range(0, 30)};
    c.outputs = {Out::number("ratio", {"Übersetzung ü", "Turns ratio"}, "", 3).primary(),
                 Out::number("I1", {"Primärstrom I₁", "Primary current I₁"}, "A", 3),
                 Out::number("I2", {"Sekundärstrom I₂", "Secondary current I₂"}, "A", 3),
                 Out::number("w", {"Windungen je Volt", "Turns per volt"}, "1/V", 3),
                 Out::number("N1", {"Windungen primär N₁", "Primary turns N₁"}, "", 0),
                 Out::number("N2", {"Windungen sekundär N₂", "Secondary turns N₂"}, "", 0),
                 Out::number("Afe", {"Verwendeter Eisenquerschnitt", "Core cross-section used"}, "cm²", 1)};
    c.keywords = {"transformator", "wicklung", "windungen", "trafo", "transformer", "turns"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double S = in.num("S"), U1 = in.num("U1"), U2 = in.num("U2");
        const double A = in.maybe("A").value_or(std::sqrt(S));
        const double w = turnsPerVolt(parseDouble(in.choice("f")), in.num("B"), A * 1e-4);
        r.set("ratio", U1 / U2);
        r.set("I1", S / U1);
        r.set("I2", S / U2);
        r.set("w", w);
        r.set("N1", std::round(w * U1));
        r.set("N2", std::round(w * U2 * (1.0 + in.num("add") / 100.0)));
        r.set("Afe", A);
        if (!in.has("A")) r.info({"A_Fe nach Faustformel √S geschätzt.", "A_core estimated by the rule of thumb √S."});
        return r;
    };
    return c;
}

Calculator antennaLength() {
    Calculator c;
    c.id = "antenna_length";
    c.category = kCat;
    c.title = {"Antennenlängenberechnung", "Antenna length"};
    c.summary = {"Wellenlänge und Länge von Dipol, Monopol (λ/4) und 5/8 λ", "Wavelength and length of dipole, monopole (λ/4) and 5/8 λ"};
    c.description = {"λ = c / f (c = 299 792 458 m/s)    l = Bruchteil · λ · k\n"
                     "k = Verkürzungsfaktor (Drahtdipol ≈ 0,95, dicke Rohre kleiner); λ/2-Dipol: Gesamtlänge, jeder Arm l/2.",
                     "λ = c / f (c = 299 792 458 m/s)    l = fraction · λ · k\n"
                     "k = shortening factor (wire dipole ≈ 0.95, thick tubes smaller); λ/2 dipole: total length, each arm l/2."};
    c.inputs = {Num::number("f", {"Frequenz f", "Frequency f"}, "MHz", 433).positive(),
                Num::choice("type", {"Antennenform", "Antenna type"},
                            {{"1", {"λ (Ganzwelle)", "λ (full wave)"}}, {"0.5", {"λ/2-Dipol", "λ/2 dipole"}},
                             {"0.25", {"λ/4-Monopol (Groundplane)", "λ/4 monopole (ground plane)"}}, {"0.625", {"5/8 λ", "5/8 λ"}}},
                            "0.25"),
                Num::number("k", {"Verkürzungsfaktor k", "Shortening factor k"}, "", 0.95).rangeExclusiveMin(0, 1)};
    c.outputs = {Out::number("l", {"Antennenlänge", "Antenna length"}, "cm", 1).primary(),
                 Out::number("lambda", {"Wellenlänge λ", "Wavelength λ"}, "m", 3),
                 Out::number("l0", {"Länge ohne Verkürzung", "Length without shortening"}, "cm", 1)};
    c.keywords = {"antenne", "dipol", "wellenlänge", "antenna", "wavelength"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double lambda = wavelength(in.num("f") * 1e6);
        const double frac = parseDouble(in.choice("type"));
        r.set("lambda", lambda);
        r.set("l0", lambda * frac * 100.0);
        r.set("l", lambda * frac * in.num("k") * 100.0);
        return r;
    };
    return c;
}

Calculator cctvStorage() {
    Calculator c;
    c.id = "cctv_storage";
    c.category = kCat;
    c.title = {"CCTV Speicherplatz- und Bandbreitenberechnung", "CCTV storage and bandwidth"};
    c.summary = {"Bitrate, Netzwerklast und Speicher für Videoüberwachung", "Bitrate, network load and storage for video surveillance"};
    c.description = {"Bitrate ≈ Pixel · Bilder/s · Bits je Pixel (Richtwerte: H.264 0,08, H.265 0,04, MJPEG 0,75 bit/Pixel; "
                     "stark szenenabhängig – Herstellerwert eintragen, wenn bekannt).\n"
                     "Bandbreite = n · Bitrate    Speicher = n · Bitrate · Aufzeichnungsdauer · Anteil / 8 (1 TB = 10¹² Byte).",
                     "Bitrate ≈ pixels · frames/s · bits per pixel (guide values: H.264 0.08, H.265 0.04, MJPEG 0.75 bit/pixel; "
                     "strongly scene-dependent – enter the manufacturer value if known).\n"
                     "Bandwidth = n · bitrate    Storage = n · bitrate · recording time · share / 8 (1 TB = 10¹² bytes)."};
    c.inputs = {Num::number("n", {"Anzahl Kameras", "Number of cameras"}, "", 8).range(1, 10000),
                Num::choice("res", {"Auflösung", "Resolution"},
                            {{"0.92", {"720p (1280 × 720)", "720p (1280 × 720)"}}, {"2.07", {"1080p (1920 × 1080)", "1080p (1920 × 1080)"}},
                             {"3.69", {"4 MP (2560 × 1440)", "4 MP (2560 × 1440)"}}, {"5.04", {"5 MP (2592 × 1944)", "5 MP (2592 × 1944)"}},
                             {"8.29", {"4K / 8 MP (3840 × 2160)", "4K / 8 MP (3840 × 2160)"}}},
                            "2.07"),
                Num::number("fps", {"Bilder pro Sekunde", "Frames per second"}, "1/s", 15).range(1, 60),
                Num::choice("codec", {"Videokompression", "Video codec"},
                            {{"h264", "H.264"}, {"h265", "H.265"}, {"mjpeg", "MJPEG"}}, "h265"),
                Num::number("br", {"Bitrate je Kamera (Herstellerwert, optional)", "Bitrate per camera (manufacturer, optional)"}, "Mbit/s", kNaN)
                    .positive()
                    .opt(),
                Num::number("hours", {"Aufzeichnung pro Tag", "Recording per day"}, "h", 24).range(0, 24),
                Num::number("share", {"Anteil mit Aufzeichnung (Bewegung)", "Share recorded (motion)"}, "%", 100).range(0, 100),
                Num::number("days", {"Speicherdauer", "Retention"}, "d", 30).range(1, 3650)};
    c.outputs = {Out::number("TB", {"Speicherbedarf", "Storage required"}, "TB", 2).primary(),
                 Out::number("rate", {"Bitrate je Kamera", "Bitrate per camera"}, "Mbit/s", 2),
                 Out::number("bw", {"Netzwerklast gesamt", "Total network load"}, "Mbit/s", 1),
                 Out::number("GBday", {"Speicher pro Tag", "Storage per day"}, "GB", 1)};
    c.keywords = {"cctv", "video", "kamera", "speicher", "bandbreite", "camera", "storage"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double px = parseDouble(in.choice("res")) * 1e6;
        const double est = px * in.num("fps") * cctvBitsPerPixel(in.choice("codec")) / 1e6;  // Mbit/s
        const double rate = in.maybe("br").value_or(est);
        const double n = std::round(in.num("n"));
        const double bytesDay = n * rate * 1e6 * in.num("hours") * 3600.0 * in.num("share") / 100.0 / 8.0;
        r.set("rate", rate);
        r.set("bw", n * rate);
        r.set("GBday", bytesDay / 1e9);
        r.set("TB", bytesDay * in.num("days") / 1e12);
        if (!in.has("br")) r.info({"Bitrate geschätzt (Richtwert) – Herstellerangabe ist genauer.", "Bitrate estimated (guide value) – the manufacturer value is more accurate."});
        if (n * rate > 600) r.warn({"Netzwerklast > 600 Mbit/s – 1-Gbit-Uplink wird knapp (max. ca. 70 % auslasten).",
                                    "Network load > 600 Mbit/s – a 1 Gbit uplink gets tight (use at most approx. 70 %)."});
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ temperature sensors

Calculator rtdCopper() {
    Calculator c;
    c.id = "rtd_cu";
    c.category = kCat;
    c.title = {"Kupfer-Widerstandsthermometer (Cu10/Cu50/Cu100)", "Copper RTD (Cu10/Cu50/Cu100)"};
    c.summary = {"R = R₀ · (1 + α · θ), linear", "R = R₀ · (1 + α · θ), linear"};
    c.description = {"Kupfer-Messwiderstände sind im Bereich −50 … +150 °C nahezu linear: R = R₀ · (1 + α · θ).\n"
                     "α = 0,00427 1/K (Cu10 nach US-Praxis), 0,00428 1/K (GOST 6651). Pt- und Ni-Fühler: eigener Rechner.",
                     "Copper RTDs are nearly linear from −50 to +150 °C: R = R₀ · (1 + α · θ).\n"
                     "α = 0.00427 1/K (Cu10, US practice), 0.00428 1/K (GOST 6651). Pt and Ni sensors: separate calculator."};
    c.inputs = {Num::choice("R0", {"Fühler", "Sensor"}, {{"10", "Cu10"}, {"50", "Cu50"}, {"100", "Cu100"}}, "100"),
                Num::choice("alpha", {"Temperaturkoeffizient α", "Temperature coefficient α"},
                            {{"0.00427", "0,00427 1/K"}, {"0.00428", "0,00428 1/K"}}, "0.00428"),
                Num::choice("mode", {"Gesucht", "Find"}, {{"R", {"Widerstand aus Temperatur", "Resistance from temperature"}},
                                                          {"T", {"Temperatur aus Widerstand", "Temperature from resistance"}}}, "R"),
                Num::number("T", {"Temperatur θ", "Temperature θ"}, "°C", 50).range(-50, 150),
                Num::number("R", {"Widerstand R", "Resistance R"}, "Ω", 121.4).positive()};
    c.inputs[3].showIf("mode", {"R"});
    c.inputs[4].showIf("mode", {"T"});
    c.outputs = {Out::number("Rout", {"Widerstand R", "Resistance R"}, "Ω", 3).primary(),
                 Out::number("Tout", {"Temperatur θ", "Temperature θ"}, "°C", 2).primary(),
                 Out::number("sens", {"Empfindlichkeit", "Sensitivity"}, "Ω/K", 4)};
    c.keywords = {"cu100", "cu50", "kupfer", "rtd", "temperaturfühler", "copper"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double R0 = parseDouble(in.choice("R0"));
        const double a = parseDouble(in.choice("alpha"));
        r.set("sens", R0 * a);
        if (in.choice("mode") == "R") {
            r.set("Rout", R0 * (1.0 + a * in.num("T")));
        } else {
            const double T = (in.num("R") / R0 - 1.0) / a;
            if (T < -50 || T > 150) r.warn({"Außerhalb −50 … 150 °C (linearer Bereich).", "Outside −50 … 150 °C (linear range)."});
            r.set("Tout", T);
        }
        return r;
    };
    return c;
}

Calculator thermocoupleCalc() {
    Calculator c;
    c.id = "thermocouple";
    c.category = kCat;
    c.title = {"Thermoelemente (Typ K, J, T, N, E, R, S, B)", "Thermocouples (type K, J, T, N, E, R, S, B)"};
    c.summary = {"Thermospannung ↔ Temperatur mit Vergleichsstelle", "Thermal EMF ↔ temperature with reference junction"};
    c.description = {"Grundwerte nach IEC 60584-1 (= NIST ITS-90-Referenzfunktionen, Vergleichsstelle 0 °C).\n"
                     "Gemessene Spannung U = E(θ_M) − E(θ_V) mit θ_V = Temperatur der Vergleichsstelle (Klemmen). "
                     "Rückrechnung: E(θ_M) = U + E(θ_V).\nFarben der Ausgleichsleitungen nach IEC 60584-3: K grün, J schwarz, T braun, "
                     "N rosa, E violett, R/S orange, B grau (Pluspol farbig, Minus weiß).",
                     "Reference values per IEC 60584-1 (= NIST ITS-90 reference functions, reference junction 0 °C).\n"
                     "Measured voltage U = E(θ_M) − E(θ_R) with θ_R = reference (terminal) temperature. "
                     "Inverse: E(θ_M) = U + E(θ_R).\nExtension cable colours per IEC 60584-3: K green, J black, T brown, "
                     "N pink, E violet, R/S orange, B grey (plus conductor coloured, minus white)."};
    c.inputs = {Num::choice("type", {"Typ", "Type"},
                            {{"K", "K (NiCr-Ni)"}, {"J", "J (Fe-CuNi)"}, {"T", "T (Cu-CuNi)"}, {"N", "N (NiCrSi-NiSi)"},
                             {"E", "E (NiCr-CuNi)"}, {"R", "R (Pt13Rh-Pt)"}, {"S", "S (Pt10Rh-Pt)"}, {"B", "B (Pt30Rh-Pt6Rh)"}},
                            "K"),
                Num::choice("mode", {"Gesucht", "Find"}, {{"U", {"Spannung aus Temperatur", "Voltage from temperature"}},
                                                          {"T", {"Temperatur aus Spannung", "Temperature from voltage"}}}, "U"),
                Num::number("T", {"Messtemperatur θ_M", "Measuring temperature θ_M"}, "°C", 400),
                Num::number("U", {"Gemessene Spannung U", "Measured voltage U"}, "mV", 15.6),
                Num::number("Tref", {"Vergleichsstellentemperatur θ_V", "Reference junction temperature θ_R"}, "°C", 20)};
    c.inputs[2].showIf("mode", {"U"});
    c.inputs[3].showIf("mode", {"T"});
    c.outputs = {Out::number("Uout", {"Thermospannung U", "Thermal EMF U"}, "mV", 3).primary(),
                 Out::number("Tout", {"Messtemperatur θ_M", "Measuring temperature θ_M"}, "°C", 1).primary(),
                 Out::number("E0", {"E(θ_M) bezogen auf 0 °C", "E(θ_M) referred to 0 °C"}, "mV", 3),
                 Out::number("Eref", {"E(θ_V) der Vergleichsstelle", "E(θ_R) of the reference junction"}, "mV", 3),
                 Out::number("sens", {"Empfindlichkeit bei θ_M", "Sensitivity at θ_M"}, "µV/K", 1)};
    c.keywords = {"thermoelement", "thermocouple", "typ k", "mv", "vergleichsstelle"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto t = thermocouple::parse(in.choice("type"));
        const auto [lo, hi] = thermocouple::range(t);
        const auto eref = thermocouple::emf(t, in.num("Tref"));
        if (!eref) throw CalcError({"Vergleichsstellentemperatur außerhalb des Bereichs.", "Reference temperature out of range."});
        r.set("Eref", *eref);
        double T = 0;
        if (in.choice("mode") == "U") {
            T = in.num("T");
            const auto e = thermocouple::emf(t, T);
            if (!e)
                throw CalcError(cat(Text{"Außerhalb des Bereichs ", "Out of range "}, numText(lo, 0), " … ", numText(hi, 0, "°C")));
            r.set("E0", *e);
            r.set("Uout", *e - *eref);
        } else {
            const double e0 = in.num("U") + *eref;
            const auto tt = thermocouple::temperature(t, e0);
            if (!tt) throw CalcError({"Spannung außerhalb des Bereichs dieses Typs.", "Voltage out of range for this type."});
            T = *tt;
            r.set("E0", e0);
            r.set("Tout", T);
        }
        const double h = 0.5;
        const auto ea = thermocouple::emf(t, std::max(lo, T - h));
        const auto eb = thermocouple::emf(t, std::min(hi, T + h));
        if (ea && eb) r.set("sens", (*eb - *ea) / (std::min(hi, T + h) - std::max(lo, T - h)) * 1e3);
        return r;
    };
    c.chart = [](const Inputs& in, const Result& res, const Context&) {
        Chart ch;
        const auto t = thermocouple::parse(in.choice("type"));
        const auto [lo, hi] = thermocouple::range(t);
        const double x0 = std::max(lo, -200.0);
        ch.x = {{"Temperatur θ in °C", "Temperature θ in °C"}, x0, hi, hi - x0 > 1000 ? 200.0 : 100.0};
        double ymin = 0, ymax = 0;
        ChartSeries s{"process", {}};
        for (int i = 0; i <= 120; ++i) {
            const double T = x0 + (hi - x0) * i / 120.0;
            const double e = thermocouple::emf(t, T).value_or(0.0);
            s.pts.push_back({T, e});
            ymin = std::min(ymin, e);
            ymax = std::max(ymax, e);
        }
        const double step = ymax > 50 ? 10.0 : ymax > 20 ? 5.0 : 2.0;
        ch.y = {{"Thermospannung E in mV", "Thermal EMF E in mV"}, std::floor(ymin / step) * step, std::ceil(ymax / step) * step, step};
        ch.series.push_back(s);
        const auto Tm = in.choice("mode") == "U" ? std::optional<double>(in.num("T")) : (res.values.contains("Tout") ? std::optional<double>(res.values.at("Tout")) : std::nullopt);
        if (Tm) {
            if (const auto e = thermocouple::emf(t, *Tm)) ch.markers.push_back({*Tm, *e, "θ", "state"});
        }
        ch.note = {"Kennlinie E(θ) bei Vergleichsstelle 0 °C.", "Characteristic E(θ) with the reference junction at 0 °C."};
        return ch;
    };
    return c;
}

}  // namespace

void addElectricalComponents(std::vector<Calculator>& out) {
    out.push_back(colourFromValue());
    out.push_back(smdCode());
    out.push_back(capacitorCodeCalc());
    out.push_back(inductorCodeCalc());
    out.push_back(currentDivider());
    out.push_back(zenerCalc());
    out.push_back(seriesResistor());
    out.push_back(transformerWindings());
    out.push_back(antennaLength());
    out.push_back(cctvStorage());
    out.push_back(rtdCopper());
    out.push_back(thermocoupleCalc());
}

}  // namespace elc::catalog
