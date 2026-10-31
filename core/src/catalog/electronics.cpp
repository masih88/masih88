// Calculators: Elektronik / Electronics
#include <array>
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/basics.hpp"
#include "elc/formulas/electronics.hpp"

namespace elc::catalog {

using namespace elc::electronics;

namespace {

struct Colour {
    const char* id;
    const char* de;
    const char* en;
};
constexpr std::array<Colour, 12> kColours{{
    {"black", "Schwarz", "Black"},
    {"brown", "Braun", "Brown"},
    {"red", "Rot", "Red"},
    {"orange", "Orange", "Orange"},
    {"yellow", "Gelb", "Yellow"},
    {"green", "Grün", "Green"},
    {"blue", "Blau", "Blue"},
    {"violet", "Violett", "Violet"},
    {"grey", "Grau", "Grey"},
    {"white", "Weiß", "White"},
    {"gold", "Gold", "Gold"},
    {"silver", "Silber", "Silver"},
}};

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

Calculator colourCode() {
    Calculator c;
    c.id = "resistor_color";
    c.category = "electronics";
    c.title = {"Farbcode Widerstand / Spule", "Colour code resistor / inductor"};
    c.summary = {"4- und 5-Ring-Code nach IEC 60062", "4- and 5-band code per IEC 60062"};
    c.description = {"4 Ringe: Ziffer, Ziffer, Multiplikator, Toleranz\n5 Ringe: Ziffer, Ziffer, Ziffer, Multiplikator, Toleranz\n"
                     "Induktivitäten: gleiche Codierung, Grundeinheit µH.",
                     "4 bands: digit, digit, multiplier, tolerance\n5 bands: digit, digit, digit, multiplier, tolerance\n"
                     "Inductors: same code, base unit µH."};
    c.inputs = {
        Num::choice("part", {"Bauteil", "Component"},
                    {{"r", {"Widerstand (Ω)", "Resistor (Ω)"}}, {"l", {"Induktivität (µH)", "Inductor (µH)"}}}, "r"),
        Num::choice("bands", {"Anzahl Ringe", "Number of bands"}, {{"4", {"4 Ringe", "4 bands"}}, {"5", {"5 Ringe", "5 bands"}}}, "4"),
        Num::choice("b1", {"1. Ring (Ziffer)", "1st band (digit)"}, digitOptions(), "yellow"),
        Num::choice("b2", {"2. Ring (Ziffer)", "2nd band (digit)"}, digitOptions(), "violet"),
        Num::choice("b3", {"3. Ring (Ziffer)", "3rd band (digit)"}, digitOptions(), "black"),
        Num::choice("mult", {"Multiplikator", "Multiplier"}, multiplierOptions(), "red"),
        Num::choice("tol", {"Toleranz", "Tolerance"}, toleranceOptions(), "gold"),
    };
    c.inputs[4].showIf("bands", {"5"});
    c.outputs = {Out::number("value", {"Nennwert", "Nominal value"}, "Ω", 3).primary(),
                 Out::number("tol", {"Toleranz", "Tolerance"}, "%", 2),
                 Out::number("min", {"Minimalwert", "Minimum"}, "Ω", 3),
                 Out::number("max", {"Maximalwert", "Maximum"}, "Ω", 3)};
    c.keywords = {"farbcode", "farbring", "widerstand", "colour code", "color code", "resistor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const bool five = in.choice("bands") == "5";
        const bool henry = in.choice("part") == "l";
        double digits = digitOf(in.choice("b1")) * 10 + digitOf(in.choice("b2"));
        if (five) digits = digits * 10 + digitOf(in.choice("b3"));
        const double v = digits * multiplierOf(in.choice("mult"));
        const double tol = toleranceOf(in.choice("tol"));
        const auto [val, unit] = withPrefix(v, henry);
        r.set("value", val, unit);
        r.set("tol", tol);
        r.set("min", val * (1 - tol / 100), unit);
        r.set("max", val * (1 + tol / 100), unit);
        return r;
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

Calculator ledResistor() {
    Calculator c;
    c.id = "led_resistor";
    c.category = "electronics";
    c.title = {"LED-Vorwiderstand", "LED series resistor"};
    c.summary = {"R = (U_B − n · U_F) / I_F", "R = (U_S − n · U_F) / I_F"};
    c.description = {"R = (U_B − n · U_F) / I_F    P_R = I² · R\nGewählt wird der nächstgrößere E24-Wert; Belastbarkeit ≥ 2 · P_R wählen.\n"
                     "Typ. U_F: rot 1,8–2,1 V, grün 2,0–3,2 V, blau/weiß 2,8–3,4 V.",
                     "R = (U_S − n · U_F) / I_F    P_R = I² · R\nThe next higher E24 value is selected; use a power rating ≥ 2 · P_R.\n"
                     "Typ. U_F: red 1.8–2.1 V, green 2.0–3.2 V, blue/white 2.8–3.4 V."};
    c.inputs = {
        Num::number("Ub", {"Versorgungsspannung U_B", "Supply voltage U_S"}, "V", 24).positive(),
        Num::number("Uf", {"Durchlassspannung U_F", "Forward voltage U_F"}, "V", 2.1).positive(),
        Num::number("If", {"LED-Strom I_F", "LED current I_F"}, "mA", 10).positive(),
        Num::number("n", {"LEDs in Reihe", "LEDs in series"}, "", 1).range(1, 100),
    };
    c.outputs = {Out::number("R", {"Berechneter Widerstand", "Calculated resistance"}, "Ω", 1),
                 Out::number("Re", {"Gewählt (E24)", "Selected (E24)"}, "Ω", 0).primary(),
                 Out::number("I", {"Strom mit E24-Wert", "Current with E24 value"}, "mA", 2),
                 Out::number("P", {"Verlustleistung", "Power dissipation"}, "mW", 1),
                 Out::number("Prated", {"Empfohlene Belastbarkeit ≥", "Recommended rating ≥"}, "W", 3)};
    c.keywords = {"led", "vorwiderstand", "series resistor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double U = in.num("Ub") - std::round(in.num("n")) * in.num("Uf");
        if (U <= 0) {
            throw CalcError({"Versorgungsspannung zu klein für die LED-Reihe.", "Supply voltage too low for the LED string."});
        }
        const double I = in.num("If") / 1e3;
        const double R = U / I;
        const double Re = eSeriesMatch(ESeries::E24, R).upper;
        const double Ie = U / Re;
        r.set("R", R);
        r.set("Re", Re);
        r.set("I", Ie * 1e3);
        r.set("P", Ie * Ie * Re * 1e3);
        r.set("Prated", 2.0 * Ie * Ie * Re);
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
    out.push_back(ledResistor());
    out.push_back(voltageDivider());
    out.push_back(rcCircuit());
    out.push_back(lcResonance());
    out.push_back(reactance());
    out.push_back(batteryRuntime());
}

}  // namespace elc::catalog
