// Calculators: Grundlagen / Basics
#include <array>
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/basics.hpp"
#include "elc/formulas/cables.hpp"

namespace elc::catalog {

namespace {

Calculator ohm() {
    Calculator c;
    c.id = "ohm";
    c.category = "basics";
    c.title = {"Ohmsches Gesetz & Leistung", "Ohm's law & power"};
    c.summary = {"U, I, R, P – zwei Werte eingeben", "U, I, R, P – enter any two values"};
    c.description = {
        "Genau zwei der vier Größen eingeben, die übrigen werden berechnet.\n"
        "U = R · I    P = U · I = I² · R = U² / R",
        "Enter exactly two of the four quantities; the others are calculated.\n"
        "U = R · I    P = U · I = I² · R = U² / R"};
    c.inputs = {
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 230).opt(),
        Num::number("I", {"Strom I", "Current I"}, "A", kNaN).opt(),
        Num::number("R", {"Widerstand R", "Resistance R"}, "Ω", kNaN).opt().nonNegative(),
        Num::number("P", {"Leistung P", "Power P"}, "W", 1000).opt(),
    };
    c.outputs = {Out::number("U", {"Spannung U", "Voltage U"}, "V", 3), Out::number("I", {"Strom I", "Current I"}, "A", 4),
                 Out::number("R", {"Widerstand R", "Resistance R"}, "Ω", 4),
                 Out::number("P", {"Leistung P", "Power P"}, "W", 3)};
    c.keywords = {"ohm", "spannung", "strom", "widerstand", "leistung", "voltage", "current"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto v = basics::solveOhm(in.maybe("U"), in.maybe("I"), in.maybe("R"), in.maybe("P"));
        if (!v) {
            r.error({"Bitte genau zwei Werte eingeben (keine Division durch 0).",
                     "Please enter exactly two values (no division by zero)."});
            return r;
        }
        r.set("U", v->U);
        r.set("I", v->I);
        r.set("R", v->R);
        r.set("P", v->P);
        return r;
    };
    return c;
}

Calculator acPower() {
    Calculator c;
    c.id = "power";
    c.category = "basics";
    c.title = {"Leistung P, Q, S", "Power P, Q, S"};
    c.summary = {"Wirk-, Blind-, Scheinleistung aus U, I, cos φ", "Active, reactive, apparent power from U, I, cos φ"};
    c.description = {
        "S = k · U · I   (k = 1 bei DC/1~, k = √3 bei 3~ mit Außenleiterspannung)\n"
        "P = S · cos φ    Q = S · sin φ    φ = arccos(cos φ)",
        "S = k · U · I   (k = 1 for DC/1~, k = √3 for 3~ with line-to-line voltage)\n"
        "P = S · cos φ    Q = S · sin φ    φ = arccos(cos φ)"};
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        Num::number("I", {"Strom I", "Current I"}, "A", 16).nonNegative(),
        cosPhiInput(0.9),
    };
    c.outputs = {Out::number("S", {"Scheinleistung S", "Apparent power S"}, "kVA", 3),
                 Out::number("P", {"Wirkleistung P", "Active power P"}, "kW", 3),
                 Out::number("Q", {"Blindleistung Q", "Reactive power Q"}, "kvar", 3),
                 Out::number("phi", {"Phasenwinkel φ", "Phase angle φ"}, "°", 2)};
    c.keywords = {"wirkleistung", "blindleistung", "scheinleistung", "cos phi", "leistungsfaktor", "power factor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const auto p = basics::powerFromVoltageCurrent(sys, in.num("U"), in.num("I"), in.num("cosphi"));
        r.set("S", p.S / 1e3);
        r.set("P", p.P / 1e3);
        r.set("Q", p.Q / 1e3);
        r.set("phi", p.phiDeg);
        return r;
    };
    return c;
}

Calculator currentFromPower() {
    Calculator c;
    c.id = "current_from_power";
    c.category = "basics";
    c.title = {"Strom aus Leistung", "Current from power"};
    c.summary = {"I = P / (k · U · cos φ · η)", "I = P / (k · U · cos φ · η)"};
    c.description = {
        "I = P / (k · U · cos φ · η)\nk = 1 bei DC/1~, k = √3 bei 3~. P ist die abgegebene Leistung; "
        "η = 100 % für die aufgenommene Leistung.",
        "I = P / (k · U · cos φ · η)\nk = 1 for DC/1~, k = √3 for 3~. P is the output power; "
        "use η = 100 % for input power."};
    c.inputs = {
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
        Num::number("P", {"Leistung P", "Power P"}, "kW", 11).positive(),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        cosPhiInput(0.85),
        Num::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 100).rangeExclusiveMin(0, 100),
    };
    c.outputs = {Out::number("I", {"Strom I", "Current I"}, "A", 2),
                 Out::number("S", {"Aufgenommene Scheinleistung S", "Apparent input power S"}, "kVA", 3)};
    c.keywords = {"strom", "leistung", "current", "power"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double eta = in.num("eta") / 100.0;
        const double I = basics::currentFromActivePower(sys, in.num("P") * 1e3, in.num("U"), in.num("cosphi"), eta);
        r.set("I", I);
        r.set("S", powerFactorK(sys) * in.num("U") * I / 1e3);
        return r;
    };
    return c;
}

Calculator conductorResistance() {
    Calculator c;
    c.id = "conductor_resistance";
    c.category = "basics";
    c.title = {"Leiterwiderstand", "Conductor resistance"};
    c.summary = {"R = ρ(θ) · L / A", "R = ρ(θ) · L / A"};
    c.description = {
        "R = ρ(θ) · L / A    ρ(θ) = ρ20 · (1 + α20 · (θ − 20 °C))\n"
        "Cu: ρ20 = 0,017241 Ω·mm²/m, α20 = 0,00393 1/K (IEC 60028)\n"
        "Al: ρ20 = 0,028264 Ω·mm²/m, α20 = 0,00403 1/K (IEC 60889)",
        "R = ρ(θ) · L / A    ρ(θ) = ρ20 · (1 + α20 · (θ − 20 °C))\n"
        "Cu: ρ20 = 0.017241 Ω·mm²/m, α20 = 0.00393 1/K (IEC 60028)\n"
        "Al: ρ20 = 0.028264 Ω·mm²/m, α20 = 0.00403 1/K (IEC 60889)"};
    c.inputs = {
        Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
        Num::number("L", {"Länge L (einfach)", "Length L (one way)"}, "m", 100).positive(),
        Num::number("A", {"Querschnitt A", "Cross-section A"}, "mm²", 2.5).positive(),
        Num::number("T", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 20).range(-60, 250),
    };
    c.outputs = {Out::number("R", {"Widerstand eines Leiters", "Resistance of one conductor"}, "Ω", 4),
                 Out::number("Rloop", {"Schleifenwiderstand (Hin + Rück)", "Loop resistance (out + return)"}, "Ω", 4),
                 Out::number("rho", {"Spez. Widerstand ρ(θ)", "Resistivity ρ(θ)"}, "Ω·mm²/m", 6),
                 Out::number("kappa", {"Leitfähigkeit κ(θ)", "Conductivity κ(θ)"}, "m/(Ω·mm²)", 2)};
    c.keywords = {"widerstand", "leitung", "kupfer", "resistance", "copper", "kappa"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double rho = cables::resistivity(parseMaterial(in.choice("mat")), in.num("T"));
        const double R = rho * in.num("L") / in.num("A");
        r.set("R", R);
        r.set("Rloop", 2.0 * R);
        r.set("rho", rho);
        r.set("kappa", 1.0 / rho);
        return r;
    };
    return c;
}

Calculator seriesParallel(bool capacitors) {
    Calculator c;
    c.id = capacitors ? "capacitors_series_parallel" : "resistors_series_parallel";
    c.category = "basics";
    const std::string unit = capacitors ? "µF" : "Ω";
    if (capacitors) {
        c.title = {"Kondensatoren: Reihe & parallel", "Capacitors: series & parallel"};
        c.summary = {"C_ges für bis zu 6 Kondensatoren", "C_total for up to 6 capacitors"};
        c.description = {"Parallel: C = C1 + C2 + …\nReihe: 1/C = 1/C1 + 1/C2 + …\nLeere Felder werden ignoriert.",
                         "Parallel: C = C1 + C2 + …\nSeries: 1/C = 1/C1 + 1/C2 + …\nEmpty fields are ignored."};
    } else {
        c.title = {"Widerstände: Reihe & parallel", "Resistors: series & parallel"};
        c.summary = {"R_ges für bis zu 6 Widerstände (auch L)", "R_total for up to 6 resistors (also L)"};
        c.description = {"Reihe: R = R1 + R2 + …\nParallel: 1/R = 1/R1 + 1/R2 + …\n"
                         "Gilt genauso für Induktivitäten ohne Kopplung. Leere Felder werden ignoriert.",
                         "Series: R = R1 + R2 + …\nParallel: 1/R = 1/R1 + 1/R2 + …\n"
                         "Also valid for uncoupled inductors. Empty fields are ignored."};
    }
    const std::string sym = capacitors ? "C" : "R";
    for (int i = 1; i <= 6; ++i) {
        const auto n = std::to_string(i);
        const double def = i == 1 ? (capacitors ? 10.0 : 100.0) : (i == 2 ? (capacitors ? 22.0 : 220.0) : kNaN);
        c.inputs.push_back(Num::number("x" + n, {sym + n, sym + n}, unit, def).opt().positive());
    }
    c.outputs = {Out::number("series", {"Reihenschaltung", "Series connection"}, unit, 4),
                 Out::number("parallel", {"Parallelschaltung", "Parallel connection"}, unit, 4)};
    c.keywords = capacitors ? std::vector<std::string>{"kondensator", "kapazität", "capacitor"}
                            : std::vector<std::string>{"widerstand", "reihe", "parallel", "resistor", "induktivität"};
    c.compute = [capacitors](const Inputs& in, const Context&) {
        Result r;
        std::vector<double> v;
        for (int i = 1; i <= 6; ++i) {
            if (const auto x = in.maybe("x" + std::to_string(i))) v.push_back(*x);
        }
        if (v.empty()) {
            r.error({"Mindestens einen Wert eingeben.", "Enter at least one value."});
            return r;
        }
        const double sum = basics::directSum(v);
        const double rec = basics::reciprocalSum(v);
        r.set("series", capacitors ? rec : sum);
        r.set("parallel", capacitors ? sum : rec);
        return r;
    };
    return c;
}

Calculator energyCost() {
    Calculator c;
    c.id = "energy_cost";
    c.category = "basics";
    c.title = {"Energieverbrauch & Kosten", "Energy consumption & cost"};
    c.summary = {"kWh und Kosten pro Tag/Jahr", "kWh and cost per day/year"};
    c.description = {"E = P · t    Kosten = E · Preis\nDie Währung ergibt sich aus dem eingegebenen Preis (€ oder CHF).",
                     "E = P · t    Cost = E · price\nThe currency follows the entered price (€ or CHF)."};
    c.inputs = {
        Num::number("P", {"Leistung P", "Power P"}, "W", 2000).nonNegative(),
        Num::number("h", {"Betriebsstunden pro Tag", "Operating hours per day"}, "h", 8).range(0, 24),
        Num::number("d", {"Betriebstage pro Jahr", "Operating days per year"}, "d", 365).range(0, 366),
        Num::number("price", {"Strompreis pro kWh", "Energy price per kWh"}, "€ / CHF", 0.30).nonNegative(),
    };
    c.outputs = {Out::number("Eday", {"Energie pro Tag", "Energy per day"}, "kWh", 3),
                 Out::number("Eyear", {"Energie pro Jahr", "Energy per year"}, "kWh", 1),
                 Out::number("Cday", {"Kosten pro Tag", "Cost per day"}, "€", 2),
                 Out::number("Cyear", {"Kosten pro Jahr", "Cost per year"}, "€", 2)};
    c.keywords = {"kosten", "kwh", "energie", "cost", "energy"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const std::string currency = ctx.norm == NormProfile::CH ? "CHF" : "€";
        const double eDay = in.num("P") * in.num("h") / 1e3;
        const double eYear = eDay * in.num("d");
        r.set("Eday", eDay);
        r.set("Eyear", eYear);
        r.set("Cday", eDay * in.num("price"), currency);
        r.set("Cyear", eYear * in.num("price"), currency);
        return r;
    };
    return c;
}

}  // namespace

void addBasics(std::vector<Calculator>& out) {
    out.push_back(ohm());
    out.push_back(acPower());
    out.push_back(currentFromPower());
    out.push_back(conductorResistance());
    out.push_back(seriesParallel(false));
    out.push_back(seriesParallel(true));
    out.push_back(energyCost());
}

}  // namespace elc::catalog
