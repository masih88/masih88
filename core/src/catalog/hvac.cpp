// Calculators (category automation): moist air and outdoor air demand
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/air.hpp"

namespace elc::catalog {

namespace {

Calculator humidAir() {
    Calculator c;
    c.id = "humid_air";
    c.category = "automation";
    c.title = {"Feuchte Luft (h-x)", "Moist air (psychrometrics)"};
    c.summary = {"Taupunkt, absolute Feuchte, Enthalpie", "Dew point, humidity ratio, enthalpy"};
    c.description = {
        "Sättigungsdampfdruck (Magnus, WMO, über Wasser, −45…60 °C): p_s = 6,112 · e^(17,62·t / (243,12 + t)) hPa\n"
        "p_D = φ · p_s    x = 0,622 · p_D / (p − p_D)    h = 1,006 · t + x · (2501 + 1,86 · t) kJ/kg\n"
        "Taupunkt: τ = 243,12 · γ / (17,62 − γ), γ = ln(φ) + 17,62 · t / (243,12 + t). Luftdruck nach Höhe anpassen "
        "(Basel ≈ 980 hPa).",
        "Saturation pressure (Magnus, WMO, over water, −45…60 °C): p_s = 6.112 · e^(17.62·t / (243.12 + t)) hPa\n"
        "p_v = φ · p_s    x = 0.622 · p_v / (p − p_v)    h = 1.006 · t + x · (2501 + 1.86 · t) kJ/kg\n"
        "Dew point: τ = 243.12 · γ / (17.62 − γ), γ = ln(φ) + 17.62 · t / (243.12 + t). Adjust pressure for altitude "
        "(Basel ≈ 980 hPa)."};
    c.inputs = {
        Num::number("t", {"Lufttemperatur t", "Air temperature t"}, "°C", 22).range(-40, 60),
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"rh", {"Relative Feuchte", "Relative humidity"}}, {"td", {"Taupunkt", "Dew point"}}}, "rh"),
        Num::number("rh", {"Relative Feuchte φ", "Relative humidity φ"}, "%", 45).rangeExclusiveMin(0, 100).showIf("dir", {"rh"}),
        Num::number("td", {"Taupunkt τ", "Dew point τ"}, "°C", 9.5).range(-60, 60).showIf("dir", {"td"}),
        Num::number("p", {"Luftdruck", "Air pressure"}, "hPa", 1013.25).range(500, 1100),
    };
    c.outputs = {Out::number("td", {"Taupunkt τ", "Dew point τ"}, "°C", 2).primary(),
                 Out::number("rh", {"Relative Feuchte φ", "Relative humidity φ"}, "%", 1),
                 Out::number("x", {"Absolute Feuchte x", "Humidity ratio x"}, "g/kg", 2).primary(),
                 Out::number("h", {"Enthalpie h", "Enthalpy h"}, "kJ/kg", 2),
                 Out::number("pd", {"Dampfdruck p_D", "Vapour pressure p_v"}, "hPa", 2),
                 Out::number("ps", {"Sättigungsdampfdruck p_s", "Saturation pressure p_s"}, "hPa", 2),
                 Out::number("rho", {"Dichte", "Density"}, "kg/m³", 3)};
    c.keywords = {"taupunkt", "feuchte", "enthalpie", "h-x", "mollier", "psychrometrie", "dew point", "humidity"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double t = in.num("t");
        double rh = 0, td = 0;
        if (in.choice("dir") == "td") {
            td = in.num("td");
            if (td > t) throw CalcError({"Taupunkt kann nicht über der Lufttemperatur liegen.", "Dew point cannot exceed air temperature."});
            rh = air::relativeHumidity(t, td);
        } else {
            rh = in.num("rh");
            td = air::dewPoint(t, rh);
        }
        const double ps = air::saturationPressure(t);
        const double pd = ps * rh / 100.0;
        const double x = air::humidityRatio(pd, in.num("p"));
        r.set("td", td);
        r.set("rh", rh);
        r.set("x", x * 1000.0);
        r.set("h", air::enthalpy(t, x));
        r.set("pd", pd);
        r.set("ps", ps);
        r.set("rho", air::density(t, pd, in.num("p")));
        return r;
    };
    return c;
}

Calculator freshAir() {
    Calculator c;
    c.id = "fresh_air";
    c.category = "automation";
    c.title = {"Außenluftvolumenstrom", "Outdoor air flow rate"};
    c.summary = {"Nach EN 16798-1 (Personen + Fläche) oder CO₂-Bilanz", "Per EN 16798-1 (persons + area) or CO₂ balance"};
    c.description = {
        "EN 16798-1 (Anhang B, Verfahren 1): q = n · q_P + A · q_B\n"
        "q_P je Person: Kat. I 10, II 7, III 4 l/s.   q_B je m² (schadstoffarmes Gebäude): I 1,0, II 0,7, III 0,4 l/(s·m²); "
        "sehr schadstoffarm: 0,5 / 0,35 / 0,2; nicht schadstoffarm: 2,0 / 1,4 / 0,8.\n"
        "CO₂-Bilanz (stationär): V̇ = n · G / (C_Soll − C_außen), G ≈ 20 l/h je Person bei Büroarbeit.",
        "EN 16798-1 (Annex B, method 1): q = n · q_P + A · q_B\n"
        "q_P per person: cat. I 10, II 7, III 4 l/s.   q_B per m² (low-polluting building): I 1.0, II 0.7, III 0.4 l/(s·m²); "
        "very low-polluting: 0.5 / 0.35 / 0.2; non-low-polluting: 2.0 / 1.4 / 0.8.\n"
        "CO₂ balance (steady state): V̇ = n · G / (C_set − C_outdoor), G ≈ 20 l/h per person for office work."};
    c.inputs = {
        Num::choice("method", {"Verfahren", "Method"},
                    {{"en", "EN 16798-1"}, {"co2", {"CO₂-Bilanz", "CO₂ balance"}}}, "en"),
        Num::number("n", {"Personen", "Persons"}, "", 10).nonNegative(),
        Num::number("A", {"Raumfläche", "Floor area"}, "m²", 80).nonNegative(),
        Num::number("H", {"Raumhöhe (für Luftwechsel)", "Room height (for air change)"}, "m", 3).positive(),
        Num::choice("cat", {"Kategorie", "Category"},
                    {{"I", {"I – hohe Erwartung", "I – high expectation"}}, {"II", {"II – normal", "II – medium"}},
                     {"III", {"III – moderat", "III – moderate"}}},
                    "II")
            .showIf("method", {"en"}),
        Num::choice("poll", {"Gebäude-Emission", "Building pollution"},
                    {{"very", {"sehr schadstoffarm", "very low-polluting"}}, {"low", {"schadstoffarm", "low-polluting"}},
                     {"non", {"nicht schadstoffarm", "non-low-polluting"}}},
                    "low")
            .showIf("method", {"en"}),
        Num::number("G", {"CO₂-Abgabe je Person", "CO₂ emission per person"}, "l/h", 20).positive(),
        Num::number("Cset", {"CO₂-Sollwert", "CO₂ set point"}, "ppm", 1000).positive().showIf("method", {"co2"}),
        Num::number("Cout", {"CO₂ außen", "CO₂ outdoor"}, "ppm", 420).nonNegative(),
    };
    c.outputs = {Out::number("q", {"Außenluftvolumenstrom", "Outdoor airflow"}, "m³/h", 0).primary(),
                 Out::number("qls", {"Außenluftvolumenstrom", "Outdoor airflow"}, "l/s", 1),
                 Out::number("qp", {"Je Person", "Per person"}, "m³/h", 1),
                 Out::number("ach", {"Luftwechselrate", "Air change rate"}, "1/h", 2),
                 Out::number("co2", {"CO₂-Konzentration (stationär)", "CO₂ concentration (steady state)"}, "ppm", 0).primary()};
    c.keywords = {"außenluft", "frischluft", "co2", "en 16798", "lüftung", "luftqualität", "fresh air", "ventilation"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double n = in.num("n");
        double q = 0;  // m³/h
        if (in.choice("method") == "co2") {
            if (in.num("Cset") <= in.num("Cout")) throw CalcError({"CO₂-Sollwert muss über dem Außenwert liegen.", "CO₂ set point must exceed outdoor value."});
            q = air::co2Airflow(n, in.num("G"), in.num("Cset"), in.num("Cout"));
        } else {
            const auto& k = in.choice("cat");
            const int i = k == "I" ? 0 : (k == "II" ? 1 : 2);
            constexpr double qp[3] = {10.0, 7.0, 4.0};
            constexpr double very[3] = {0.5, 0.35, 0.2};
            constexpr double low[3] = {1.0, 0.7, 0.4};
            constexpr double non[3] = {2.0, 1.4, 0.8};
            const auto& p = in.choice("poll");
            const double qb = p == "very" ? very[i] : (p == "non" ? non[i] : low[i]);
            q = (n * qp[i] + in.num("A") * qb) * 3.6;
        }
        if (q <= 0) throw CalcError({"Keine Personen und keine Fläche angegeben.", "No persons and no area given."});
        r.set("q", q);
        r.set("qls", q / 3.6);
        if (n > 0) r.set("qp", q / n);
        if (in.num("A") > 0) r.set("ach", q / (in.num("A") * in.num("H")));
        const double co2 = air::co2Concentration(n, in.num("G"), q, in.num("Cout"));
        r.set("co2", co2);
        // Umweltbundesamt (UBA) guidance values for indoor CO₂
        if (co2 > 2000) {
            r.error({"CO₂ > 2000 ppm – hygienisch inakzeptabel (UBA).", "CO₂ > 2000 ppm – hygienically unacceptable (UBA)."});
        } else if (co2 > 1000) {
            r.warn({"CO₂ 1000–2000 ppm – hygienisch auffällig (UBA).", "CO₂ 1000–2000 ppm – hygienically conspicuous (UBA)."});
        } else {
            r.ok({"CO₂ ≤ 1000 ppm – hygienisch unbedenklich (UBA).", "CO₂ ≤ 1000 ppm – hygienically harmless (UBA)."});
        }
        return r;
    };
    return c;
}

}  // namespace

void addHvac(std::vector<Calculator>& out) {
    out.push_back(humidAir());
    out.push_back(freshAir());
}

}  // namespace elc::catalog
