// Area "Elektrotechnik" (v1.7): motor power/voltage/power factor, NEC full-load currents, connection diagrams for
// three-phase motors with 6 (IEC), 9 and 12 (NEMA MG 1) leads and motor connections / direction of rotation.
#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <string>
#include <tuple>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"
#include "elc/formulas/electrical.hpp"

namespace elc::catalog {

using namespace elc::electrical;

namespace {

constexpr const char* kCat = "electrical";
using P = Sketch::P;

std::vector<Option> acOptions() {
    return {{"ac1", {"Wechselstrom 1~", "Single-phase AC"}}, {"ac3", {"Drehstrom 3~", "Three-phase AC"}}};
}

// ------------------------------------------------------------------ power, voltage, power factor

enum class Solve { Power, Voltage, PowerFactor };

Calculator motorValues(Solve what) {
    Calculator c;
    c.category = kCat;
    const Text formula{"P_ab = k · U · I · cos φ · η    k = 1 (1~), √3 (3~); P_auf = P_ab / η; 1 hp = 745,7 W.",
                       "P_out = k · U · I · cos φ · η    k = 1 (1~), √3 (3~); P_in = P_out / η; 1 hp = 745.7 W."};
    c.description = formula;
    std::vector<InputDef> defs{Num::choice("sys", {"Netz", "System"}, acOptions(), "ac3")};
    if (what != Solve::Voltage) defs.push_back(Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive());
    if (what != Solve::Power) defs.push_back(Num::number("P", {"Abgegebene Leistung P_ab", "Output power P_out"}, "kW", 7.5).positive());
    defs.push_back(Num::number("I", {"Strom I", "Current I"}, "A", 14.5).positive());
    if (what != Solve::PowerFactor)
        defs.push_back(Num::number("cosphi", {"Leistungsfaktor cos φ", "Power factor cos φ"}, "", 0.84).rangeExclusiveMin(0, 1));
    defs.push_back(Num::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 90).rangeExclusiveMin(0, 100));
    c.inputs = defs;
    switch (what) {
        case Solve::Power:
            c.id = "motor_power";
            c.title = {"Motorleistung", "Motor power"};
            c.summary = {"Abgegebene und aufgenommene Leistung aus U, I, cos φ, η", "Output and input power from U, I, cos φ, η"};
            c.outputs = {Out::number("Pout", {"Abgegebene Leistung P_ab", "Output power P_out"}, "kW", 2).primary(),
                         Out::number("hp", {"Abgegebene Leistung", "Output power"}, "hp", 2),
                         Out::number("Pin", {"Aufgenommene Leistung P_auf", "Input power P_in"}, "kW", 2),
                         Out::number("S", {"Scheinleistung S", "Apparent power S"}, "kVA", 2),
                         Out::number("Ploss", {"Verluste", "Losses"}, "kW", 2)};
            c.keywords = {"motorleistung", "motor power", "kw", "hp", "ps"};
            break;
        case Solve::Voltage:
            c.id = "motor_voltage";
            c.title = {"Motorspannung", "Motor voltage"};
            c.summary = {"Spannung aus Leistung, Strom, cos φ und η", "Voltage from power, current, cos φ and η"};
            c.outputs = {Out::number("U", {"Spannung U", "Voltage U"}, "V", 1).primary(),
                         Out::number("Pin", {"Aufgenommene Leistung P_auf", "Input power P_in"}, "kW", 2)};
            c.keywords = {"motorspannung", "motor voltage"};
            break;
        case Solve::PowerFactor:
            c.id = "motor_pf";
            c.title = {"Leistungsfaktor (Motor)", "Power factor (motor)"};
            c.summary = {"cos φ aus Leistung, Spannung, Strom und η", "cos φ from power, voltage, current and η"};
            c.outputs = {Out::number("cosphi", {"Leistungsfaktor cos φ", "Power factor cos φ"}, "", 3).primary(),
                         Out::number("phi", {"Phasenwinkel φ", "Phase angle φ"}, "°", 1),
                         Out::number("Pin", {"Aufgenommene Leistung P_auf", "Input power P_in"}, "kW", 2),
                         Out::number("Q", {"Blindleistung Q", "Reactive power Q"}, "kvar", 2)};
            c.keywords = {"cos phi", "leistungsfaktor", "motor power factor"};
            break;
    }
    c.compute = [what](const Inputs& in, const Context&) {
        Result r;
        const double k = powerFactorK(parseSystem(in.choice("sys")));
        const double eta = in.num("eta") / 100.0;
        switch (what) {
            case Solve::Power: {
                const double S = k * in.num("U") * in.num("I");
                const double Pin = S * in.num("cosphi");
                r.set("S", S / 1e3);
                r.set("Pin", Pin / 1e3);
                r.set("Pout", Pin * eta / 1e3);
                r.set("hp", Pin * eta / kWattPerHp);
                r.set("Ploss", Pin * (1 - eta) / 1e3);
                break;
            }
            case Solve::Voltage: {
                const double Pin = in.num("P") * 1e3 / eta;
                r.set("Pin", Pin / 1e3);
                r.set("U", Pin / (k * in.num("I") * in.num("cosphi")));
                break;
            }
            case Solve::PowerFactor: {
                const double Pin = in.num("P") * 1e3 / eta;
                const double S = k * in.num("U") * in.num("I");
                const double cp = Pin / S;
                if (cp > 1.0 + 1e-9)
                    throw CalcError({"cos φ > 1 – Leistung, Strom oder Wirkungsgrad prüfen.", "cos φ > 1 – check power, current or efficiency."});
                r.set("cosphi", cp);
                r.set("phi", std::acos(std::min(1.0, cp)) * 180.0 / std::numbers::pi);
                r.set("Pin", Pin / 1e3);
                r.set("Q", std::sqrt(std::max(0.0, S * S - Pin * Pin)) / 1e3);
                break;
            }
        }
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ NEC full-load current

std::string hpText(double hp) {
    if (std::fabs(hp - 1.0 / 6.0) < 1e-6) return "1/6";
    if (std::fabs(hp - 0.25) < 1e-6) return "1/4";
    if (std::fabs(hp - 1.0 / 3.0) < 1e-6) return "1/3";
    if (std::fabs(hp - 0.5) < 1e-6) return "1/2";
    if (std::fabs(hp - 0.75) < 1e-6) return "3/4";
    if (std::fabs(hp - 1.5) < 1e-6) return "1 1/2";
    if (std::fabs(hp - 7.5) < 1e-6) return "7 1/2";
    return formatNumber(hp, 0, Lang::En);
}

std::vector<Option> hpOptions(int phases) {
    std::vector<Option> o;
    for (const double hp : necHorsepowers(phases)) {
        const std::string t = hpText(hp) + " hp (" + formatNumber(hp * kWattPerHp / 1e3, 2, Lang::De) + " kW)";
        const std::string te = hpText(hp) + " hp (" + formatNumber(hp * kWattPerHp / 1e3, 2, Lang::En) + " kW)";
        o.push_back({formatNumber(hp, 6, Lang::En), {t, te}});
    }
    return o;
}

std::vector<Option> voltOptions(int phases) {
    std::vector<Option> o;
    for (const int v : necVoltages(phases)) o.push_back({std::to_string(v), {std::to_string(v) + " V", std::to_string(v) + " V"}});
    return o;
}

Calculator necFlc() {
    Calculator c;
    c.id = "nec_flc";
    c.category = kCat;
    c.title = {"Motor-Volllaststrom (NEC)", "Motor full-load current (NEC)"};
    c.summary = {"Tabellenstrom nach NEC 430.248 / 430.250 (Nordamerika)", "Table current per NEC 430.248 / 430.250 (North America)"};
    c.description = {
        "NEC (NFPA 70) Tab. 430.248 (Einphasen-Wechselstrommotoren) und Tab. 430.250 (Drehstrom-Asynchronmotoren). "
        "Nach NEC 430.6(A)(1) sind für Leiter, Kurzschluss- und Erdschlussschutz die Tabellenwerte (nicht die Typenschildwerte) "
        "zu verwenden; Leiter für einen Motor ≥ 125 % des Tabellenstroms (430.22). Der Motorschutz (Überlast) richtet sich nach "
        "dem Typenschildstrom. 208/200 V = Werte für 208/200-V-Motoren. Gegen die gültige NEC-Ausgabe prüfen.",
        "NEC (NFPA 70) Table 430.248 (single-phase AC motors) and Table 430.250 (three-phase induction motors). "
        "Per NEC 430.6(A)(1) the table values (not nameplate) are used for conductors and short-circuit/ground-fault protection; "
        "conductors for one motor ≥ 125 % of the table current (430.22). Overload protection uses the nameplate current. "
        "Check against the NEC edition in force."};
    c.inputs = {Num::choice("ph", {"Phasen", "Phases"}, {{"3", {"Drehstrom (430.250)", "Three-phase (430.250)"}},
                                                         {"1", {"Einphasen (430.248)", "Single-phase (430.248)"}}}, "3"),
                Num::choice("hp3", {"Motorleistung", "Motor rating"}, hpOptions(3), "10"),
                Num::choice("hp1", {"Motorleistung", "Motor rating"}, hpOptions(1), "1"),
                Num::choice("v3", {"Spannung", "Voltage"}, voltOptions(3), "460"),
                Num::choice("v1", {"Spannung", "Voltage"}, voltOptions(1), "230")};
    c.inputs[1].showIf("ph", {"3"});
    c.inputs[2].showIf("ph", {"1"});
    c.inputs[3].showIf("ph", {"3"});
    c.inputs[4].showIf("ph", {"1"});
    c.outputs = {Out::number("flc", {"Volllaststrom (Tabelle)", "Full-load current (table)"}, "A", 1).primary(),
                 Out::number("cond", {"Leiter-Bemessungsstrom 125 %", "Conductor design current 125 %"}, "A", 1),
                 Out::number("kw", {"Leistung", "Power"}, "kW", 2)};
    c.keywords = {"nec", "flc", "fla", "full load", "430.250", "hp", "amerika", "usa"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const bool three = in.choice("ph") == "3";
        const double hp = parseDouble(in.choice(three ? "hp3" : "hp1"));
        const int v = static_cast<int>(parseDouble(in.choice(three ? "v3" : "v1")));
        r.set("kw", hp * kWattPerHp / 1e3);
        const auto flc = necFullLoadCurrent(three ? 3 : 1, hp, v);
        if (!flc) throw CalcError({"Für diese Kombination enthält die NEC-Tabelle keinen Wert.", "The NEC table has no value for this combination."});
        r.set("flc", *flc);
        r.set("cond", *flc * 1.25);
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ winding diagrams

struct Coil {
    const char* a;  // end towards the line terminal (star) or towards the side start (delta)
    const char* b;
};

struct Winding {
    bool delta = false;
    bool parallel = false;
    std::array<std::vector<Coil>, 3> groups;  // star: arms L1/L2/L3; delta: sides L1→L2, L2→L3, L3→L1
};

P polar(P c, double r, double deg) {
    const double a = deg * std::numbers::pi / 180.0;
    return {c[0] + r * std::cos(a), c[1] + r * std::sin(a)};
}

/// unit normal of a → b pointing away from `away`
P outward(P a, P b, P away) {
    const double dx = b[0] - a[0], dy = b[1] - a[1];
    const double l = std::hypot(dx, dy);
    P n{-dy / l, dx / l};
    const P mid{(a[0] + b[0]) / 2, (a[1] + b[1]) / 2};
    if ((mid[0] - away[0]) * n[0] + (mid[1] - away[1]) * n[1] < 0) n = {-n[0], -n[1]};
    return n;
}

P shift(P p, P n, double d) { return {p[0] + n[0] * d, p[1] + n[1] * d}; }

/// coil body between p and q with end labels (offset `lab` along n)
void coil(Sketch& s, P p, P q, P n, double lab, const Coil& cl) {
    s.bar("bgfill", Sketch::along(p, q, 0.12), Sketch::along(p, q, 0.88), 3.4);
    if (lab == 0.0) return;
    // labels beside both ends of the coil body (not beyond them: keeps corners and the star point readable)
    const P la = shift(Sketch::along(p, q, 0.2), n, lab);
    const P lb = shift(Sketch::along(p, q, 0.8), n, lab);
    s.label(la[0], la[1], cl.a);
    s.label(lb[0], lb[1], cl.b);
}

void dot(Sketch& s, P p) { s.circle("inkfill", p[0], p[1], 0.9, 12); }

/// Parallel coils: ends that meet at the same node share one label outside ("T1, T7")
void pairLabels(Sketch& s, P a, P b, P n, const std::vector<Coil>& g) {
    std::string start, end;
    for (const auto& cl : g) {
        if (*cl.a) start += (start.empty() ? "" : ", ") + std::string(cl.a);
        if (*cl.b) end += (end.empty() ? "" : ", ") + std::string(cl.b);
    }
    const P la = shift(Sketch::along(a, b, 0.2), n, 8.6);
    const P lb = shift(Sketch::along(a, b, 0.8), n, 8.6);
    s.label(la[0], la[1], Text{start, start});
    s.label(lb[0], lb[1], Text{end, end});
}

/// Line terminal with label "L1"… beyond point p in direction n
void lineTerminal(Sketch& s, P p, P n, const char* name) {
    const P t = shift(p, n, 5.5);
    s.line("ink", {p, t});
    s.circle("bgfill", t[0], t[1], 1.6, 16);
    s.label(shift(t, n, 4.5)[0], shift(t, n, 4.5)[1], name, "cb");
}

void drawWinding(Sketch& s, const Winding& w, P c, double R) {
    static const std::array<const char*, 3> lines{"L1", "L2", "L3"};
    static const std::array<double, 3> angles{90, 210, 330};
    if (!w.delta) {
        for (std::size_t i = 0; i < 3; ++i) {
            const P end = polar(c, R, angles[i]);
            const P n = outward(c, end, polar(c, R, angles[i] + 90));  // any perpendicular
            const auto& g = w.groups[i];
            if (!w.parallel) {
                s.line("ink", {c, end});
                const P mid = Sketch::along(end, c, 0.42);
                if (g.size() == 1) coil(s, Sketch::along(end, c, 0.04), Sketch::along(end, c, 0.72), n, 5.0, g[0]);
                else coil(s, end, Sketch::along(end, c, 0.40), n, 5.0, g[0]);
                if (g.size() > 1) {
                    coil(s, Sketch::along(end, c, 0.44), Sketch::along(end, c, 0.80), n, -5.0, g[1]);
                    dot(s, mid);
                }
            } else {
                const P outer = Sketch::along(end, c, 0.06);
                const P inner = Sketch::along(end, c, 0.74);
                s.line("ink", {end, outer});
                s.line("ink", {inner, c});
                s.line("ink", {shift(outer, n, 2.6), shift(outer, n, -2.6)});
                s.line("ink", {shift(inner, n, 2.6), shift(inner, n, -2.6)});
                for (std::size_t k = 0; k < g.size(); ++k) {
                    const double off = k == 0 ? 2.6 : -2.6;
                    const P a = shift(outer, n, off), b = shift(inner, n, off);
                    s.line("ink", {a, b});
                    coil(s, a, b, n, 0.0, g[k]);
                }
                pairLabels(s, outer, inner, n, g);
            }
            lineTerminal(s, end, polar({0, 0}, 1, angles[i]), lines[i]);
        }
        dot(s, c);
        return;
    }
    std::array<P, 3> v{polar(c, R, 90), polar(c, R, 210), polar(c, R, 330)};
    for (std::size_t i = 0; i < 3; ++i) {
        const P a = v[i], b = v[(i + 1) % 3];
        const P n = outward(a, b, c);
        const auto& g = w.groups[i];
        if (!w.parallel) {
            s.line("ink", {a, b});
            if (g.size() == 1) coil(s, Sketch::along(a, b, 0.1), Sketch::along(a, b, 0.9), n, 5.0, g[0]);
            else coil(s, Sketch::along(a, b, 0.06), Sketch::along(a, b, 0.47), n, 5.0, g[0]);
            if (g.size() > 1) {
                coil(s, Sketch::along(a, b, 0.53), Sketch::along(a, b, 0.94), n, 5.0, g[1]);
                dot(s, Sketch::along(a, b, 0.5));
            }
        } else {
            const P a1 = Sketch::along(a, b, 0.2), b1 = Sketch::along(a, b, 0.8);
            s.line("ink", {a, a1});
            s.line("ink", {b1, b});
            s.line("ink", {shift(a1, n, 2.6), shift(a1, n, -2.6)});
            s.line("ink", {shift(b1, n, 2.6), shift(b1, n, -2.6)});
            for (std::size_t k = 0; k < g.size(); ++k) {
                const double off = k == 0 ? 2.6 : -2.6;
                const P p = shift(a1, n, off), q = shift(b1, n, off);
                s.line("ink", {p, q});
                coil(s, p, q, n, 0.0, g[k]);
            }
            pairLabels(s, a1, b1, n, g);
        }
    }
    for (std::size_t i = 0; i < 3; ++i) {
        dot(s, v[i]);
        lineTerminal(s, v[i], polar({0, 0}, 1, angles[i]), lines[i]);
    }
}

// ------------------------------------------------------------------ 6 terminals (IEC 60034-8)

Calculator wiring6() {
    Calculator c;
    c.id = "motor_wiring_6";
    c.category = kCat;
    c.title = {"Schaltschema Drehstrommotor (6 Klemmen)", "Three-phase motor connection (6 terminals)"};
    c.summary = {"Stern- oder Dreieckschaltung am Klemmbrett (U1…W2)", "Star or delta connection on the terminal board (U1…W2)"};
    c.description = {
        "Klemmenbezeichnung nach IEC 60034-8 (alt DIN VDE 0530: U, V, W / X, Y, Z). Oben W2 U2 V2, unten U1 V1 W1.\n"
        "Stern (Y): Brücke W2–U2–V2, Netz an U1/V1/W1; jede Wicklung liegt an U/√3.\n"
        "Dreieck (Δ): Brücken W2–U1, U2–V1, V2–W1; jede Wicklung liegt an U.\n"
        "Typenschild 230/400 V am 400-V-Netz → Stern; 400/690 V am 400-V-Netz → Dreieck (auch für Stern-Dreieck-Anlauf).\n"
        "L1→U1, L2→V1, L3→W1 = Rechtslauf (Blick auf das Wellenende).",
        "Terminal marking per IEC 60034-8 (old DIN VDE 0530: U, V, W / X, Y, Z). Top W2 U2 V2, bottom U1 V1 W1.\n"
        "Star (Y): bridge W2–U2–V2, supply at U1/V1/W1; each winding sees U/√3.\n"
        "Delta (Δ): bridges W2–U1, U2–V1, V2–W1; each winding sees U.\n"
        "Rating plate 230/400 V on a 400 V system → star; 400/690 V on a 400 V system → delta (also for star-delta starting).\n"
        "L1→U1, L2→V1, L3→W1 = clockwise (looking at the shaft end)."};
    c.inputs = {Num::number("Un", {"Netzspannung (Außenleiter)", "System voltage (line)"}, "V", 400).positive(),
                Num::choice("plate", {"Typenschild", "Rating plate"},
                            {{"230/400", "230/400 V Δ/Y"}, {"400/690", "400/690 V Δ/Y"}, {"500/865", "500/865 V Δ/Y"},
                             {"220/380", "220/380 V Δ/Y"}, {"240/415", "240/415 V Δ/Y"}},
                            "230/400"),
                Num::choice("conn", {"Darstellung", "Show"},
                            {{"auto", {"Passende Schaltung", "Correct connection"}}, {"star", {"Stern (Y)", "Star (Y)"}},
                             {"delta", {"Dreieck (Δ)", "Delta (Δ)"}}},
                            "auto")};
    c.outputs = {Out::text("conn", {"Schaltung", "Connection"}).primary(),
                 Out::text("bridges", {"Brücken", "Bridges"}),
                 Out::text("supply", {"Netzanschluss", "Supply"}),
                 Out::number("Uw", {"Spannung je Wicklung", "Voltage per winding"}, "V", 0)};
    c.keywords = {"stern", "dreieck", "klemmbrett", "klemmenkasten", "u1 v1 w1", "star delta", "terminal board", "wiring"};
    auto chosen = [](const Inputs& in) {
        const auto& plate = in.choice("plate");
        const double low = parseDouble(plate.substr(0, plate.find('/')));
        const double high = parseDouble(plate.substr(plate.find('/') + 1));
        const double U = in.num("Un");
        std::string conn = in.choice("conn");
        if (conn == "auto") conn = std::fabs(U - high) <= std::fabs(U - low) ? "star" : "delta";
        return std::tuple{conn, low, high};
    };
    c.compute = [chosen](const Inputs& in, const Context&) {
        Result r;
        const auto [conn, low, high] = chosen(in);
        const double U = in.num("Un");
        const bool star = conn == "star";
        r.setText("conn", star ? Text{"Stern (Y)", "Star (Y)"} : Text{"Dreieck (Δ)", "Delta (Δ)"});
        r.setText("bridges", star ? Text{"W2 – U2 – V2 (eine Brücke über die obere Reihe)", "W2 – U2 – V2 (one bridge across the top row)"}
                                  : Text{"W2–U1, U2–V1, V2–W1 (drei senkrechte Brücken)", "W2–U1, U2–V1, V2–W1 (three vertical bridges)"});
        r.setText("supply", {"L1 → U1, L2 → V1, L3 → W1", "L1 → U1, L2 → V1, L3 → W1"});
        const double Uw = star ? U / kSqrt3 : U;
        r.set("Uw", Uw);
        const double rated = low;  // winding voltage = lower plate value
        if (std::fabs(Uw - rated) > 0.1 * rated)
            r.error(cat(Text{"Wicklung erhält ", "Winding receives "}, numText(Uw, 0, "V"), Text{", ausgelegt für ", ", rated for "},
                        numText(rated, 0, "V"), Text{" – falsche Schaltung für dieses Netz!", " – wrong connection for this system!"}));
        else
            r.ok({"Wicklungsspannung passt zum Typenschild.", "Winding voltage matches the rating plate."});
        if (!star && std::fabs(U - low) < 0.1 * low && high > 600)
            r.info({"Dreieck im Betrieb → Stern-Dreieck-Anlauf möglich.", "Delta in operation → star-delta starting possible."});
        return r;
    };
    c.chart = [chosen](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 104, 1.04);
        const bool star = std::get<0>(chosen(in)) == "star";
        // terminal board
        s.rect("ink", 20, 64, 80, 98);
        const std::array<double, 3> xs{32, 50, 68};
        const std::array<const char*, 3> top{"W2", "U2", "V2"};
        const std::array<const char*, 3> bottom{"U1", "V1", "W1"};
        const std::array<const char*, 3> lines{"L1", "L2", "L3"};
        for (std::size_t i = 0; i < 3; ++i) {
            s.line("ink", {{xs[i], 60}, {xs[i], 74}});
            s.label(xs[i], 56.5, lines[i], "cb");
        }
        for (std::size_t i = 0; i < 3; ++i) {
            s.circle("bgfill", xs[i], 90, 2.6, 20);
            s.circle("bgfill", xs[i], 74, 2.6, 20);
            s.label(xs[i], 95.6, top[i]);
            s.label(xs[i] + 6.5, 74, bottom[i], "l");
        }
        if (star) {
            s.line("line:#B87333", {{xs[0], 90}, {xs[2], 90}});
        } else {
            for (const double x : xs) s.line("line:#B87333", {{x, 90}, {x, 74}});
        }
        Winding w;
        w.delta = !star;
        w.groups = star ? std::array<std::vector<Coil>, 3>{{{{"U1", "U2"}}, {{"V1", "V2"}}, {{"W1", "W2"}}}}
                        : std::array<std::vector<Coil>, 3>{{{{"U1", "U2"}}, {{"V1", "V2"}}, {{"W1", "W2"}}}};
        drawWinding(s, w, {50, 22}, 17);
        s.ch.note = {"Oben: Klemmbrett mit Brücken (Kupfer). Unten: Wicklungen in der gewählten Schaltung.",
                     "Top: terminal board with bridges (copper). Bottom: windings in the chosen connection."};
        return s.ch;
    };
    return c;
}

// ------------------------------------------------------------------ 9 and 12 leads (NEMA MG 1)

Calculator wiringNema(int leads) {
    Calculator c;
    c.id = leads == 9 ? "motor_wiring_9" : "motor_wiring_12";
    c.category = kCat;
    c.title = leads == 9 ? Text{"Schaltschema Drehstrommotor (9 Klemmen)", "Three-phase motor connection (9 leads)"}
                         : Text{"Schaltschema Drehstrommotor (12 Klemmen)", "Three-phase motor connection (12 leads)"};
    c.summary = leads == 9 ? Text{"NEMA T1–T9, Stern oder Dreieck, hohe/niedrige Spannung", "NEMA T1–T9, wye or delta, high/low voltage"}
                           : Text{"NEMA T1–T12, Stern/Dreieck, Reihe/parallel", "NEMA T1–T12, wye/delta, series/parallel"};
    c.description = {
        "Anschlussbezeichnung nach NEMA MG 1 (Nordamerika), Doppelspannungsmotoren z. B. 230/460 V: Hohe Spannung = "
        "Wicklungshälften in Reihe, niedrige Spannung = parallel.\n"
        "9 Klemmen: Stern (innerer Sternpunkt T7-T8-T9 intern) oder Dreieck (innere Dreieckecken T1, T2, T3).\n"
        "12 Klemmen: alle Wicklungsenden herausgeführt; Stern oder Dreieck frei wählbar (auch Stern-Dreieck-Anlauf, "
        "Teilwicklungsanlauf). Typenschild und Anschlussplan des Herstellers haben Vorrang.",
        "Lead marking per NEMA MG 1 (North America), dual-voltage motors e.g. 230/460 V: high voltage = winding halves in "
        "series, low voltage = parallel.\n"
        "9 leads: wye (internal star point of T7-T8-T9) or delta (internal delta corners T1, T2, T3).\n"
        "12 leads: all winding ends brought out; wye or delta selectable (also wye-delta or part-winding starting). "
        "The manufacturer's nameplate and connection diagram take precedence."};
    c.inputs = {Num::choice("conn", {"Schaltung", "Connection"},
                            {{"wye", {"Stern (Y)", "Wye (Y)"}}, {"delta", {"Dreieck (Δ)", "Delta (Δ)"}}}, "wye"),
                Num::choice("volt", {"Spannung", "Voltage"},
                            {{"high", {"Hohe Spannung (Reihe), z. B. 460 V", "High voltage (series), e.g. 460 V"}},
                             {"low", {"Niedrige Spannung (parallel), z. B. 230 V", "Low voltage (parallel), e.g. 230 V"}}},
                            "high")};
    c.inputs[0].withHint(leads == 9 ? Text{"Bauart des Motors (siehe Typenschild)", "Motor design (see nameplate)"}
                                    : Text{"Gewünschte Schaltung", "Desired connection"});
    c.outputs = {Out::text("L1", {"L1", "L1"}).primary(), Out::text("L2", {"L2", "L2"}).primary(), Out::text("L3", {"L3", "L3"}).primary(),
                 Out::text("ties", {"Zusammen verbinden (isoliert)", "Tie together (insulated)"})};
    c.keywords = {"nema", "9 leads", "12 leads", "t1", "dual voltage", "wye", "delta", "klemmen"};
    struct Plan {
        const char* l1;
        const char* l2;
        const char* l3;
        Text ties;
    };
    auto plan = [leads](const Inputs& in) -> Plan {
        const bool wye = in.choice("conn") == "wye";
        const bool high = in.choice("volt") == "high";
        if (leads == 9) {
            if (wye && high) return {"T1", "T2", "T3", {"T4–T7, T5–T8, T6–T9", "T4–T7, T5–T8, T6–T9"}};
            if (wye) return {"T1, T7", "T2, T8", "T3, T9", {"T4–T5–T6", "T4–T5–T6"}};
            if (high) return {"T1", "T2", "T3", {"T4–T7, T5–T8, T6–T9", "T4–T7, T5–T8, T6–T9"}};
            return {"T1, T6, T7", "T2, T4, T8", "T3, T5, T9", {"keine", "none"}};
        }
        if (wye && high) return {"T1", "T2", "T3", {"T4–T7, T5–T8, T6–T9, T10–T11–T12", "T4–T7, T5–T8, T6–T9, T10–T11–T12"}};
        if (wye) return {"T1, T7", "T2, T8", "T3, T9", {"T4–T5–T6, T10–T11–T12", "T4–T5–T6, T10–T11–T12"}};
        if (high) return {"T1, T12", "T2, T10", "T3, T11", {"T4–T7, T5–T8, T6–T9", "T4–T7, T5–T8, T6–T9"}};
        return {"T1, T6, T7, T12", "T2, T4, T8, T10", "T3, T5, T9, T11", {"keine", "none"}};
    };
    c.compute = [plan](const Inputs& in, const Context&) {
        Result r;
        const auto p = plan(in);
        r.setText("L1", {p.l1, p.l1});
        r.setText("L2", {p.l2, p.l2});
        r.setText("L3", {p.l3, p.l3});
        r.setText("ties", p.ties);
        r.info({"Verbindungen isolieren; nicht verwendete Enden einzeln isolieren.", "Insulate all ties; insulate unused leads individually."});
        return r;
    };
    c.chart = [leads](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 86, 0.86);
        const bool wye = in.choice("conn") == "wye";
        const bool high = in.choice("volt") == "high";
        Winding w;
        w.delta = !wye;
        w.parallel = !high;
        if (wye) {
            // arm i: outer coil T(i+1)–T(i+4), inner coil T(i+7)–(T(i+10) or internal star)
            const char* star9 = "";
            w.groups = {{{{"T1", "T4"}, {"T7", leads == 12 ? "T10" : star9}},
                         {{"T2", "T5"}, {"T8", leads == 12 ? "T11" : star9}},
                         {{"T3", "T6"}, {"T9", leads == 12 ? "T12" : star9}}}};
        } else if (leads == 9) {
            // internal delta: coils T7→T2, T8→T3, T9→T1
            w.groups = {{{{"T1", "T4"}, {"T7", "T2"}}, {{"T2", "T5"}, {"T8", "T3"}}, {{"T3", "T6"}, {"T9", "T1"}}}};
        } else {
            w.groups = {{{{"T1", "T4"}, {"T7", "T10"}}, {{"T2", "T5"}, {"T8", "T11"}}, {{"T3", "T6"}, {"T9", "T12"}}}};
        }
        drawWinding(s, w, {50, 40}, 27);
        s.ch.note = wye ? Text{"Wicklungshälften je Strang; Punkt = Verbindung (Brücke bzw. Sternpunkt).",
                               "Winding halves per phase; dot = connection (tie or star point)."}
                        : Text{"Wicklungshälften je Dreieckseite; Punkt = Verbindung.", "Winding halves per delta side; dot = connection."};
        return s.ch;
    };
    return c;
}

// ------------------------------------------------------------------ motor connections / direction of rotation

Calculator motorConnections() {
    Calculator c;
    c.id = "motor_connections";
    c.category = kCat;
    c.title = {"Motor-Verbindungen (Drehrichtung)", "Motor connections (direction of rotation)"};
    c.summary = {"Anschluss und Drehrichtungsumkehr von Drehstrom- und Kondensatormotoren",
                 "Connection and reversing of three-phase and capacitor motors"};
    c.description = {
        "Drehstrom (IEC 60034-8): L1→U1, L2→V1, L3→W1 ergibt Rechtslauf (Blick auf das Wellenende, Rechtsdrehfeld). "
        "Linkslauf: zwei Außenleiter tauschen (z. B. L1↔L2).\n"
        "Einphasen-Kondensatormotor: Hauptwicklung U1–U2 an L–N, Hilfswicklung Z1–Z2 über den Betriebskondensator C_B parallel. "
        "Drehrichtung umkehren: Hilfswicklung umpolen (Z1↔Z2) – die tatsächliche Richtung zeigt das Anschlussbild des Herstellers.",
        "Three-phase (IEC 60034-8): L1→U1, L2→V1, L3→W1 gives clockwise rotation (looking at the shaft end, clockwise field). "
        "Counter-clockwise: swap two lines (e.g. L1↔L2).\n"
        "Single-phase capacitor motor: main winding U1–U2 across L–N, auxiliary winding Z1–Z2 in parallel via the run capacitor C_B. "
        "Reversing: swap the auxiliary winding (Z1↔Z2) – the actual direction is given by the manufacturer's diagram."};
    c.inputs = {Num::choice("type", {"Motor", "Motor"}, {{"3ph", {"Drehstrommotor", "Three-phase motor"}},
                                                         {"1ph", {"Einphasen-Kondensatormotor", "Single-phase capacitor motor"}}}, "3ph"),
                Num::choice("dir", {"Drehrichtung", "Direction"}, {{"cw", {"Rechtslauf / Richtung 1", "Clockwise / direction 1"}},
                                                                   {"ccw", {"Linkslauf / Richtung 2", "Counter-clockwise / direction 2"}}}, "cw")};
    c.outputs = {Out::text("conn", {"Anschluss", "Connection"}).primary(), Out::text("how", {"Umkehr", "Reversing"})};
    c.keywords = {"drehrichtung", "rechtslauf", "linkslauf", "wendeschaltung", "kondensatormotor", "rotation", "reversing"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const bool cw = in.choice("dir") == "cw";
        if (in.choice("type") == "3ph") {
            r.setText("conn", cw ? Text{"L1 → U1, L2 → V1, L3 → W1", "L1 → U1, L2 → V1, L3 → W1"}
                                 : Text{"L1 → V1, L2 → U1, L3 → W1", "L1 → V1, L2 → U1, L3 → W1"});
            r.setText("how", {"Zwei Außenleiter tauschen (Wendeschütz: L1 und L3 tauschen ist ebenso üblich).",
                              "Swap two lines (reversing contactor: swapping L1 and L3 is also common)."});
        } else {
            r.setText("conn", cw ? Text{"L → U1 und Z1 (über C_B), N → U2 und Z2", "L → U1 and Z1 (via C_B), N → U2 and Z2"}
                                 : Text{"L → U1 und Z2 (über C_B), N → U2 und Z1", "L → U1 and Z2 (via C_B), N → U2 and Z1"});
            r.setText("how", {"Hilfswicklung umpolen (Z1 ↔ Z2); Hauptwicklung bleibt.", "Reverse the auxiliary winding (Z1 ↔ Z2); main winding unchanged."});
        }
        return r;
    };
    c.chart = [](const Inputs& in, const Result&, const Context&) {
        Sketch s;
        s.frame(100, 64, 0.64);
        const bool cw = in.choice("dir") == "cw";
        if (in.choice("type") == "3ph") {
            const std::array<double, 3> yl{52, 36, 20};
            const std::array<const char*, 3> L{"L1", "L2", "L3"};
            const std::array<const char*, 3> T{"U1", "V1", "W1"};
            const std::array<std::size_t, 3> map = cw ? std::array<std::size_t, 3>{0, 1, 2} : std::array<std::size_t, 3>{1, 0, 2};
            for (std::size_t i = 0; i < 3; ++i) {
                s.label(8, yl[i], L[i], "cb");
                s.line("ink", {{13, yl[i]}, {30, yl[i]}, {46, yl[map[i]]}, {60, yl[map[i]]}});
                s.circle("bgfill", 60, yl[i], 1.8, 16);
                s.label(56, yl[i] + 4, T[i]);
            }
            s.circle("bgfill", 80, 36, 15, 40);
            s.label(80, 39, "M", "cb");
            s.label(80, 32, "3~", "c");
            for (std::size_t i = 0; i < 3; ++i) s.line("ink", {{61.8, yl[i]}, {66.5, yl[i]}});
        } else {
            // L at the top, N at the bottom, main winding and auxiliary branch in parallel
            s.label(6, 56, "L", "cb");
            s.label(6, 8, "N", "cb");
            s.line("ink", {{10, 56}, {90, 56}});
            s.line("ink", {{10, 8}, {90, 8}});
            // main winding
            s.line("ink", {{30, 56}, {30, 8}});
            s.bar("bgfill", {30, 44}, {30, 20}, 5);
            s.label(24, 46, "U1", "r");
            s.label(24, 18, "U2", "r");
            // auxiliary winding with capacitor
            s.line("ink", {{68, 56}, {68, 48}});
            s.line("ink", {{68, 46}, {68, 8}});
            s.line("ink", {{62, 46}, {74, 46}});
            s.line("ink", {{62, 48}, {74, 48}});
            s.label(80, 47, "C_B", "l");
            s.bar("bgfill", {68, 36}, {68, 16}, 5);
            s.label(78, 36, cw ? "Z1" : "Z2", "l");
            s.label(78, 16, cw ? "Z2" : "Z1", "l");
            s.label(49, 30, cw ? "→ 1" : "← 2", "cb");
        }
        s.ch.note = {"Prinzipdarstellung; Klemmenlage am Motor siehe Anschlussbild des Herstellers.",
                     "Principle only; for the terminal layout see the manufacturer's diagram."};
        return s.ch;
    };
    return c;
}

}  // namespace

void addElectricalMotors(std::vector<Calculator>& out) {
    out.push_back(motorValues(Solve::Power));
    out.push_back(motorValues(Solve::Voltage));
    out.push_back(motorValues(Solve::PowerFactor));
    out.push_back(necFlc());
    out.push_back(wiring6());
    out.push_back(wiringNema(9));
    out.push_back(wiringNema(12));
    out.push_back(motorConnections());
}

}  // namespace elc::catalog
