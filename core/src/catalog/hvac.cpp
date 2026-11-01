// Calculators (category automation): moist air and outdoor air demand
#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/air.hpp"

namespace elc::catalog {

namespace {

// ---------------------------------------------------------------- h,x chart helpers

struct StatePoint {
    std::string label;
    double t;  // °C
    double x;  // kg/kg
};

double ceilTo(double v, double step) { return std::ceil(v / step) * step; }
double floorTo(double v, double step) { return std::floor(v / step) * step; }

/// Mollier grid: isotherms, isenthalps, lines of constant relative humidity and the saturation curve.
/// The range grows with the state points (x in g/kg on the abscissa, oblique t/h ordinate).
Chart hxGrid(double p, const std::vector<StatePoint>& pts) {
    double xMaxG = 30, yMin = -20, yMax = 50;
    for (const auto& s : pts) {
        xMaxG = std::max(xMaxG, ceilTo(s.x * 1000.0 + 3.0, 5.0));
        yMin = std::min(yMin, floorTo(s.t - 5.0, 10.0));
        yMax = std::max(yMax, ceilTo(air::mollierY(s.t, s.x) + 5.0, 10.0));
    }
    xMaxG = std::min(xMaxG, 80.0);
    const double xMax = xMaxG / 1000.0;

    Chart ch;
    ch.x = {{"Wassergehalt x in g/kg", "Humidity ratio x in g/kg"}, 0.0, xMaxG, xMaxG > 40 ? 10.0 : 5.0};
    ch.y = {{"Temperatur t in °C", "Temperature t in °C"}, yMin, yMax, 5.0};
    const auto pt = [](double t, double x) { return std::array<double, 2>{x * 1000.0, air::mollierY(t, x)}; };

    // isotherms (straight lines) up to the saturation curve
    for (double t = yMin; t <= yMax + 1e-9; t += 5.0) {
        const double xe = std::min(air::saturationRatio(t, p), xMax);
        ch.series.push_back({"t", {pt(t, 0.0), pt(t, xe)}});
    }
    // isenthalps h = const: y = (h − 2501·x) / 1.006, from the upper/left border to the saturation curve
    const double hMax = air::enthalpy(air::mollierT(yMax, xMax), xMax);
    for (double h = ceilTo(air::enthalpy(yMin, 0.0), 10.0); h <= hMax; h += 10.0) {
        const double x0 = std::max(0.0, (h - 1.006 * yMax) / 2501.0);
        double lo = -80.0, hi = 120.0;  // saturation temperature on this isenthalp
        for (int i = 0; i < 80; ++i) {
            const double mid = 0.5 * (lo + hi);
            (air::enthalpy(mid, air::saturationRatio(mid, p)) < h ? lo : hi) = mid;
        }
        const double x1 = std::min(air::saturationRatio(lo, p), xMax);
        if (x1 <= x0) continue;
        const auto y = [h](double x) { return (h - 2501.0 * x) / 1.006; };
        ch.series.push_back({"h", {{x0 * 1000.0, y(x0)}, {x1 * 1000.0, y(x1)}}});
        if (std::fmod(std::fabs(h), 20.0) < 1e-9 && y(x0) <= yMax + 1e-9 && x0 * 1000.0 < xMaxG - 2.0) {
            ch.labels.push_back({x0 * 1000.0, y(x0), formatNumber(h, 0), "h"});
        }
    }
    // relative humidity 10 … 100 %
    for (int rh = 10; rh <= 100; rh += 10) {
        ChartSeries s{rh == 100 ? "sat" : "rh", {}};
        for (double t = yMin; t <= yMax + 0.5; t += 0.5) {
            const double x = air::ratioFromRh(t, rh, p);
            s.pts.push_back(pt(t, x));
            if (x > xMax || air::mollierY(t, x) > yMax) break;  // first point outside: the UI clips
        }
        // label at the last point clearly inside the plot (curves leave at the top or at the right)
        const std::array<double, 2>* at = nullptr;
        for (const auto& q : s.pts) {
            if (q[1] <= yMax - 0.09 * (yMax - yMin) && q[0] <= xMaxG * 0.97) at = &q;
        }
        if (at) ch.labels.push_back({(*at)[0], (*at)[1], std::to_string(rh) + " %", rh == 100 ? "sat" : "rh"});
        ch.series.push_back(std::move(s));
    }
    return ch;
}

/// State markers and the dew point of the first state (dashed projection onto the saturation curve)
void addStates(Chart& ch, const std::vector<StatePoint>& pts, double p, bool dewPoint) {
    if (pts.empty()) return;
    if (dewPoint) {
        const auto& s = pts.front();
        const double td = air::dewPointFromRatio(s.x, p);
        if (td < s.t - 0.05) {
            ch.series.push_back({"aux", {{s.x * 1000.0, air::mollierY(s.t, s.x)}, {s.x * 1000.0, air::mollierY(td, s.x)}}});
            ch.markers.push_back({s.x * 1000.0, air::mollierY(td, s.x), "τ", "aux"});
        }
    }
    for (const auto& s : pts) ch.markers.push_back({s.x * 1000.0, air::mollierY(s.t, s.x), s.label, "state"});
}

Text pressureNote(double p, std::optional<double> altitude) {
    const std::string pd = formatNumber(p, 0, Lang::De);
    const std::string alt = altitude ? " (" + formatNumber(*altitude, 0, Lang::De) + " m ü. M.)" : std::string();
    const std::string altEn = altitude ? " (" + formatNumber(*altitude, 0, Lang::En) + " m a.s.l.)" : std::string();
    return {"p = " + pd + " hPa" + alt + " · gestrichelt: Enthalpie h in kJ/kg · ins Diagramm tippen = Zustand 1 setzen",
            "p = " + formatNumber(p, 0, Lang::En) + " hPa" + altEn + " · dashed: enthalpy h in kJ/kg · tap the chart to set state 1"};
}

/// Temperature and relative humidity at a chart position (clamped to the saturation curve), rounded to 0.1
std::pair<double, double> pickState(double xG, double y, double p) {
    const double x = std::max(0.0, xG / 1000.0);
    const double t = std::clamp(air::mollierT(y, x), -30.0, 60.0);
    const double rh = std::clamp(air::rhFromRatio(t, x, p), 1.0, 100.0);
    return {std::round(t * 10.0) / 10.0, std::round(rh * 10.0) / 10.0};
}

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
    c.chart = [](const Inputs& in, const Result& r, const Context&) {
        const double p = in.num("p");
        std::vector<StatePoint> pts;
        if (!r.hasError()) pts.push_back({"1", in.num("t"), r.values.at("x") / 1000.0});
        Chart ch = hxGrid(p, pts);
        addStates(ch, pts, p, true);
        ch.note = pressureNote(p, std::nullopt);
        return ch;
    };
    c.pick = [](const Inputs& in, double x, double y, const Context&) {
        const auto s = pickState(x, y, in.num("p"));
        return std::vector<PickValue>{{"t", s.first, {}}, {"dir", 0, "rh"}, {"rh", s.second, {}}};
    };
    return c;
}

Calculator hxDiagram() {
    Calculator c;
    c.id = "hx_diagram";
    c.category = "automation";
    c.title = {"h,x-Diagramm (Mollier) & Zustandsänderungen", "h,x chart (Mollier) & air processes"};
    c.summary = {"Taupunkt, Feuchte, Heizen/Kühlen/Befeuchten/Mischen im Diagramm",
                 "Dew point, humidity, heating/cooling/humidifying/mixing in the chart"};
    c.description = {
        "Luftdruck nach Normatmosphäre: p = 1013,25 · (1 − 0,0065 · H / 288,15)^5,255 hPa\n"
        "p_s nach Magnus (WMO, über Wasser): p_s = 6,112 · e^(17,62·t / (243,12 + t)) hPa    p_D = φ · p_s\n"
        "x = 0,622 · p_D / (p − p_D)    h = 1,006 · t + x · (2501 + 1,86 · t) kJ/kg tr. L.\n"
        "Taupunkt τ aus p_D (Magnus umgekehrt); Kühlgrenze t_f: h + (x_s(t_f) − x) · 4,19 · t_f = h_s(t_f)\n"
        "Erwärmen/Kühlen: x = konst.; unter dem Taupunkt ideal gesättigt (x₂ = x_s(t₂), Kondensat ṁ · Δx)\n"
        "Dampfbefeuchtung: t ≈ konst.; Verdunstungsbefeuchtung: h = konst.; Mischen: x und h massengewichtet\n"
        "Leistung Q = ṁ · Δh, ṁ = V̇ · ρ₁ / 3600. Diagramm: schiefwinklige h,x-Koordinaten (0-°C-Isotherme waagrecht).\n"
        "Unter 0 °C mit Sättigung über Wasser gerechnet. Reale Kühler: Bypass-Faktor / Oberflächentemperatur beachten.",
        "Air pressure from the standard atmosphere: p = 1013.25 · (1 − 0.0065 · H / 288.15)^5.255 hPa\n"
        "p_s by Magnus (WMO, over water): p_s = 6.112 · e^(17.62·t / (243.12 + t)) hPa    p_v = φ · p_s\n"
        "x = 0.622 · p_v / (p − p_v)    h = 1.006 · t + x · (2501 + 1.86 · t) kJ/kg dry air\n"
        "Dew point τ from p_v (inverse Magnus); wet bulb t_wb: h + (x_s(t_wb) − x) · 4.19 · t_wb = h_s(t_wb)\n"
        "Heating/cooling: x = const.; below the dew point ideally saturated (x₂ = x_s(t₂), condensate ṁ · Δx)\n"
        "Steam humidifier: t ≈ const.; evaporative humidifier: h = const.; mixing: x and h mass weighted\n"
        "Power Q = ṁ · Δh, ṁ = V̇ · ρ₁ / 3600. Chart: oblique h,x coordinates (0 °C isotherm horizontal).\n"
        "Below 0 °C saturation over water is used. Real coils: consider bypass factor / surface temperature."};
    const std::vector<std::string> withPower{"heat", "cool", "steam", "adiabatic"};
    c.inputs = {
        Num::number("alt", {"Höhe über Meer", "Altitude above sea level"}, "m", 300).range(-500, 5000)
            .withHint({"Basel ≈ 260–300 m, Zürich ≈ 410 m; ergibt den Luftdruck", "Sets the air pressure"}),
        Num::number("t1", {"Temperatur t₁", "Temperature t₁"}, "°C", 32).range(-30, 60)
            .startsSection({"Zustand 1 (z. B. Außenluft)", "State 1 (e.g. outdoor air)"}),
        Num::number("rh1", {"Relative Feuchte φ₁", "Relative humidity φ₁"}, "%", 40).rangeExclusiveMin(0, 100),
        Num::choice("proc", {"Art", "Type"},
                    {{"none", {"keine (nur Zustand 1)", "none (state 1 only)"}},
                     {"heat", {"Erwärmen (Heizregister)", "Heating (heating coil)"}},
                     {"cool", {"Kühlen / Entfeuchten (Kühlregister)", "Cooling / dehumidifying (cooling coil)"}},
                     {"steam", {"Dampfbefeuchten", "Steam humidifying"}},
                     {"adiabatic", {"Verdunstungsbefeuchten (adiabat)", "Evaporative humidifying (adiabatic)"}},
                     {"mix", {"Mischen mit Zustand B", "Mixing with state B"}}},
                    "cool")
            .startsSection({"Zustandsänderung 1 → 2", "Process 1 → 2"}),
        Num::number("t2", {"Zieltemperatur t₂", "Target temperature t₂"}, "°C", 14).range(-30, 80).showIf("proc", {"heat", "cool"}),
        Num::number("rhT", {"Ziel-Feuchte φ₂", "Target humidity φ₂"}, "%", 50).rangeExclusiveMin(0, 100)
            .showIf("proc", {"steam", "adiabatic"}),
        Num::number("tB", {"Temperatur Zustand B", "Temperature state B"}, "°C", 22).range(-30, 60).showIf("proc", {"mix"}),
        Num::number("rhB", {"Relative Feuchte Zustand B", "Relative humidity state B"}, "%", 40).rangeExclusiveMin(0, 100)
            .showIf("proc", {"mix"}),
        Num::number("shareB", {"Anteil B am Massenstrom", "Share of B in mass flow"}, "%", 70).range(0, 100).showIf("proc", {"mix"})
            .withHint({"z. B. Umluftanteil; Rest = Zustand 1", "e.g. recirculation share; rest = state 1"}),
        Num::number("V", {"Volumenstrom V̇ (bei Zustand 1)", "Airflow V̇ (at state 1)"}, "m³/h", 10000).positive()
            .showIf("proc", withPower),
    };
    c.outputs = {
        Out::number("p", {"Luftdruck p", "Air pressure p"}, "hPa", 1),
        Out::number("x1", {"Wassergehalt x₁", "Humidity ratio x₁"}, "g/kg", 2).primary().abbrev({"x₁", "x₁"}),
        Out::number("td1", {"Taupunkt τ₁", "Dew point τ₁"}, "°C", 1).primary().abbrev({"Taupunkt τ₁", "Dew point τ₁"}),
        Out::number("h1", {"Enthalpie h₁", "Enthalpy h₁"}, "kJ/kg", 1),
        Out::number("twb1", {"Kühlgrenztemperatur t_f,1", "Wet-bulb temperature t_wb,1"}, "°C", 1),
        Out::number("rho1", {"Dichte ρ₁", "Density ρ₁"}, "kg/m³", 3),
        Out::number("t2", {"Temperatur t₂", "Temperature t₂"}, "°C", 1),
        Out::number("rh2", {"Relative Feuchte φ₂", "Relative humidity φ₂"}, "%", 1),
        Out::number("x2", {"Wassergehalt x₂", "Humidity ratio x₂"}, "g/kg", 2),
        Out::number("h2", {"Enthalpie h₂", "Enthalpy h₂"}, "kJ/kg", 1),
        Out::number("td2", {"Taupunkt τ₂", "Dew point τ₂"}, "°C", 1),
        Out::number("dh", {"Enthalpiedifferenz Δh", "Enthalpy difference Δh"}, "kJ/kg", 2),
        Out::number("Q", {"Leistung Q (+ heizen / − kühlen)", "Power Q (+ heating / − cooling)"}, "kW", 1).primary().abbrev({"Q", "Q"}),
        Out::number("water", {"Wasser (+ Befeuchtung / − Kondensat)", "Water (+ humidifying / − condensate)"}, "kg/h", 1),
    };
    c.keywords = {"mollier", "h-x", "hx", "taupunkt", "feuchte", "enthalpie", "kühlgrenze", "entfeuchtung", "befeuchtung",
                  "mischluft", "rlt", "psychrometric", "dew point", "kondensat"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double p = air::pressureAtAltitude(in.num("alt"));
        const air::AirState s1{in.num("t1"), air::ratioFromRh(in.num("t1"), in.num("rh1"), p)};
        const double h1 = air::enthalpy(s1.t, s1.x);
        const double rho1 = air::density(s1.t, air::saturationPressure(s1.t) * in.num("rh1") / 100.0, p);
        r.set("p", p);
        r.set("x1", s1.x * 1000.0);
        r.set("td1", air::dewPointFromRatio(s1.x, p));
        r.set("h1", h1);
        r.set("twb1", air::wetBulb(s1.t, s1.x, p));
        r.set("rho1", rho1);

        const auto& proc = in.choice("proc");
        if (proc == "none") {
            r.info({"Oberflächen kälter als τ₁ = " + formatNumber(r.values["td1"], 1, Lang::De) + " °C betauen.",
                    "Surfaces colder than τ₁ = " + formatNumber(r.values["td1"], 1, Lang::En) + " °C will condense."});
            return r;
        }
        air::AirState s2 = s1;
        if (proc == "heat" || proc == "cool") {
            const double t2 = in.num("t2");
            if (proc == "heat" && t2 < s1.t) throw CalcError({"Zieltemperatur liegt unter t₁ – „Kühlen“ wählen.", "Target below t₁ – choose “Cooling”."});
            if (proc == "cool" && t2 > s1.t) throw CalcError({"Zieltemperatur liegt über t₁ – „Erwärmen“ wählen.", "Target above t₁ – choose “Heating”."});
            s2 = air::coolOrHeat(s1, t2, p);
            if (s2.x < s1.x) {
                r.info({"Kühlen unter den Taupunkt: Luft wird entfeuchtet (ideal gesättigt, reale Kühler mit Bypass-Faktor).",
                        "Cooling below the dew point: air is dehumidified (ideally saturated, real coils have a bypass factor)."});
            }
        } else if (proc == "steam" || proc == "adiabatic") {
            const double rhT = in.num("rhT");
            if (rhT <= in.num("rh1")) {
                r.warn({"Ziel-Feuchte ≤ φ₁ – keine Befeuchtung nötig.", "Target humidity ≤ φ₁ – no humidification needed."});
            }
            if (proc == "adiabatic" && rhT > 95) {
                r.warn({"Verdunstungsbefeuchter erreichen meist nur 90–95 % r. F.", "Evaporative humidifiers usually reach only 90–95 % RH."});
            }
            s2 = proc == "steam" ? air::steamHumidify(s1, rhT, p) : air::adiabaticHumidify(s1, rhT, p);
        } else if (proc == "mix") {
            const air::AirState b{in.num("tB"), air::ratioFromRh(in.num("tB"), in.num("rhB"), p)};
            s2 = air::mix(s1, b, in.num("shareB") / 100.0);
            r.set("_tB", b.t);
            r.set("_xB", b.x);
        }
        const double rh2 = air::rhFromRatio(s2.t, s2.x, p);
        const double h2 = air::enthalpy(s2.t, s2.x);
        r.set("t2", s2.t);
        r.set("rh2", std::min(rh2, 100.0));
        r.set("x2", s2.x * 1000.0);
        r.set("h2", h2);
        r.set("td2", air::dewPointFromRatio(s2.x, p));
        r.set("dh", h2 - h1);
        if (proc == "mix") {
            if (rh2 > 100.0 + 1e-6) {
                r.warn({"Mischpunkt liegt im Nebelgebiet (übersättigt) – vorwärmen.", "Mixed state lies in the fog region (supersaturated) – preheat."});
            }
            return r;
        }
        const double m = in.num("V") * rho1 / 3600.0;  // kg/s
        r.set("Q", m * (h2 - h1));
        r.set("water", m * 3600.0 * (s2.x - s1.x));
        return r;
    };
    c.chart = [](const Inputs& in, const Result& r, const Context&) {
        const double p = air::pressureAtAltitude(in.num("alt"));
        std::vector<StatePoint> pts;
        const auto& proc = in.choice("proc");
        if (!r.hasError()) {
            pts.push_back({"1", in.num("t1"), r.values.at("x1") / 1000.0});
            if (proc != "none") pts.push_back({proc == "mix" ? "M" : "2", r.values.at("t2"), r.values.at("x2") / 1000.0});
            if (proc == "mix") pts.push_back({"B", r.values.at("_tB"), r.values.at("_xB")});
        }
        Chart ch = hxGrid(p, pts);
        if (pts.size() >= 2) {
            ChartSeries path{"process", {}};
            const auto& a = pts[0];
            const auto& b = pts[1];
            const auto add = [&](double t, double x) { path.pts.push_back({x * 1000.0, air::mollierY(t, x)}); };
            if (proc == "mix") {
                const auto& bb = pts[2];
                add(a.t, a.x);
                add(bb.t, bb.x);
            } else if (proc == "cool" && b.x < a.x) {
                // 1 → dew point (x const) → along the saturation curve down to t2
                const double td = air::dewPointFromRatio(a.x, p);
                add(a.t, a.x);
                for (double t = td; t > b.t; t -= 0.5) add(t, air::saturationRatio(t, p));
                add(b.t, b.x);
            } else {
                add(a.t, a.x);
                add(b.t, b.x);
            }
            ch.series.push_back(std::move(path));
        }
        addStates(ch, pts, p, true);
        ch.note = pressureNote(p, in.num("alt"));
        return ch;
    };
    c.pick = [](const Inputs& in, double x, double y, const Context&) {
        const auto s = pickState(x, y, air::pressureAtAltitude(in.num("alt")));
        return std::vector<PickValue>{{"t1", s.first, {}}, {"rh1", s.second, {}}};
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
        "CO₂-Bilanz (stationär): V̇ = n · G / (C_Soll − C_außen), G ≈ 20 l/h je Person bei Büroarbeit.\n"
        "Außenluft ≈ 400 ppm, in Städten oft 550 … 700 ppm (Siemens „Regeln und Steuern von Lüftungsanlagen“, 10/2022, Kap. 3.3.1); "
        "bedarfsgeregelte Lüftung über CO₂/VOC siehe VDMA 24773.",
        "EN 16798-1 (Annex B, method 1): q = n · q_P + A · q_B\n"
        "q_P per person: cat. I 10, II 7, III 4 l/s.   q_B per m² (low-polluting building): I 1.0, II 0.7, III 0.4 l/(s·m²); "
        "very low-polluting: 0.5 / 0.35 / 0.2; non-low-polluting: 2.0 / 1.4 / 0.8.\n"
        "CO₂ balance (steady state): V̇ = n · G / (C_set − C_outdoor), G ≈ 20 l/h per person for office work.\n"
        "Outdoor air ≈ 400 ppm, in cities often 550 … 700 ppm (Siemens “Regeln und Steuern von Lüftungsanlagen”, 10/2022, ch. 3.3.1); "
        "demand-controlled ventilation via CO₂/VOC see VDMA 24773."};
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
    out.push_back(hxDiagram());
    out.push_back(humidAir());
    out.push_back(freshAir());
}

}  // namespace elc::catalog
