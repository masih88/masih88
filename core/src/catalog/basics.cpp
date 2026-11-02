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
    c.description = {"E = P · t    Kosten = E · Preis\nWährung (€ oder CHF) und Wechselkurs in den Einstellungen; der Betrag "
                     "wird zusätzlich in der anderen Währung angezeigt.",
                     "E = P · t    Cost = E · price\nCurrency (€ or CHF) and exchange rate in the settings; the amount is "
                     "also shown in the other currency."};
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
        const std::string currency = currencySymbol(ctx);
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
    out.push_back(seriesParallel(false));
    out.push_back(seriesParallel(true));
    out.push_back(energyCost());
}

}  // namespace elc::catalog
