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
                Num::number("R3", {"R₃", "R₃"}, "Ω", 470).positive().opt()};
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
                Num::number("Pz", {"Belastbarkeit der Z-Diode", "Zener power rating"}, "W", 0.5).positive().opt()};
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

// v1.9: one calculator for LED series resistors and dropping resistors (merges "LED-Vorwiderstand")
Calculator seriesResistor() {
    Calculator c;
    c.id = "series_resistor";
    c.category = kCat;
    c.title = {"Vorwiderstand (LED und Verbraucher)", "Series resistor (LED and load)"};
    c.summary = {"R = (U − n · U_F) / I_F bzw. (U − U_L) / I_L mit Normwert und Leistung",
                 "R = (U − n · U_F) / I_F or (U − U_L) / I_L with standard value and power"};
    c.description = {"LED: R = (U − n · U_F) / I_F, gewählt wird der nächstgrößere E24-Wert (Strom bleibt ≤ I_F). "
                     "Typ. U_F: rot 1,8–2,1 V, grün 2,0–3,2 V, blau/weiß 2,8–3,4 V.\n"
                     "Verbraucher: R_v = (U − U_L) / I_L, nächster E12-Wert; U_L mit diesem Wert bei ohmscher Last. Nur für "
                     "konstante Last geeignet – bei schwankendem Strom Spannungsregler verwenden.\n"
                     "P_R = I² · R, Belastbarkeit ≥ 2 · P_R wählen. Wirkungsgrad η = U_Last / U.",
                     "LED: R = (U − n · U_F) / I_F, the next higher E24 value is selected (current stays ≤ I_F). "
                     "Typ. U_F: red 1.8–2.1 V, green 2.0–3.2 V, blue/white 2.8–3.4 V.\n"
                     "Load: R_v = (U − U_L) / I_L, nearest E12 value; U_L with this value for a resistive load. Only for a "
                     "constant load – with varying current use a voltage regulator.\n"
                     "P_R = I² · R, choose a rating ≥ 2 · P_R. Efficiency η = U_load / U."};
    c.inputs = {Num::choice("load", {"Last", "Load"},
                            {{"led", {"LED (eine oder mehrere in Reihe)", "LED (one or several in series)"}},
                             {"dev", {"Verbraucher mit U_L und I_L", "Load with U_L and I_L"}}},
                            "led"),
                Num::number("U", {"Versorgungsspannung U", "Supply voltage U"}, "V", 24).positive(),
                Num::number("Uf", {"Durchlassspannung U_F", "Forward voltage U_F"}, "V", 2.1).positive(),
                Num::number("If", {"LED-Strom I_F", "LED current I_F"}, "mA", 10).positive(),
                Num::number("n", {"LEDs in Reihe", "LEDs in series"}, "", 1).range(1, 100),
                Num::number("UL", {"Spannung am Verbraucher U_L", "Voltage at the load U_L"}, "V", 12).positive(),
                Num::number("I", {"Strom des Verbrauchers I_L", "Load current I_L"}, "mA", 100).positive()};
    for (const int i : {2, 3, 4}) c.inputs[static_cast<std::size_t>(i)].showIf("load", {"led"});
    for (const int i : {5, 6}) c.inputs[static_cast<std::size_t>(i)].showIf("load", {"dev"});
    c.outputs = {Out::number("R", {"Berechneter Widerstand", "Calculated resistance"}, "Ω", 1),
                 Out::number("Rn", {"Gewählter Normwert", "Selected standard value"}, "Ω", 1).primary(),
                 Out::number("In", {"Strom mit Normwert", "Current with standard value"}, "mA", 2),
                 Out::number("ULn", {"Spannung an der Last mit Normwert", "Load voltage with standard value"}, "V", 2),
                 Out::number("P", {"Verlustleistung P_R", "Power P_R"}, "W", 3),
                 Out::number("Prated", {"Empfohlene Belastbarkeit ≥", "Recommended rating ≥"}, "W", 3),
                 Out::number("eta", {"Wirkungsgrad", "Efficiency"}, "%", 1)};
    c.keywords = {"vorwiderstand", "led", "spannung reduzieren", "series resistor", "dropping resistor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double U = in.num("U");
        if (in.choice("load") == "led") {
            const double Uload = std::round(in.num("n")) * in.num("Uf");
            const double Ur = U - Uload;
            if (Ur <= 0) throw CalcError({"Versorgungsspannung zu klein für die LED-Reihe.", "Supply voltage too low for the LED string."});
            const double R = Ur / (in.num("If") / 1e3);
            const double Rn = electronics::eSeriesMatch(electronics::ESeries::E24, R).upper;
            const double In = Ur / Rn;
            r.set("R", R);
            r.set("Rn", Rn);
            r.set("In", In * 1e3);
            r.set("P", In * In * Rn);
            r.set("Prated", 2.0 * In * In * Rn);
            r.set("eta", Uload / U * 100.0);
            return r;
        }
        const double UL = in.num("UL"), I = in.num("I") / 1e3;
        if (UL >= U) throw CalcError({"U_L muss kleiner als U sein.", "U_L must be smaller than U."});
        const double R = (U - UL) / I;
        const double Rn = nearestLog(electronics::eSeriesValues(electronics::ESeries::E12), R);
        const double Rload = UL / I;
        const double In = U / (Rload + Rn);
        r.set("R", R);
        r.set("Rn", Rn);
        r.set("In", In * 1e3);
        r.set("ULn", In * Rload);
        r.set("P", In * In * Rn);
        r.set("Prated", 2.0 * In * In * Rn);
        r.set("eta", UL / U * 100.0);
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
                Num::number("A", {"Eisenquerschnitt A_Fe", "Core cross-section A_core"}, "cm²", 10).positive().opt(),
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
                Num::number("br", {"Bitrate je Kamera (Herstellerwert)", "Bitrate per camera (manufacturer)"}, "Mbit/s", kNaN)
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
    out.push_back(smdCode());
    out.push_back(capacitorCodeCalc());
    out.push_back(inductorCodeCalc());
    out.push_back(currentDivider());
    out.push_back(zenerCalc());
    out.push_back(seriesResistor());
    out.push_back(transformerWindings());
    out.push_back(antennaLength());
    out.push_back(cctvStorage());
    out.push_back(thermocoupleCalc());
}

}  // namespace elc::catalog
