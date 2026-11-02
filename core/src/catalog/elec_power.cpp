// Area "Elektrotechnik" (v1.7): voltage, power factor, impedance, operating current, Joule heat,
// minimum short-circuit current, earthing/RCD, overvoltage risk, compensation of transformers and capacitors.
#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/electrical.hpp"

namespace elc::catalog {

using namespace elc::electrical;

namespace {

constexpr const char* kCat = "electrical";

std::vector<Option> acOptions() {
    return {{"ac1", {"Wechselstrom 1~", "Single-phase AC"}}, {"ac3", {"Drehstrom 3~", "Three-phase AC"}}};
}

double sinFromCos(double c) { return std::sqrt(std::max(0.0, 1.0 - c * c)); }

// One calculator for P = k · U · I · cos φ · η solved for P, I or U (v1.9 merges "Leistung P, Q, S", "Strom aus
// Leistung", "Spannungsberechnung", "Motor-Bemessungsstrom", "Motorleistung" and "Motorspannung").
Calculator powerCalc() {
    Calculator c;
    c.id = "power";
    c.category = kCat;
    c.title = {"Leistung, Strom und Spannung", "Power, current and voltage"};
    c.summary = {"P, Q, S, I oder U für DC, 1~ und 3~ – auch Motor mit cos φ und η",
                 "P, Q, S, I or U for DC, 1~ and 3~ – also motors with cos φ and η"};
    c.description = {
        "P_auf = k · U · I · cos φ    P_ab = P_auf · η    k = 1 (DC, 1~), √3 (3~, U = Außenleiterspannung)\n"
        "Strom: I = P_ab / (k · U · cos φ · η)    Spannung: U = P_ab / (k · I · cos φ · η)\n"
        "S = k · U · I    Q = S · sin φ    φ = arccos(cos φ)    U₀ = U / √3    1 hp = 745,7 W\n"
        "Motor: P_ab = Bemessungsleistung vom Typenschild (mechanisch) mit η; sonst η = 100 % (P = aufgenommene Wirkleistung).",
        "P_in = k · U · I · cos φ    P_out = P_in · η    k = 1 (DC, 1~), √3 (3~, U = line voltage)\n"
        "Current: I = P_out / (k · U · cos φ · η)    Voltage: U = P_out / (k · I · cos φ · η)\n"
        "S = k · U · I    Q = S · sin φ    φ = arccos(cos φ)    U₀ = U / √3    1 hp = 745.7 W\n"
        "Motor: P_out = rated (mechanical) power from the nameplate with η; otherwise η = 100 % (P = active input power)."};
    c.inputs = {
        Num::choice("solve", {"Gesucht", "Solve for"},
                    {{"I", {"Strom I (aus P und U)", "Current I (from P and U)"}},
                     {"P", {"Leistung P, Q, S (aus U und I)", "Power P, Q, S (from U and I)"}},
                     {"U", {"Spannung U (aus P und I)", "Voltage U (from P and I)"}}},
                    "I"),
        Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        Num::number("I", {"Strom I", "Current I"}, "A", 16).positive(),
        Num::number("P", {"Leistung P (abgegeben)", "Power P (output)"}, "kW", 7.5).positive()
            .withHint({"Motor: Bemessungsleistung P_N vom Typenschild", "Motor: rated power P_N from the nameplate"}),
        cosPhiInput(0.85),
        Num::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 100).rangeExclusiveMin(0, 100)
            .withHint({"100 % = P ist die aufgenommene Wirkleistung; Motor z. B. 85–95 %",
                       "100 % = P is the active input power; motors e.g. 85–95 %"}),
    };
    c.inputs[2].showIf("solve", {"I", "P"});
    c.inputs[3].showIf("solve", {"P", "U"});
    c.inputs[4].showIf("solve", {"I", "U"});
    c.outputs = {Out::number("I", {"Strom I", "Current I"}, "A", 2).primary(),
                 Out::number("U", {"Spannung U", "Voltage U"}, "V", 1).primary(),
                 Out::number("U0", {"Strangspannung U₀ (3~)", "Phase voltage U₀ (3~)"}, "V", 1),
                 Out::number("P", {"Abgegebene Leistung P_ab", "Output power P_out"}, "kW", 3).primary(),
                 Out::number("hp", {"Abgegebene Leistung", "Output power"}, "hp", 2),
                 Out::number("Pin", {"Aufgenommene Wirkleistung P_auf", "Active input power P_in"}, "kW", 3),
                 Out::number("S", {"Scheinleistung S", "Apparent power S"}, "kVA", 3),
                 Out::number("Q", {"Blindleistung Q", "Reactive power Q"}, "kvar", 3),
                 Out::number("phi", {"Phasenwinkel φ", "Phase angle φ"}, "°", 1),
                 Out::number("Ploss", {"Verluste P_auf − P_ab", "Losses P_in − P_out"}, "kW", 3)};
    c.keywords = {"leistung", "strom", "spannung", "wirkleistung", "blindleistung", "scheinleistung", "cos phi", "motor",
                  "nennstrom", "bemessungsstrom", "motorleistung", "motorspannung", "power", "current", "voltage", "hp", "ps"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double k = powerFactorK(sys);
        const double cp = sys == System::DC ? 1.0 : in.num("cosphi");
        const double eta = in.num("eta") / 100.0;
        const auto& solve = in.choice("solve");
        double U = 0, I = 0, Pin = 0;
        if (solve == "P") {
            U = in.num("U");
            I = in.num("I");
            Pin = k * U * I * cp;
        } else if (solve == "I") {
            U = in.num("U");
            Pin = in.num("P") * 1e3 / eta;
            I = Pin / (k * U * cp);
        } else {
            I = in.num("I");
            Pin = in.num("P") * 1e3 / eta;
            U = Pin / (k * I * cp);
        }
        const double S = k * U * I;
        const double Pout = Pin * eta;
        if (solve == "I") r.set("I", I);  // only the unknown, the given values stay in the inputs
        if (solve == "U") r.set("U", U);
        if (sys == System::AC3) r.set("U0", U / kSqrt3);
        r.set("P", Pout / 1e3);
        r.set("hp", Pout / kWattPerHp);
        r.set("Pin", Pin / 1e3);
        r.set("S", S / 1e3);
        if (sys != System::DC) {
            r.set("Q", S * sinFromCos(cp) / 1e3);
            r.set("phi", std::acos(cp) * 180.0 / std::numbers::pi);
        }
        if (eta < 1.0) r.set("Ploss", (Pin - Pout) / 1e3);
        return r;
    };
    return c;
}

Calculator powerFactorCalc() {
    Calculator c;
    c.id = "power_factor_calc";
    c.category = kCat;
    c.title = {"Leistungsfaktorberechnung", "Power factor calculation"};
    c.summary = {"cos φ aus P und U·I, aus P und S oder aus P und Q", "cos φ from P and U·I, from P and S or from P and Q"};
    c.description = {"cos φ = P / S    S = k · U · I (k = 1 bei 1~, √3 bei 3~)    S = √(P² + Q²)\n"
                     "Motor: P_auf = P_ab / η. φ = arccos(cos φ), Q = P · tan φ. Der Leistungsfaktor λ = P/S ist bei Oberschwingungen kleiner als cos φ₁.",
                     "cos φ = P / S    S = k · U · I (k = 1 for 1~, √3 for 3~)    S = √(P² + Q²)\n"
                     "Motor: P_in = P_out / η. φ = arccos(cos φ), Q = P · tan φ. With harmonics the power factor λ = P/S is smaller than cos φ₁."};
    c.inputs = {
        Num::choice("mode", {"Bekannt", "Known"},
                    {{"pui", {"P, U und I", "P, U and I"}}, {"ps", {"P und S", "P and S"}}, {"pq", {"P und Q", "P and Q"}}}, "pui"),
        Num::choice("sys", {"Netz", "System"}, acOptions(), "ac3"),
        Num::number("P", {"Wirkleistung P", "Active power P"}, "kW", 9).positive(),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        Num::number("I", {"Strom I", "Current I"}, "A", 16).positive(),
        Num::number("S", {"Scheinleistung S", "Apparent power S"}, "kVA", 11).positive(),
        Num::number("Q", {"Blindleistung Q", "Reactive power Q"}, "kvar", 5).nonNegative(),
        Num::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 100).rangeExclusiveMin(0, 100)
            .withHint({"Motor: P = abgegebene Leistung vom Typenschild mit η; sonst 100 %",
                       "Motor: P = rated output from the nameplate with η; otherwise 100 %"}),
    };
    c.inputs[1].showIf("mode", {"pui"});
    c.inputs[7].showIf("mode", {"pui"});
    c.inputs[3].showIf("mode", {"pui"});
    c.inputs[4].showIf("mode", {"pui"});
    c.inputs[5].showIf("mode", {"ps"});
    c.inputs[6].showIf("mode", {"pq"});
    c.outputs = {Out::number("cosphi", {"Leistungsfaktor cos φ", "Power factor cos φ"}, "", 3).primary(),
                 Out::number("phi", {"Phasenwinkel φ", "Phase angle φ"}, "°", 1),
                 Out::number("sinphi", {"sin φ", "sin φ"}, "", 3),
                 Out::number("tanphi", {"tan φ", "tan φ"}, "", 3),
                 Out::number("S", {"Scheinleistung S", "Apparent power S"}, "kVA", 3),
                 Out::number("Q", {"Blindleistung Q", "Reactive power Q"}, "kvar", 3),
                 Out::number("Pin", {"Aufgenommene Wirkleistung P / η", "Active input power P / η"}, "kW", 3)};
    c.keywords = {"cos phi", "leistungsfaktor", "power factor", "phasenwinkel", "motor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& mode = in.choice("mode");
        const double P = mode == "pui" ? in.num("P") / (in.num("eta") / 100.0) : in.num("P");
        double S = 0;
        if (mode == "ps") S = in.num("S");
        else if (mode == "pq") S = std::hypot(P, in.num("Q"));
        else S = powerFactorK(parseSystem(in.choice("sys"))) * in.num("U") * in.num("I") / 1e3;
        if (P > S * (1 + 1e-9))
            throw CalcError({"P ist größer als S – Eingaben prüfen (cos φ > 1).", "P exceeds S – check the inputs (cos φ > 1)."});
        const double cp = std::min(1.0, P / S);
        r.set("cosphi", cp);
        r.set("phi", std::acos(cp) * 180.0 / std::numbers::pi);
        r.set("sinphi", sinFromCos(cp));
        r.set("tanphi", sinFromCos(cp) / cp);
        r.set("S", S);
        r.set("Q", std::sqrt(std::max(0.0, S * S - P * P)));
        if (mode == "pui" && in.num("eta") < 100) r.set("Pin", P);
        if (cp < 0.9) r.warn({"cos φ < 0,9: Blindleistungskompensation prüfen (Netzbetreiber fordern meist ≥ 0,9).",
                              "cos φ < 0.9: consider power factor correction (utilities usually require ≥ 0.9)."});
        return r;
    };
    return c;
}

Calculator impedanceCalc() {
    Calculator c;
    c.id = "impedance_ui";
    c.category = kCat;
    c.title = {"Impedanzberechnung", "Impedance calculation"};
    c.summary = {"Z, R und X aus Spannung, Strom und cos φ", "Z, R and X from voltage, current and cos φ"};
    c.description = {"1~: Z = U / I    3~ (je Strang, Sternersatz): Z = U / (√3 · I)\nR = Z · cos φ    X = Z · sin φ",
                     "1~: Z = U / I    3~ (per phase, star equivalent): Z = U / (√3 · I)\nR = Z · cos φ    X = Z · sin φ"};
    c.inputs = {Num::choice("sys", {"Netz", "System"}, acOptions(), "ac1"),
                Num::number("U", {"Spannung U", "Voltage U"}, "V", 230).positive(),
                Num::number("I", {"Strom I", "Current I"}, "A", 10).positive(),
                Num::number("cosphi", {"Leistungsfaktor cos φ", "Power factor cos φ"}, "", 0.8).range(0, 1)};
    c.outputs = {Out::number("Z", {"Impedanz Z", "Impedance Z"}, "Ω", 3).primary(),
                 Out::number("R", {"Wirkwiderstand R", "Resistance R"}, "Ω", 3),
                 Out::number("X", {"Blindwiderstand X", "Reactance X"}, "Ω", 3),
                 Out::number("phi", {"Phasenwinkel φ", "Phase angle φ"}, "°", 1)};
    c.keywords = {"impedanz", "scheinwiderstand", "impedance"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto z = impedanceFromUI(parseSystem(in.choice("sys")), in.num("U"), in.num("I"), in.num("cosphi"));
        r.set("Z", z.Z);
        r.set("R", z.R);
        r.set("X", z.X);
        r.set("phi", std::acos(in.num("cosphi")) * 180.0 / std::numbers::pi);
        return r;
    };
    return c;
}

Calculator operatingCurrentCalc() {
    Calculator c;
    c.id = "operating_current";
    c.category = kCat;
    c.title = {"Betriebsstrom", "Operating current"};
    c.summary = {"I_b mit Ausnutzungs- und Gleichzeitigkeitsfaktor", "I_b with utilisation and simultaneity factor"};
    c.description = {"P_b = P_inst · k_u · k_s    I_b = P_b / (k · U · cos φ · η)\n"
                     "k_u = Ausnutzungsfaktor (Last eines Verbrauchers / Bemessungsleistung, Motoren typ. 0,75), "
                     "k_s = Gleichzeitigkeitsfaktor der Gruppe (z. B. 0,8–0,9 für Verteiler mit 4–9 Abgängen, IEC 61439-1 Tab. 101 als Anhalt).",
                     "P_b = P_inst · k_u · k_s    I_b = P_b / (k · U · cos φ · η)\n"
                     "k_u = utilisation factor (load / rated power, motors typ. 0.75), "
                     "k_s = simultaneity factor of the group (e.g. 0.8–0.9 for a board with 4–9 circuits, IEC 61439-1 Tab. 101 as a guide)."};
    c.inputs = {Num::choice("sys", {"Netz", "System"}, systemOptions(), "ac3"),
                Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
                Num::number("P", {"Installierte Leistung P_inst", "Installed power P_inst"}, "kW", 10).positive(),
                Num::number("ku", {"Ausnutzungsfaktor k_u", "Utilisation factor k_u"}, "", 0.75).rangeExclusiveMin(0, 1),
                Num::number("ks", {"Gleichzeitigkeitsfaktor k_s", "Simultaneity factor k_s"}, "", 0.8).rangeExclusiveMin(0, 1),
                cosPhiInput(0.85),
                Num::number("eta", {"Wirkungsgrad η", "Efficiency η"}, "%", 100).rangeExclusiveMin(0, 100)};
    c.outputs = {Out::number("Ib", {"Betriebsstrom I_b", "Operating current I_b"}, "A", 2).primary(),
                 Out::number("Pb", {"Betriebsleistung P_b", "Operating power P_b"}, "kW", 2),
                 Out::number("Imax", {"Strom bei Volllast (k_u = k_s = 1)", "Current at full load (k_u = k_s = 1)"}, "A", 2)};
    c.keywords = {"betriebsstrom", "gleichzeitigkeit", "operating current", "demand"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto sys = parseSystem(in.choice("sys"));
        const double eta = in.num("eta") / 100.0;
        const double P = in.num("P") * 1e3;
        r.set("Ib", operatingCurrent(sys, in.num("U"), P, in.num("ku"), in.num("ks"), in.num("cosphi"), eta));
        r.set("Pb", P * in.num("ku") * in.num("ks") / 1e3);
        r.set("Imax", operatingCurrent(sys, in.num("U"), P, 1, 1, in.num("cosphi"), eta));
        return r;
    };
    return c;
}

Calculator jouleCalc() {
    Calculator c;
    c.id = "joule_effect";
    c.category = kCat;
    c.title = {"Joule-Effekt", "Joule effect"};
    c.summary = {"Stromwärme W = I² · R · t und Erwärmung", "Joule heat W = I² · R · t and temperature rise"};
    c.description = {"P = I² · R    W = I² · R · t    ΔT = W / (m · c) (ohne Wärmeverluste)\n"
                     "c: Wasser 4186, Kupfer 385, Aluminium 897, Luft 1005 J/(kg·K).",
                     "P = I² · R    W = I² · R · t    ΔT = W / (m · c) (no heat losses)\n"
                     "c: water 4186, copper 385, aluminium 897, air 1005 J/(kg·K)."};
    c.inputs = {Num::number("R", {"Widerstand R", "Resistance R"}, "Ω", 26.5).positive(),
                Num::number("I", {"Strom I", "Current I"}, "A", 8.7).positive(),
                Num::number("t", {"Zeit t", "Time t"}, "s", 180).positive(),
                Num::number("m", {"Masse m", "Mass m"}, "kg", 1.5).positive().opt(),
                Num::number("c", {"Spezifische Wärmekapazität c", "Specific heat capacity c"}, "J/(kg·K)", 4186).positive()};
    c.outputs = {Out::number("P", {"Leistung P", "Power P"}, "W", 1).primary(),
                 Out::number("W", {"Wärmeenergie W", "Heat W"}, "kJ", 2),
                 Out::number("Wh", {"Wärmeenergie W", "Heat W"}, "Wh", 2),
                 Out::number("dT", {"Temperaturerhöhung ΔT", "Temperature rise ΔT"}, "K", 2)};
    c.keywords = {"joule", "stromwärme", "erwärmung", "heat"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double W = jouleHeat(in.num("R"), in.num("I"), in.num("t"));
        r.set("P", in.num("I") * in.num("I") * in.num("R"));
        r.set("W", W / 1e3);
        r.set("Wh", W / 3600.0);
        if (const auto m = in.maybe("m")) r.set("dT", temperatureRise(W, *m, in.num("c")));
        return r;
    };
    return c;
}

Calculator shortCircuitMinCalc() {
    Calculator c;
    c.id = "short_circuit_min";
    c.category = kCat;
    c.title = {"Minimaler Kurzschlussstrom", "Minimum short-circuit current"};
    c.summary = {"I_k1min, I_k2min am Leitungsende (c_min = 0,95)", "I_k1min, I_k2min at the end of a cable (c_min = 0.95)"};
    c.description = {
        "I_k1min = c_min · U₀ / |Z_V + Z_L + Z_PE|    I_k2min = c_min · √3 · U₀ / |Z_V + 2 · Z_L|\n"
        "c_min = 0,95 (NS, IEC 60909-0 / DIN EN 60909-0). Leiterwiderstand bei Endtemperatur θ (Kurzschlussende, "
        "meist 80 °C für PVC bzw. Betriebstemperatur 70/90 °C). Z_V = Schleifenimpedanz am Anfang der Leitung (L–PE, gemessen "
        "oder vom Netzbetreiber), X'_L = 0,08 mΩ/m je Leiter.\nPrüfung: I_k1min ≥ I_a der Schutzeinrichtung (Abschaltbedingung).",
        "I_k1min = c_min · U₀ / |Z_up + Z_L + Z_PE|    I_k2min = c_min · √3 · U₀ / |Z_up + 2 · Z_L|\n"
        "c_min = 0.95 (LV, IEC 60909-0). Conductor resistance at end temperature θ (end of short circuit, usually "
        "80 °C for PVC, or operating temperature 70/90 °C). Z_up = loop impedance at the origin of the cable (L–PE, measured "
        "or from the utility), X'_L = 0.08 mΩ/m per conductor.\nCheck: I_k1min ≥ I_a of the protective device (disconnection)."};
    c.inputs = {
        Num::number("U0", {"Nennspannung gegen Erde U₀", "Nominal voltage to earth U₀"}, "V", 230).positive(),
        Num::number("Zup", {"Schleifenimpedanz am Leitungsanfang Z_V", "Loop impedance at the origin Z_up"}, "mΩ", 150).nonNegative(),
        Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
        Num::number("L", {"Leitungslänge L", "Cable length L"}, "m", 50).positive(),
        Num::choice("Sph", {"Querschnitt Außenleiter", "Line conductor cross-section"}, areaOptions(cables::kStandardAreas), "2.5"),
        Num::choice("Spe", {"Querschnitt PE", "PE cross-section"}, areaOptions(cables::kStandardAreas), "2.5"),
        Num::number("theta", {"Leitertemperatur θ", "Conductor temperature θ"}, "°C", 80).range(20, 250),
        Num::number("Ia", {"Abschaltstrom I_a der Schutzeinrichtung", "Tripping current I_a of the device"}, "A", 80)
            .positive()
            .opt()
            .withHint({"z. B. B16: 5 · 16 = 80 A, C16: 160 A", "e.g. B16: 5 · 16 = 80 A, C16: 160 A"}),
    };
    c.outputs = {Out::number("ik1", {"I_k1min (L–PE)", "I_k1min (L–PE)"}, "A", 0).primary(),
                 Out::number("ik1n", {"I_k1min (L–N, N wie PE)", "I_k1min (L–N, N as PE)"}, "A", 0),
                 Out::number("ik2", {"I_k2min (L–L)", "I_k2min (L–L)"}, "A", 0),
                 Out::number("zs", {"Schleifenimpedanz Z_s am Ende", "Loop impedance Z_s at the end"}, "mΩ", 0),
                 Out::number("ratio", {"I_k1min / I_a", "I_k1min / I_a"}, "", 2)};
    c.keywords = {"kurzschluss", "ik min", "abschaltung", "short circuit", "fault current"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double c0 = 0.95;
        const auto mat = parseMaterial(in.choice("mat"));
        const double Sph = parseDouble(in.choice("Sph"));
        const double Spe = parseDouble(in.choice("Spe"));
        const auto sc = minShortCircuit(c0, in.num("U0"), in.num("Zup") / 1e3, 0.5, in.num("L"), Sph, Spe, Spe, mat,
                                        in.num("theta"), 0.08e-3);
        r.set("ik1", sc.ik1);
        r.set("ik1n", sc.ik1N);
        r.set("ik2", sc.ik2);
        r.set("zs", c0 * in.num("U0") / sc.ik1 * 1e3);
        if (const auto Ia = in.maybe("Ia")) {
            r.set("ratio", sc.ik1 / *Ia);
            if (sc.ik1 >= *Ia) r.ok({"I_k1min ≥ I_a: Abschaltbedingung erfüllt.", "I_k1min ≥ I_a: disconnection condition met."});
            else r.error({"I_k1min < I_a: Abschaltung nicht sichergestellt – Querschnitt erhöhen, Länge verkürzen oder RCD einsetzen.",
                          "I_k1min < I_a: disconnection not ensured – increase cross-section, shorten the cable or add an RCD."});
        }
        r.info({"Z_V wird mit X/R = 0,5 aufgeteilt (typisch für Niederspannungsnetze).",
                "Z_up is split with X/R = 0.5 (typical for low-voltage networks)."});
        return r;
    };
    return c;
}

Calculator earthingCalc() {
    Calculator c;
    c.id = "earthing_rcd";
    c.category = kCat;
    c.title = {"Erdungssystem und Koordination mit Fehlerstrom-Schutzeinrichtung",
               "Earthing system and coordination with RCD"};
    c.summary = {"TT: R_A · I_Δn ≤ U_L    TN: Z_s · I_a ≤ U₀", "TT: R_A · I_Δn ≤ U_L    TN: Z_s · I_a ≤ U₀"};
    c.description = {
        "TT-System (IEC 60364-4-41 / DIN VDE 0100-410, 411.5.3): R_A · I_Δn ≤ U_L, U_L = 50 V AC (25 V in besonderen "
        "Räumen, z. B. landwirtschaftliche Betriebsstätten). R_A = Summe aus Erder- und Schutzleiterwiderstand.\n"
        "TN-System (411.4.4): Z_s · I_a ≤ U₀; mit RCD ist I_a = I_Δn (5 · I_Δn für 0,04 s).\n"
        "Gegen die gültige Ausgabe prüfen.",
        "TT system (IEC 60364-4-41, 411.5.3): R_A · I_Δn ≤ U_L, U_L = 50 V AC (25 V in special locations, e.g. "
        "agricultural premises). R_A = sum of earth electrode and protective conductor resistance.\n"
        "TN system (411.4.4): Z_s · I_a ≤ U₀; with an RCD I_a = I_Δn (5 · I_Δn for 0.04 s).\nCheck against the edition in force."};
    c.inputs = {
        Num::choice("sys", {"Netzsystem", "Earthing system"},
                    {{"tt", {"TT-System", "TT system"}}, {"tn", {"TN-System mit RCD", "TN system with RCD"}}}, "tt"),
        Num::choice("idn", {"Bemessungsdifferenzstrom I_Δn", "Rated residual current I_Δn"},
                    {{"10", "10 mA"}, {"30", "30 mA"}, {"100", "100 mA"}, {"300", "300 mA"}, {"500", "500 mA"}, {"1000", "1 A"}}, "30"),
        Num::choice("ul", {"Berührungsspannung U_L", "Touch voltage U_L"}, {{"50", "50 V"}, {"25", "25 V"}}, "50"),
        Num::number("U0", {"Nennspannung U₀", "Nominal voltage U₀"}, "V", 230).positive(),
        Num::number("R", {"Gemessen: R_A bzw. Z_s", "Measured: R_A or Z_s"}, "Ω", 120).nonNegative().opt(),
    };
    c.inputs[2].showIf("sys", {"tt"});
    c.inputs[3].showIf("sys", {"tn"});
    c.outputs = {Out::number("Rmax", {"Zulässiger Widerstand (R_A bzw. Z_s)", "Permissible resistance (R_A or Z_s)"}, "Ω", 1).primary(),
                 Out::number("Uf", {"Berührungsspannung bei I_Δn", "Touch voltage at I_Δn"}, "V", 1),
                 Out::number("IdnMax", {"Max. I_Δn für den gemessenen Widerstand", "Max. I_Δn for the measured resistance"}, "mA", 0)};
    c.keywords = {"erdung", "rcd", "fi", "fehlerstrom", "tt", "earthing", "residual current"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double idn = parseDouble(in.choice("idn")) / 1e3;
        const bool tt = in.choice("sys") == "tt";
        const double limitU = tt ? parseDouble(in.choice("ul")) : in.num("U0");
        const double Rmax = tt ? maxEarthResistanceTT(limitU, idn) : maxLoopImpedanceTN(limitU, idn);
        r.set("Rmax", Rmax);
        if (const auto R = in.maybe("R")) {
            r.set("Uf", *R * idn);
            if (*R > 0) r.set("IdnMax", limitU / *R * 1e3);
            if (*R <= Rmax) r.ok({"Gemessener Wert zulässig.", "Measured value permissible."});
            else r.error({"Gemessener Wert zu hoch – Erder verbessern oder kleineres I_Δn wählen.",
                          "Measured value too high – improve the earth electrode or choose a smaller I_Δn."});
        }
        if (tt) r.info({"Im TT-System ist ein RCD praktisch immer nötig: Überstrom-Schutzeinrichtungen schalten wegen R_A meist nicht rechtzeitig ab.",
                        "In a TT system an RCD is practically always required: overcurrent devices rarely disconnect in time because of R_A."});
        return r;
    };
    return c;
}

Calculator surgeRiskCalc() {
    Calculator c;
    c.id = "surge_risk";
    c.category = kCat;
    c.title = {"Risikoanalyse für Überspannungen atmosphärischen Ursprungs", "Risk assessment for overvoltages of atmospheric origin"};
    c.summary = {"Bewertung nach IEC 60364-4-44, 443.5 (CRL)", "Assessment per IEC 60364-4-44, 443.5 (CRL)"};
    c.description = {
        "IEC 60364-4-44:2007+A1:2015 Abschnitt 443 (DIN VDE 0100-443:2016-10):\n"
        "Schutz gegen transiente Überspannungen ist immer vorzusehen, wenn Überspannungen Folgen haben für Menschenleben "
        "(z. B. Sicherheitsdienste, Medizin), öffentliche Einrichtungen und Kulturgüter, Gewerbe/Industrie oder eine große Anzahl "
        "von Personen (443.4). Sonst Risikobewertung:\n"
        "L_P = 2 · L_PAL + L_PCL + 0,4 · L_PAH + 0,2 · L_PCH (km; NS-Freileitung, NS-Kabel, HS-Freileitung, HS-Kabel ab dem "
        "Gebäude, zusammen höchstens 1 km)    CRL = f_env / (L_P · N_g)\n"
        "f_env = 85 (ländlich/vorstädtisch), 850 (städtisch). CRL < 1000 → Überspannungsschutz erforderlich.\n"
        "N_g = Blitzdichte (Erdblitze je km² und Jahr), z. B. aus Blitzdichtekarten (Deutschland meist 0,5–4).\n"
        "Gegen die gültige Ausgabe (DE: DIN VDE 0100-443/-534, CH: NIN 4.4.3) prüfen.",
        "IEC 60364-4-44:2007+A1:2015 section 443:\n"
        "Protection against transient overvoltages is always required where overvoltages affect human life (e.g. safety "
        "services, medical), public services and cultural heritage, commercial/industrial activity or a large number of "
        "persons (443.4). Otherwise risk assessment:\n"
        "L_P = 2 · L_PAL + L_PCL + 0.4 · L_PAH + 0.2 · L_PCH (km; LV overhead, LV cable, HV overhead, HV cable from the "
        "building, together at most 1 km)    CRL = f_env / (L_P · N_g)\n"
        "f_env = 85 (rural/suburban), 850 (urban). CRL < 1000 → overvoltage protection required.\n"
        "N_g = lightning ground flash density (flashes per km² and year) from flash density maps.\nCheck against the edition in force."};
    c.inputs = {
        Num::choice("crit", {"Folgen für Leben, öffentliche Dienste, Gewerbe oder viele Personen?",
                             "Consequences for life, public services, business or many persons?"},
                    {{"no", {"Nein (z. B. Wohngebäude)", "No (e.g. dwelling)"}}, {"yes", {"Ja", "Yes"}}}, "no"),
        Num::choice("env", {"Umgebung", "Environment"},
                    {{"rural", {"Ländlich / vorstädtisch (f_env = 85)", "Rural / suburban (f_env = 85)"}},
                     {"urban", {"Städtisch (f_env = 850)", "Urban (f_env = 850)"}}}, "rural"),
        Num::number("Ng", {"Blitzdichte N_g", "Flash density N_g"}, "1/(km²·a)", 2).positive(),
        Num::number("lpal", {"NS-Freileitung L_PAL", "LV overhead line L_PAL"}, "km", 0.2).nonNegative(),
        Num::number("lpcl", {"NS-Kabel L_PCL", "LV cable L_PCL"}, "km", 0.3).nonNegative(),
        Num::number("lpah", {"HS-Freileitung L_PAH", "HV overhead line L_PAH"}, "km", 0.5).nonNegative(),
        Num::number("lpch", {"HS-Kabel L_PCH", "HV cable L_PCH"}, "km", 0).nonNegative(),
    };
    c.outputs = {Out::text("verdict", {"Ergebnis", "Result"}).primary(),
                 Out::number("LP", {"Bewertungslänge L_P", "Risk assessment length L_P"}, "km", 3),
                 Out::number("CRL", {"Berechneter Risikopegel CRL", "Calculated risk level CRL"}, "", 0)};
    c.keywords = {"überspannung", "spd", "blitz", "crl", "443", "surge", "overvoltage", "lightning"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        double lpal = in.num("lpal"), lpcl = in.num("lpcl"), lpah = in.num("lpah"), lpch = in.num("lpch");
        const double total = lpal + lpcl + lpah + lpch;
        if (total > 1.0) {
            const double k = 1.0 / total;
            lpal *= k;
            lpcl *= k;
            lpah *= k;
            lpch *= k;
            r.warn({"Leitungslängen zusammen > 1 km: anteilig auf 1 km begrenzt. Genauer: nur den ersten Kilometer ab dem Gebäude eingeben.",
                    "Line lengths exceed 1 km in total: scaled to 1 km. More exact: enter only the first kilometre from the building."});
        }
        const double LP = surgeLP(lpal, lpcl, lpah, lpch);
        r.set("LP", LP);
        const bool critical = in.choice("crit") == "yes";
        const double fenv = in.choice("env") == "urban" ? 850.0 : 85.0;
        if (LP <= 0.0) {
            r.setText("verdict", critical ? Text{"Überspannungsschutz erforderlich (443.4)", "Overvoltage protection required (443.4)"}
                                          : Text{"Kein Risiko über Leitungen (L_P = 0)", "No risk via lines (L_P = 0)"});
            return r;
        }
        const double crl = surgeCRL(fenv, LP, in.num("Ng"));
        r.set("CRL", crl);
        if (critical || crl < 1000.0) {
            r.setText("verdict", {"Überspannungsschutz (SPD Typ 2, ggf. Typ 1) erforderlich", "Overvoltage protection (SPD type 2, or type 1) required"});
            r.warn(critical ? Text{"Erforderlich wegen der Folgen nach 443.4.", "Required because of the consequences per 443.4."}
                            : Text{"CRL < 1000.", "CRL < 1000."});
        } else {
            r.setText("verdict", {"Nach Risikobewertung nicht erforderlich", "Not required by the risk assessment"});
            r.ok({"CRL ≥ 1000. Überspannungsschutz trotzdem empfehlenswert (Geräteschutz).",
                  "CRL ≥ 1000. Overvoltage protection still recommended (equipment protection)."});
        }
        r.info({"Bei äußerem Blitzschutz (LPS) oder Freileitungseinspeisung: SPD Typ 1 am Hausanschluss (DIN VDE 0100-534).",
                "With an external lightning protection system or overhead supply: SPD type 1 at the service entrance (IEC 60364-5-53/534)."});
        return r;
    };
    return c;
}

Calculator transformerPfcCalc() {
    Calculator c;
    c.id = "transformer_pfc";
    c.category = kCat;
    c.title = {"Blindleistungskompensation von MS/NS-Transformatoren", "Reactive power compensation of MV/LV transformers"};
    c.summary = {"Leerlauf- und Last-Blindleistung, Festkompensation", "No-load and load reactive power, fixed compensation"};
    c.description = {
        "Leerlauf: Q₀ = i₀ · S_N (Magnetisierung, immer vorhanden)\nLast: Q_L = u_k · S_N · (S/S_N)² (Streufeld)\n"
        "Festkompensation des Transformators ≈ Q₀ (Kondensator auf der NS-Seite, dauernd zugeschaltet). i₀ und u_k vom "
        "Typenschild bzw. Prüfprotokoll; Öltransformatoren typ. i₀ = 0,5–2 %, Gießharz 0,5–1,5 %. Höchstwert der "
        "Festkompensation nach den Technischen Anschlussbedingungen des Netzbetreibers prüfen (Resonanz, Überkompensation).",
        "No-load: Q₀ = i₀ · S_N (magnetising, always present)\nLoad: Q_L = u_k · S_N · (S/S_N)² (leakage field)\n"
        "Fixed compensation of the transformer ≈ Q₀ (capacitor on the LV side, permanently connected). i₀ and u_k from the "
        "rating plate or test report; oil transformers typ. i₀ = 0.5–2 %, cast resin 0.5–1.5 %. Check the maximum fixed "
        "compensation with the utility's connection rules (resonance, over-compensation)."};
    c.inputs = {Num::number("Sn", {"Bemessungsleistung S_N", "Rated power S_N"}, "kVA", 630).positive(),
                Num::number("i0", {"Leerlaufstrom i₀", "No-load current i₀"}, "%", 1.5).rangeExclusiveMin(0, 10),
                Num::number("uk", {"Kurzschlussspannung u_k", "Short-circuit voltage u_k"}, "%", 6).rangeExclusiveMin(0, 20),
                Num::number("S", {"Belastung S", "Load S"}, "kVA", 500).nonNegative()};
    c.outputs = {Out::number("Q0", {"Leerlauf-Blindleistung Q₀", "No-load reactive power Q₀"}, "kvar", 2).primary(),
                 Out::number("QL", {"Last-Blindleistung Q_L", "Load reactive power Q_L"}, "kvar", 2),
                 Out::number("Q", {"Gesamt Q₀ + Q_L", "Total Q₀ + Q_L"}, "kvar", 2),
                 Out::number("load", {"Auslastung S/S_N", "Loading S/S_N"}, "%", 1)};
    c.keywords = {"transformator", "kompensation", "blindleistung", "trafo", "transformer", "pfc"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double Sn = in.num("Sn");
        const double Q0 = transformerNoLoadQ(Sn, in.num("i0"));
        const double QL = transformerLoadQ(Sn, in.num("uk"), in.num("S"));
        r.set("Q0", Q0);
        r.set("QL", QL);
        r.set("Q", Q0 + QL);
        r.set("load", in.num("S") / Sn * 100.0);
        if (in.num("S") > Sn) r.warn({"Transformator überlastet (S > S_N).", "Transformer overloaded (S > S_N)."});
        return r;
    };
    return c;
}

Calculator capacitorVoltageCalc() {
    Calculator c;
    c.id = "capacitor_voltage";
    c.category = kCat;
    c.title = {"Kondensatorleistung bei verschiedenen Spannungen", "Capacitor power at different voltages"};
    c.summary = {"Q = Q_N · (U/U_N)² · (f/f_N) und Kapazität", "Q = Q_N · (U/U_N)² · (f/f_N) and capacitance"};
    c.description = {"Q = Q_N · (U/U_N)² · (f/f_N)\nKapazität (3~): Dreieck C_Δ = Q / (3 · 2πf · U²) je Kondensator, "
                     "Stern C_Y = Q / (2πf · U²) je Kondensator (U = Außenleiterspannung). 1~: C = Q / (2πf · U²).\n"
                     "Kondensatoren dürfen dauerhaft höchstens mit 1,1 · U_N betrieben werden (IEC 60831-1).",
                     "Q = Q_N · (U/U_N)² · (f/f_N)\nCapacitance (3~): delta C_Δ = Q / (3 · 2πf · U²) per capacitor, "
                     "star C_Y = Q / (2πf · U²) per capacitor (U = line voltage). 1~: C = Q / (2πf · U²).\n"
                     "Capacitors may be operated continuously at no more than 1.1 · U_N (IEC 60831-1)."};
    c.inputs = {Num::number("Qn", {"Bemessungsleistung Q_N", "Rated power Q_N"}, "kvar", 25).positive(),
                Num::number("Un", {"Bemessungsspannung U_N", "Rated voltage U_N"}, "V", 440).positive(),
                Num::choice("fn", {"Bemessungsfrequenz f_N", "Rated frequency f_N"}, {{"50", "50 Hz"}, {"60", "60 Hz"}}, "50"),
                Num::number("U", {"Betriebsspannung U", "Operating voltage U"}, "V", 400).positive(),
                Num::choice("f", {"Netzfrequenz f", "System frequency f"}, {{"50", "50 Hz"}, {"60", "60 Hz"}}, "50"),
                Num::choice("sys", {"Ausführung", "Design"},
                            {{"delta", {"3~ Dreieck", "3~ delta"}}, {"star", {"3~ Stern", "3~ star"}}, {"ac1", {"1~", "1~"}}}, "delta")};
    c.outputs = {Out::number("Q", {"Leistung bei U und f", "Power at U and f"}, "kvar", 2).primary(),
                 Out::number("ratio", {"Q / Q_N", "Q / Q_N"}, "%", 1),
                 Out::number("C", {"Kapazität je Kondensator", "Capacitance per capacitor"}, "µF", 1),
                 Out::number("I", {"Strom (Außenleiter)", "Current (line)"}, "A", 1)};
    c.keywords = {"kondensator", "kompensation", "kvar", "capacitor"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double fn = parseDouble(in.choice("fn"));
        const double f = parseDouble(in.choice("f"));
        const double Un = in.num("Un");
        const double U = in.num("U");
        const double Qn = in.num("Qn") * 1e3;
        const double Q = capacitorPowerAt(Qn, Un, fn, U, f);
        r.set("Q", Q / 1e3);
        r.set("ratio", Q / Qn * 100.0);
        const auto& sys = in.choice("sys");
        // capacitance from the rated values (independent of the operating point)
        const double Cfull = capacitanceFor(Qn, Un, fn);
        r.set("C", (sys == "delta" ? Cfull / 3.0 : Cfull) * 1e6);
        r.set("I", sys == "ac1" ? Q / U : Q / (kSqrt3 * U));
        if (U > 1.1 * Un) r.error({"U > 1,1 · U_N: Kondensator überlastet (IEC 60831-1).", "U > 1.1 · U_N: capacitor overloaded (IEC 60831-1)."});
        else if (U < Un) r.info({"Unter Bemessungsspannung sinkt die Leistung quadratisch – häufig bewusst (440-V-Kondensator im 400-V-Netz wegen Oberschwingungen).",
                                 "Below the rated voltage the power drops with the square – often intended (440 V capacitor in a 400 V system because of harmonics)."});
        return r;
    };
    return c;
}

}  // namespace

void addElectricalPower(std::vector<Calculator>& out) {
    out.push_back(powerCalc());
    out.push_back(powerFactorCalc());
    out.push_back(impedanceCalc());
    out.push_back(operatingCurrentCalc());
    out.push_back(jouleCalc());
    out.push_back(shortCircuitMinCalc());
    out.push_back(earthingCalc());
    out.push_back(surgeRiskCalc());
    out.push_back(transformerPfcCalc());
    out.push_back(capacitorVoltageCalc());
}

}  // namespace elc::catalog
