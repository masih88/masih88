// Calculators: Elektronik / Electronics
#include <algorithm>
#include <array>
#include <string>
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"
#include "elc/formulas/electrical.hpp"
#include "elc/formulas/basics.hpp"
#include "elc/formulas/electronics.hpp"

namespace elc::catalog {

using namespace elc::electronics;

namespace {

struct Colour {
    const char* id;
    const char* de;
    const char* en;
    const char* hex;
};
constexpr std::array<Colour, 12> kColours{{
    {"black", "Schwarz", "Black", "#1A1A1A"},
    {"brown", "Braun", "Brown", "#8B4513"},
    {"red", "Rot", "Red", "#E53935"},
    {"orange", "Orange", "Orange", "#FB8C00"},
    {"yellow", "Gelb", "Yellow", "#FDD835"},
    {"green", "Grün", "Green", "#43A047"},
    {"blue", "Blau", "Blue", "#1E88E5"},
    {"violet", "Violett", "Violet", "#8E24AA"},
    {"grey", "Grau", "Grey", "#9E9E9E"},
    {"white", "Weiß", "White", "#FFFFFF"},
    {"gold", "Gold", "Gold", "#C9A227"},
    {"silver", "Silber", "Silver", "#C0C0C0"},
}};

/// index in kColours (−1 = none)
int colourIndex(const std::string& id) {
    for (std::size_t i = 0; i < kColours.size(); ++i) {
        if (id == kColours[i].id) return static_cast<int>(i);
    }
    return -1;
}

std::vector<Option> digitOptions() {
    std::vector<Option> o;
    for (std::size_t i = 0; i < 10; ++i) {
        const auto& c = kColours[i];
        o.push_back({c.id, {std::to_string(i) + " – " + c.de, std::to_string(i) + " – " + c.en}});
    }
    return o;
}

int digitOf(const std::string& id) {
    for (std::size_t i = 0; i < 10; ++i) {
        if (id == kColours[i].id) return static_cast<int>(i);
    }
    return 0;
}

std::vector<Option> multiplierOptions() {
    static const std::array<const char*, 12> factor{"×1", "×10", "×100", "×1k", "×10k", "×100k",
                                                    "×1M", "×10M", "×100M", "×1G", "×0,1", "×0,01"};
    static const std::array<const char*, 12> factorEn{"×1", "×10", "×100", "×1k", "×10k", "×100k",
                                                      "×1M", "×10M", "×100M", "×1G", "×0.1", "×0.01"};
    std::vector<Option> o;
    for (std::size_t i = 0; i < kColours.size(); ++i) {
        const auto& c = kColours[i];
        o.push_back({c.id, {std::string(factor[i]) + " – " + c.de, std::string(factorEn[i]) + " – " + c.en}});
    }
    return o;
}

double multiplierOf(const std::string& id) {
    if (id == "gold") return 0.1;
    if (id == "silver") return 0.01;
    return std::pow(10.0, digitOf(id));
}

std::vector<Option> toleranceOptions() {
    return {{"brown", {"±1 % – Braun", "±1 % – Brown"}},     {"red", {"±2 % – Rot", "±2 % – Red"}},
            {"green", {"±0,5 % – Grün", "±0.5 % – Green"}},  {"blue", {"±0,25 % – Blau", "±0.25 % – Blue"}},
            {"violet", {"±0,1 % – Violett", "±0.1 % – Violet"}}, {"grey", {"±0,05 % – Grau", "±0.05 % – Grey"}},
            {"gold", {"±5 % – Gold", "±5 % – Gold"}},         {"silver", {"±10 % – Silber", "±10 % – Silver"}},
            {"none", {"±20 % – kein Ring", "±20 % – no band"}}};
}

double toleranceOf(const std::string& id) {
    if (id == "brown") return 1;
    if (id == "red") return 2;
    if (id == "green") return 0.5;
    if (id == "blue") return 0.25;
    if (id == "violet") return 0.1;
    if (id == "grey") return 0.05;
    if (id == "silver") return 10;
    if (id == "none") return 20;
    return 5;
}

/// Picks a readable prefix for Ω or µH values
std::pair<double, std::string> withPrefix(double v, bool henry) {
    if (henry) {  // v in µH
        if (v >= 1e6) return {v / 1e6, "H"};
        if (v >= 1e3) return {v / 1e3, "mH"};
        return {v, "µH"};
    }
    if (v >= 1e9) return {v / 1e9, "GΩ"};
    if (v >= 1e6) return {v / 1e6, "MΩ"};
    if (v >= 1e3) return {v / 1e3, "kΩ"};
    return {v, "Ω"};
}

/// colour ids of the bands (digits, multiplier, tolerance; tolerance "none" is left out)
std::vector<std::string> bandIds(const Inputs& in) {
    std::vector<std::string> ids;
    if (in.choice("dir") == "v2c") {
        const double mult = in.choice("unit") == "M" ? 1e6 : in.choice("unit") == "k" ? 1e3 : 1.0;
        const auto b = electrical::bandsFor(in.num("R") * mult, in.choice("bands") == "5" ? 3 : 2);
        if (!b) return {};
        for (const int d : b->digits) ids.emplace_back(kColours[static_cast<std::size_t>(d)].id);
        ids.emplace_back(b->exponent >= 0 ? kColours[static_cast<std::size_t>(b->exponent)].id : b->exponent == -1 ? "gold" : "silver");
    } else {
        ids = {in.choice("b1"), in.choice("b2")};
        if (in.choice("bands") == "5") ids.push_back(in.choice("b3"));
        ids.push_back(in.choice("mult"));
    }
    if (in.choice("tol") != "none") ids.push_back(in.choice("tol"));
    return ids;
}

Calculator colourCode() {
    Calculator c;
    c.id = "resistor_color";
    c.category = "electronics";
    c.title = {"Farbcode Widerstand / Spule", "Colour code resistor / inductor"};
    c.summary = {"Farbringe → Wert und Wert → Farbringe (4 und 5 Ringe, IEC 60062) mit Zeichnung",
                 "Colour bands → value and value → colour bands (4 and 5 bands, IEC 60062) with drawing"};
    c.description = {
        "4 Ringe: Ziffer, Ziffer, Multiplikator, Toleranz (E6–E24)\n5 Ringe: Ziffer, Ziffer, Ziffer, Multiplikator, Toleranz (E48–E192)\n"
        "Schwarz 0 ×1 · Braun 1 ×10 ±1 % · Rot 2 ×100 ±2 % · Orange 3 ×1 k · Gelb 4 ×10 k · Grün 5 ×100 k ±0,5 % · "
        "Blau 6 ×1 M ±0,25 % · Violett 7 ×10 M ±0,1 % · Grau 8 ×100 M ±0,05 % · Weiß 9 ×1 G · Gold ×0,1 ±5 % · "
        "Silber ×0,01 ±10 % · kein Ring ±20 %\n"
        "Wert → Farbringe: auf 2 bzw. 3 gültige Ziffern gerundet und gegen E24 bzw. E96 geprüft. "
        "Induktivitäten: gleiche Codierung, Grundeinheit µH.",
        "4 bands: digit, digit, multiplier, tolerance (E6–E24)\n5 bands: digit, digit, digit, multiplier, tolerance (E48–E192)\n"
        "Black 0 ×1 · brown 1 ×10 ±1 % · red 2 ×100 ±2 % · orange 3 ×1 k · yellow 4 ×10 k · green 5 ×100 k ±0.5 % · "
        "blue 6 ×1 M ±0.25 % · violet 7 ×10 M ±0.1 % · grey 8 ×100 M ±0.05 % · white 9 ×1 G · gold ×0.1 ±5 % · "
        "silver ×0.01 ±10 % · no band ±20 %\n"
        "Value → colour bands: rounded to 2 or 3 significant digits and checked against E24 or E96. "
        "Inductors: same code, base unit µH."};
    c.inputs = {
        Num::choice("dir", {"Richtung", "Direction"},
                    {{"c2v", {"Farbringe → Wert", "Colour bands → value"}}, {"v2c", {"Wert → Farbringe", "Value → colour bands"}}}, "c2v"),
        Num::choice("part", {"Bauteil", "Component"},
                    {{"r", {"Widerstand (Ω)", "Resistor (Ω)"}}, {"l", {"Induktivität (µH)", "Inductor (µH)"}}}, "r"),
        Num::number("R", {"Widerstandswert", "Resistance"}, "", 4.7).positive(),
        Num::choice("unit", {"Einheit", "Unit"}, {{"ohm", "Ω"}, {"k", "kΩ"}, {"M", "MΩ"}}, "k"),
        Num::choice("bands", {"Anzahl Ringe", "Number of bands"}, {{"4", {"4 Ringe", "4 bands"}}, {"5", {"5 Ringe", "5 bands"}}}, "4"),
        Num::choice("b1", {"1. Ring (Ziffer)", "1st band (digit)"}, digitOptions(), "yellow"),
        Num::choice("b2", {"2. Ring (Ziffer)", "2nd band (digit)"}, digitOptions(), "violet"),
        Num::choice("b3", {"3. Ring (Ziffer)", "3rd band (digit)"}, digitOptions(), "black"),
        Num::choice("mult", {"Multiplikator", "Multiplier"}, multiplierOptions(), "red"),
        Num::choice("tol", {"Toleranz", "Tolerance"}, toleranceOptions(), "gold"),
    };
    c.inputs[1].showIf("dir", {"c2v"});
    c.inputs[2].showIf("dir", {"v2c"});
    c.inputs[3].showIf("dir", {"v2c"});
    c.inputs[5].showIf("dir", {"c2v"});
    c.inputs[6].showIf("dir", {"c2v"});
    c.inputs[7].showIf("bands", {"5"});
    c.inputs[8].showIf("dir", {"c2v"});
    c.outputs = {Out::number("value", {"Nennwert", "Nominal value"}, "Ω", 3).primary(),
                 Out::text("colours", {"Farbringe", "Colour bands"}).primary(),
                 Out::number("tol", {"Toleranz", "Tolerance"}, "%", 2),
                 Out::number("min", {"Minimalwert", "Minimum"}, "Ω", 3),
                 Out::number("max", {"Maximalwert", "Maximum"}, "Ω", 3),
                 Out::number("nearest", {"Nächster Normwert", "Nearest standard value"}, "Ω", 3),
                 Out::text("series", {"Normreihe", "Standard series"})};
    c.keywords = {"farbcode", "farbring", "farbringe", "widerstand", "colour code", "color code", "resistor", "bands"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const bool five = in.choice("bands") == "5";
        const double tol = toleranceOf(in.choice("tol"));
        const auto ids = bandIds(in);
        if (in.choice("dir") == "v2c") {
            const double mult = in.choice("unit") == "M" ? 1e6 : in.choice("unit") == "k" ? 1e3 : 1.0;
            const double R = in.num("R") * mult;
            const auto b = electrical::bandsFor(R, five ? 3 : 2);
            if (!b) throw CalcError({"Wert mit Farbringen nicht darstellbar (0,1 Ω … 99 GΩ).", "Value cannot be shown with colour bands (0.1 Ω … 99 GΩ)."});
            const auto [v, unit] = withPrefix(b->value, false);
            r.set("value", v, unit);
            const auto match = eSeriesMatch(five ? ESeries::E96 : ESeries::E24, b->value);
            const auto [nv, nunit] = withPrefix(match.nearest, false);
            r.set("nearest", nv, nunit);
            const bool exact = std::fabs(match.nearest - b->value) <= 1e-6 * b->value;
            r.setText("series", exact ? Text{five ? "E96 ✓" : "E24 ✓", five ? "E96 ✓" : "E24 ✓"}
                                      : Text{five ? "kein E96-Wert" : "kein E24-Wert", five ? "not an E96 value" : "not an E24 value"});
            if (std::fabs(b->value - R) > 1e-9 * R)
                r.warn({"Wert auf die darstellbaren Ziffern gerundet.", "Value rounded to the digits the bands can show."});
            r.set("tol", tol);
            r.set("min", v * (1 - tol / 100), unit);
            r.set("max", v * (1 + tol / 100), unit);
        } else {
            const bool henry = in.choice("part") == "l";
            double digits = digitOf(in.choice("b1")) * 10 + digitOf(in.choice("b2"));
            if (five) digits = digits * 10 + digitOf(in.choice("b3"));
            const double v = digits * multiplierOf(in.choice("mult"));
            const auto [val, unit] = withPrefix(v, henry);
            r.set("value", val, unit);
            r.set("tol", tol);
            r.set("min", val * (1 - tol / 100), unit);
            r.set("max", val * (1 + tol / 100), unit);
        }
        Text names{"", ""};
        for (std::size_t i = 0; i < ids.size(); ++i) {
            const auto& col = kColours[static_cast<std::size_t>(std::max(0, colourIndex(ids[i])))];
            names.de += (i ? " – " : "") + std::string(col.de);
            names.en += (i ? " – " : "") + std::string(col.en);
        }
        if (in.choice("tol") == "none") {
            names.de += " (kein Toleranzring)";
            names.en += " (no tolerance band)";
        }
        r.setText("colours", names);
        return r;
    };
    c.chart = [](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 40, 0.4);
        const bool five = in.choice("bands") == "5";
        s.line("ink", {{2, 20}, {22, 20}});
        s.line("ink", {{78, 20}, {98, 20}});
        s.rect(five ? "fill:#A9CCE3" : "fill:#E8D5A9", 22, 9, 78, 31);
        const auto ids = bandIds(in);
        if (ids.empty()) return s.ch;
        const std::vector<double> xs = five ? std::vector<double>{28, 35, 42, 49, 68} : std::vector<double>{30, 38, 46, 68};
        const std::vector<const char*> tags = five ? std::vector<const char*>{"1", "2", "3", "×", "±"}
                                                   : std::vector<const char*>{"1", "2", "×", "±"};
        for (std::size_t i = 0; i < ids.size() && i < xs.size(); ++i) {
            const int k = colourIndex(ids[i]);
            if (k < 0) continue;
            s.rect(std::string("fill:") + kColours[static_cast<std::size_t>(k)].hex, xs[i] - 2.2, 9, xs[i] + 2.2, 31);
            s.label(xs[i], 4, tags[i]);
        }
        s.ch.note = {"Ringe von links lesen; der Toleranzring hat den größeren Abstand.",
                     "Read the bands from the left; the tolerance band has the larger gap."};
        return s.ch;
    };
    return c;
}

std::vector<Option> seriesOptions() {
    return {{"E6", "E6 (±20 %)"}, {"E12", "E12 (±10 %)"}, {"E24", "E24 (±5 %)"}, {"E48", "E48 (±2 %)"}, {"E96", "E96 (±1 %)"}};
}

ESeries parseSeries(const std::string& v) {
    if (v == "E6") return ESeries::E6;
    if (v == "E12") return ESeries::E12;
    if (v == "E48") return ESeries::E48;
    if (v == "E96") return ESeries::E96;
    return ESeries::E24;
}

Calculator eSeries() {
    Calculator c;
    c.id = "e_series";
    c.category = "electronics";
    c.title = {"E-Reihe: Normwert finden", "E series: find standard value"};
    c.summary = {"Nächster Wert in E6 … E96 (IEC 60063)", "Nearest value in E6 … E96 (IEC 60063)"};
    c.description = {"Sucht den nächstgelegenen Normwert (logarithmischer Abstand) sowie den nächstkleineren und -größeren Wert.",
                     "Finds the nearest standard value (logarithmic distance) plus the next lower and higher value."};
    c.inputs = {
        Num::number("v", {"Wert", "Value"}, "Ω", 5000).positive(),
        Num::choice("s", {"Reihe", "Series"}, seriesOptions(), "E24"),
    };
    c.outputs = {Out::number("nearest", {"Nächster Normwert", "Nearest standard value"}, "Ω", 3).primary(),
                 Out::number("dev", {"Abweichung", "Deviation"}, "%", 2),
                 Out::number("lower", {"Nächstkleiner", "Next lower"}, "Ω", 3),
                 Out::number("upper", {"Nächstgrößer", "Next higher"}, "Ω", 3)};
    c.keywords = {"e12", "e24", "e96", "normreihe", "normwert", "e series"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double v = in.num("v");
        const auto m = eSeriesMatch(parseSeries(in.choice("s")), v);
        r.set("nearest", m.nearest);
        r.set("dev", (m.nearest - v) / v * 100.0);
        r.set("lower", m.lower);
        r.set("upper", m.upper);
        return r;
    };
    return c;
}

Calculator voltageDivider() {
    Calculator c;
    c.id = "voltage_divider";
    c.category = "electronics";
    c.title = {"Spannungsteiler", "Voltage divider"};
    c.summary = {"U_aus unbelastet und belastet", "U_out unloaded and loaded"};
    c.description = {"U_aus = U_ein · R2 / (R1 + R2)\nBelastet: R2 ∥ R_L statt R2.",
                     "U_out = U_in · R2 / (R1 + R2)\nLoaded: R2 ∥ R_L instead of R2."};
    c.inputs = {
        Num::number("Uin", {"Eingangsspannung U_ein", "Input voltage U_in"}, "V", 24).nonNegative(),
        Num::number("R1", {"R1", "R1"}, "kΩ", 10).positive(),
        Num::number("R2", {"R2", "R2"}, "kΩ", 4.7).positive(),
        Num::number("RL", {"Lastwiderstand R_L", "Load R_L"}, "kΩ", kNaN).opt().positive(),
    };
    c.outputs = {Out::number("U0", {"U_aus unbelastet", "U_out unloaded"}, "V", 4).primary(),
                 Out::number("UL", {"U_aus belastet", "U_out loaded"}, "V", 4),
                 Out::number("I", {"Querstrom", "Divider current"}, "mA", 4),
                 Out::number("ratio", {"Teilerverhältnis", "Divider ratio"}, "", 4),
                 Out::number("P1", {"Verlustleistung R1", "Power R1"}, "mW", 3)};
    c.keywords = {"spannungsteiler", "teiler", "voltage divider"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        std::optional<double> RL;
        if (const auto v = in.maybe("RL")) RL = *v * 1e3;
        const auto d = basics::voltageDivider(in.num("Uin"), in.num("R1") * 1e3, in.num("R2") * 1e3, RL);
        r.set("U0", d.UoutUnloaded);
        if (RL) r.set("UL", d.UoutLoaded);
        r.set("I", d.I1 * 1e3);
        r.set("ratio", in.num("R2") / (in.num("R1") + in.num("R2")));
        r.set("P1", d.P1 * 1e3);
        return r;
    };
    return c;
}

Calculator rcCircuit() {
    Calculator c;
    c.id = "rc";
    c.category = "electronics";
    c.title = {"RC-Glied", "RC circuit"};
    c.summary = {"τ, Grenzfrequenz, Ladekurve, Energie", "τ, cut-off frequency, charging, energy"};
    c.description = {"τ = R · C    f_g = 1 / (2π · R · C)    u_C(t) = U_0 · (1 − e^(−t/τ))\nNach 5τ ≈ 99,3 % geladen.    E = ½ · C · U²    Q = C · U",
                     "τ = R · C    f_c = 1 / (2π · R · C)    u_C(t) = U_0 · (1 − e^(−t/τ))\nAfter 5τ ≈ 99.3 % charged.    E = ½ · C · U²    Q = C · U"};
    c.inputs = {
        Num::number("R", {"Widerstand R", "Resistance R"}, "kΩ", 10).positive(),
        Num::number("C", {"Kapazität C", "Capacitance C"}, "µF", 10).positive(),
        Num::number("U", {"Ladespannung U_0", "Charging voltage U_0"}, "V", 24).nonNegative(),
        Num::number("t", {"Zeit t", "Time t"}, "ms", kNaN).opt().nonNegative(),
    };
    c.outputs = {Out::number("tau", {"Zeitkonstante τ", "Time constant τ"}, "ms", 3).primary(),
                 Out::number("t5", {"5 τ", "5 τ"}, "ms", 2),
                 Out::number("fc", {"Grenzfrequenz f_g", "Cut-off frequency f_c"}, "Hz", 3),
                 Out::number("Ut", {"Spannung u_C(t)", "Voltage u_C(t)"}, "V", 3),
                 Out::number("E", {"Gespeicherte Energie", "Stored energy"}, "mJ", 3),
                 Out::number("Q", {"Ladung Q", "Charge Q"}, "µC", 2)};
    c.keywords = {"rc", "tiefpass", "zeitkonstante", "tau", "low pass", "filter"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double R = in.num("R") * 1e3;
        const double C = in.num("C") * 1e-6;
        const double U = in.num("U");
        const double tau = timeConstant(R, C);
        r.set("tau", tau * 1e3);
        r.set("t5", 5 * tau * 1e3);
        r.set("fc", cutoffFrequency(R, C));
        if (const auto t = in.maybe("t")) r.set("Ut", chargingVoltage(U, *t / 1e3, tau));
        r.set("E", 0.5 * C * U * U * 1e3);
        r.set("Q", C * U * 1e6);
        return r;
    };
    return c;
}

Calculator lcResonance() {
    Calculator c;
    c.id = "lc_resonance";
    c.category = "electronics";
    c.title = {"LC-Schwingkreis", "LC resonance"};
    c.summary = {"Resonanzfrequenz f₀ = 1 / (2π√(LC))", "Resonant frequency f₀ = 1 / (2π√(LC))"};
    c.description = {"f₀ = 1 / (2π · √(L · C))    Z₀ = √(L / C)", "f₀ = 1 / (2π · √(L · C))    Z₀ = √(L / C)"};
    c.inputs = {
        Num::number("L", {"Induktivität L", "Inductance L"}, "mH", 1).positive(),
        Num::number("C", {"Kapazität C", "Capacitance C"}, "µF", 1).positive(),
    };
    c.outputs = {Out::number("f0", {"Resonanzfrequenz f₀", "Resonant frequency f₀"}, "Hz", 2).primary(),
                 Out::number("w0", {"Kreisfrequenz ω₀", "Angular frequency ω₀"}, "1/s", 1),
                 Out::number("Z0", {"Kennwiderstand Z₀", "Characteristic impedance Z₀"}, "Ω", 3)};
    c.keywords = {"resonanz", "schwingkreis", "lc", "resonance"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double L = in.num("L") * 1e-3;
        const double C = in.num("C") * 1e-6;
        const double f0 = resonanceFrequency(L, C);
        r.set("f0", f0);
        r.set("w0", 2 * kPi * f0);
        r.set("Z0", std::sqrt(L / C));
        return r;
    };
    return c;
}

Calculator reactance() {
    Calculator c;
    c.id = "reactance";
    c.category = "electronics";
    c.title = {"Blindwiderstand & Impedanz (RLC Reihe)", "Reactance & impedance (series RLC)"};
    c.summary = {"X_L, X_C, Z und φ bei Frequenz f", "X_L, X_C, Z and φ at frequency f"};
    c.description = {"X_L = 2π · f · L    X_C = 1 / (2π · f · C)    Z = √(R² + (X_L − X_C)²)    φ = arctan((X_L − X_C)/R)\nLeere Felder = Bauteil nicht vorhanden.",
                     "X_L = 2π · f · L    X_C = 1 / (2π · f · C)    Z = √(R² + (X_L − X_C)²)    φ = arctan((X_L − X_C)/R)\nEmpty fields = component not present."};
    c.inputs = {
        Num::number("f", {"Frequenz f", "Frequency f"}, "Hz", 50).positive(),
        Num::number("R", {"Widerstand R", "Resistance R"}, "Ω", 10).opt().nonNegative(),
        Num::number("L", {"Induktivität L", "Inductance L"}, "mH", 100).opt().nonNegative(),
        Num::number("C", {"Kapazität C", "Capacitance C"}, "µF", kNaN).opt().positive(),
    };
    c.outputs = {Out::number("XL", {"Induktiver Blindwiderstand X_L", "Inductive reactance X_L"}, "Ω", 3),
                 Out::number("XC", {"Kapazitiver Blindwiderstand X_C", "Capacitive reactance X_C"}, "Ω", 3),
                 Out::number("Z", {"Impedanz Z", "Impedance Z"}, "Ω", 3).primary(),
                 Out::number("phi", {"Phasenwinkel φ", "Phase angle φ"}, "°", 2)};
    c.keywords = {"blindwiderstand", "impedanz", "reactance", "impedance", "xl", "xc"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double f = in.num("f");
        const double R = in.maybe("R").value_or(0.0);
        double X = 0;
        if (const auto L = in.maybe("L")) {
            const double XL = inductiveReactance(f, *L * 1e-3);
            r.set("XL", XL);
            X += XL;
        }
        if (const auto C = in.maybe("C")) {
            const double XC = capacitiveReactance(f, *C * 1e-6);
            r.set("XC", XC);
            X -= XC;
        }
        r.set("Z", std::hypot(R, X));
        r.set("phi", std::atan2(X, R) * 180.0 / kPi);
        return r;
    };
    return c;
}

Calculator batteryRuntime() {
    Calculator c;
    c.id = "battery_runtime";
    c.category = "electronics";
    c.title = {"Akku-/USV-Laufzeit", "Battery / UPS runtime"};
    c.summary = {"Überbrückungszeit aus Ah, V und Last", "Backup time from Ah, V and load"};
    c.description = {"E_nutz = U · C · DoD    t = E_nutz · η / P_Last\nVereinfachung ohne Peukert-Effekt und Alterung – Herstellerkurven beachten.",
                     "E_usable = U · C · DoD    t = E_usable · η / P_load\nSimplified, without Peukert effect and ageing – check manufacturer curves."};
    c.inputs = {
        Num::number("U", {"Batteriespannung", "Battery voltage"}, "V", 24).positive(),
        Num::number("C", {"Kapazität", "Capacity"}, "Ah", 7).positive(),
        Num::number("P", {"Lastleistung", "Load power"}, "W", 20).positive(),
        Num::number("eta", {"Wirkungsgrad Wandler", "Converter efficiency"}, "%", 90).rangeExclusiveMin(0, 100),
        Num::number("dod", {"Nutzbare Entladetiefe", "Usable depth of discharge"}, "%", 80).rangeExclusiveMin(0, 100),
    };
    c.outputs = {Out::number("E", {"Nutzbare Energie", "Usable energy"}, "Wh", 1),
                 Out::number("I", {"Batteriestrom", "Battery current"}, "A", 2),
                 Out::number("th", {"Laufzeit", "Runtime"}, "h", 2).primary(),
                 Out::number("tmin", {"Laufzeit", "Runtime"}, "min", 0)};
    c.keywords = {"akku", "batterie", "usv", "laufzeit", "battery", "ups", "runtime"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double eta = in.num("eta") / 100.0;
        const double E = in.num("U") * in.num("C") * in.num("dod") / 100.0;
        const double t = E * eta / in.num("P");
        r.set("E", E);
        r.set("I", in.num("P") / eta / in.num("U"));
        r.set("th", t);
        r.set("tmin", t * 60.0);
        return r;
    };
    return c;
}

}  // namespace

void addElectronics(std::vector<Calculator>& out) {
    out.push_back(colourCode());
    out.push_back(eSeries());
    out.push_back(voltageDivider());
    out.push_back(rcCircuit());
    out.push_back(lcResonance());
    out.push_back(reactance());
    out.push_back(batteryRuntime());
}

}  // namespace elc::catalog
