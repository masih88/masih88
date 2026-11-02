// Calculators: Umrechner / Converters
#include <cmath>
#include <span>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cables.hpp"
#include "elc/formulas/units.hpp"

namespace elc::catalog {

namespace {

/// One generic converter per unit table: value + unit in, all units out.
Calculator tableConverter(std::string id, Text title, Text summary, std::span<const units::Unit> table,
                          std::string defaultUnit, double defaultValue, std::vector<std::string> keywords, Text description = {}) {
    Calculator c;
    c.id = std::move(id);
    c.category = "converters";
    c.title = std::move(title);
    c.summary = std::move(summary);
    c.description = {"Umrechnung über die SI-Basiseinheit. Faktoren nach ISO 80000 bzw. üblichen Definitionen "
                     "(PS = 735,49875 W, hp = 745,6999 W, kcal = 4186,8 J, BTU (IT) = 1055,056 J).",
                     "Conversion via the SI base unit. Factors per ISO 80000 or common definitions "
                     "(PS = 735.49875 W, hp = 745.6999 W, kcal = 4186.8 J, BTU (IT) = 1055.056 J)."};
    if (!description.de.empty()) c.description = std::move(description);
    std::vector<Option> opts;
    for (const auto& u : table) {
        opts.push_back({std::string(u.id), {std::string(u.symbol) + " – " + u.name.de, std::string(u.symbol) + " – " + u.name.en}});
        c.outputs.push_back(Out::number(std::string(u.id), u.name, std::string(u.symbol), 4));
    }
    c.inputs = {Num::number("v", {"Wert", "Value"}, "", defaultValue), Num::choice("u", {"Einheit", "Unit"}, opts, std::move(defaultUnit))};
    c.keywords = std::move(keywords);
    c.compute = [table](const Inputs& in, const Context&) {
        Result r;
        const units::Unit* from = &table.front();
        for (const auto& u : table) {
            if (in.choice("u") == u.id) from = &u;
        }
        for (const auto& u : table) r.set(std::string(u.id), units::convert(in.num("v"), *from, u));
        return r;
    };
    return c;
}

Calculator temperature() {
    Calculator c;
    c.id = "conv_temperature";
    c.category = "converters";
    c.title = {"Temperatur", "Temperature"};
    c.summary = {"°C ↔ K ↔ °F", "°C ↔ K ↔ °F"};
    c.description = {"T[K] = θ[°C] + 273,15    θ[°F] = θ[°C] · 9/5 + 32", "T[K] = θ[°C] + 273.15    θ[°F] = θ[°C] · 9/5 + 32"};
    c.inputs = {Num::number("v", {"Wert", "Value"}, "", 20),
                Num::choice("u", {"Einheit", "Unit"}, {{"C", "°C"}, {"K", "K"}, {"F", "°F"}}, "C")};
    c.outputs = {Out::number("C", {"Celsius", "Celsius"}, "°C", 2), Out::number("K", {"Kelvin", "Kelvin"}, "K", 2),
                 Out::number("F", {"Fahrenheit", "Fahrenheit"}, "°F", 2)};
    c.keywords = {"temperatur", "celsius", "kelvin", "fahrenheit"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double v = in.num("v");
        double C = v;
        if (in.choice("u") == "K") C = v - 273.15;
        if (in.choice("u") == "F") C = units::fahrenheitToCelsius(v);
        if (C < -273.15) throw CalcError({"Unter dem absoluten Nullpunkt.", "Below absolute zero."});
        r.set("C", C);
        r.set("K", units::celsiusToKelvin(C));
        r.set("F", units::celsiusToFahrenheit(C));
        return r;
    };
    return c;
}

Calculator awg() {
    Calculator c;
    c.id = "conv_awg";
    c.category = "converters";
    c.title = {"AWG ↔ mm²", "AWG ↔ mm²"};
    c.summary = {"American Wire Gauge in metrischen Querschnitt", "American Wire Gauge to metric cross-section"};
    c.description = {"d = 0,127 mm · 92^((36 − AWG)/39)    A = π/4 · d²\nAWG 1/0 = 0, 2/0 = −1, 3/0 = −2, 4/0 = −3 eingeben.",
                     "d = 0.127 mm · 92^((36 − AWG)/39)    A = π/4 · d²\nEnter AWG 1/0 = 0, 2/0 = −1, 3/0 = −2, 4/0 = −3."};
    c.inputs = {
        Num::choice("dir", {"Gegeben", "Given"}, {{"awg", "AWG"}, {"mm2", "mm²"}}, "awg"),
        Num::number("awg", {"AWG", "AWG"}, "", 18).range(-3, 40).showIf("dir", {"awg"}),
        Num::number("mm2", {"Querschnitt", "Cross-section"}, "mm²", 1.5).positive().showIf("dir", {"mm2"}),
    };
    c.outputs = {Out::number("awg", {"AWG", "AWG"}, "", 1), Out::number("d", {"Durchmesser", "Diameter"}, "mm", 3),
                 Out::number("A", {"Querschnitt", "Cross-section"}, "mm²", 3),
                 Out::number("metric", {"Nächster metrischer Normquerschnitt ≥", "Next metric standard size ≥"}, "mm²", 2)};
    c.keywords = {"awg", "querschnitt", "gauge", "mm2"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double n = in.choice("dir") == "mm2" ? std::round(units::awgFromArea(in.num("mm2"))) : in.num("awg");
        const double A = units::awgArea(n);
        r.set("awg", n);
        r.set("d", units::awgDiameter(n));
        r.set("A", A);
        if (const auto m = nextAtLeast(cables::kStandardAreas, A)) r.set("metric", *m);
        return r;
    };
    return c;
}

}  // namespace

Calculator currency() {
    Calculator c;
    c.id = "conv_currency";
    c.category = "converters";
    c.title = {"Währung € ↔ CHF", "Currency € ↔ CHF"};
    c.summary = {"Euro und Schweizer Franken umrechnen", "Convert euro and Swiss franc"};
    c.description = {
        "CHF = € · Kurs    € = CHF / Kurs    (Kurs = Franken für 1 Euro)\n"
        "Ohne Eingabe gilt der Kurs aus den Einstellungen (dort für alle Kostenrechnungen verwendet). Stand Oktober 2026 "
        "etwa 0,92 CHF/€ – Tageskurs der Bank verwenden.",
        "CHF = € · rate    € = CHF / rate    (rate = francs per euro)\n"
        "Left empty, the rate from the settings is used (also for all cost results). October 2026 about 0.92 CHF/€ – "
        "use your bank's daily rate."};
    c.inputs = {
        Num::number("amount", {"Betrag", "Amount"}, "", 1000).nonNegative(),
        Num::choice("from", {"Von", "From"}, {{"EUR", {"Euro (€) → CHF", "Euro (€) → CHF"}}, {"CHF", {"Franken (CHF) → €", "Franc (CHF) → €"}}},
                    "EUR"),
        Num::number("rate", {"Kurs 1 € in CHF", "Rate 1 € in CHF"}, "CHF", kNaN).opt().positive()
            .withHint({"leer = Kurs aus den Einstellungen", "empty = rate from the settings"}),
    };
    c.outputs = {Out::number("out", {"Ergebnis", "Result"}, "", 2).primary(),
                 Out::number("rateUsed", {"Verwendeter Kurs 1 € =", "Rate used 1 € ="}, "CHF", 4),
                 Out::number("inv", {"Umkehrkurs 1 CHF =", "Inverse rate 1 CHF ="}, "€", 4)};
    c.keywords = {"währung", "euro", "franken", "chf", "eur", "wechselkurs", "currency", "exchange"};
    c.compute = [](const Inputs& in, const Context& ctx) {
        Result r;
        const auto given = in.maybe("rate");
        const double rate = given ? *given : ctx.chfPerEur;
        if (!(rate > 0)) throw CalcError({"Kein gültiger Wechselkurs.", "No valid exchange rate."});
        const bool fromEur = in.choice("from") == "EUR";
        r.set("out", fromEur ? in.num("amount") * rate : in.num("amount") / rate, fromEur ? "CHF" : "€");
        r.set("rateUsed", rate);
        r.set("inv", 1.0 / rate);
        if (!given) r.info({"Kurs aus den Einstellungen.", "Rate from the settings."});
        return r;
    };
    return c;
}

void addConverters(std::vector<Calculator>& out) {
    out.push_back(currency());
    out.push_back(tableConverter("conv_power", {"Leistung", "Power"}, {"W, kW, PS, hp, BTU/h, kcal/h, TR", "W, kW, PS, hp, BTU/h, kcal/h, TR"},
                                 units::powerUnits(), "kW", 1, {"leistung", "ps", "hp", "btu", "kälteleistung", "power"}));
    out.push_back(tableConverter("conv_energy", {"Energie", "Energy"}, {"J, Wh, kWh, kcal, BTU", "J, Wh, kWh, kcal, BTU"},
                                 units::energyUnits(), "kWh", 1, {"energie", "kwh", "joule", "energy"}));
    out.push_back(temperature());
    out.push_back(tableConverter("conv_pressure", {"Druck", "Pressure"}, {"Pa, kPa, bar, mbar, psi, mWS", "Pa, kPa, bar, mbar, psi, mWS"},
                                 units::pressureUnits(), "bar", 1, {"druck", "bar", "pascal", "psi", "pressure"}));
    out.push_back(tableConverter("conv_flow", {"Volumenstrom", "Flow rate"}, {"m³/h, l/s, l/min, CFM, gpm", "m³/h, l/s, l/min, CFM, gpm"},
                                 units::flowUnits(), "m3h", 1, {"volumenstrom", "durchfluss", "cfm", "flow"}));
    out.push_back(awg());
    out.push_back(tableConverter("conv_area", {"Querschnitt / Fläche", "Cross-section / area"},
                                 {"mm², cm², m², in², kcmil (MCM), cmil", "mm², cm², m², in², kcmil (MCM), cmil"}, units::areaUnits(), "mm2",
                                 2.5, {"querschnitt", "fläche", "kcmil", "mcm", "circular mil", "area"},
                                 {"Umrechnung über m². 1 in = 25,4 mm (exakt); 1 cmil = Fläche eines Kreises von 1 mil (0,001 in) "
                                  "Durchmesser = π/4 · (25,4 µm)² = 5,0671 · 10⁻⁴ mm²; 1 kcmil (MCM) = 1000 cmil = 0,5067 mm².",
                                  "Conversion via m². 1 in = 25.4 mm (exact); 1 cmil = area of a circle of 1 mil (0.001 in) "
                                  "diameter = π/4 · (25.4 µm)² = 5.0671 · 10⁻⁴ mm²; 1 kcmil (MCM) = 1000 cmil = 0.5067 mm²."}));
    out.push_back(tableConverter("conv_length", {"Länge", "Length"}, {"mm, m, km, mil, in, ft, yd, mi, sm", "mm, m, km, mil, in, ft, yd, mi, nmi"},
                                 units::lengthUnits(), "m", 1, {"länge", "zoll", "inch", "fuß", "meile", "length"},
                                 {"Umrechnung über Meter. Zoll 25,4 mm, Fuß 0,3048 m, Yard 0,9144 m, Meile 1609,344 m, "
                                  "Seemeile 1852 m (alle exakt definiert).",
                                  "Conversion via metres. Inch 25.4 mm, foot 0.3048 m, yard 0.9144 m, mile 1609.344 m, "
                                  "nautical mile 1852 m (all exact by definition)."}));
}

}  // namespace elc::catalog
