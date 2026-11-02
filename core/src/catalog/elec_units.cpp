// Area "Elektrotechnik" (v1.7): Δ-Y transformation (drawing), voltage forms and levels, phase angle.
#include <cmath>
#include <numbers>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/sketch.hpp"
#include "elc/formulas/electrical.hpp"

namespace elc::catalog {

using namespace elc::electrical;

namespace {

constexpr const char* kCat = "electrical";
using P = Sketch::P;

Calculator deltaStar() {
    Calculator c;
    c.id = "delta_star";
    c.category = kCat;
    c.title = {"Dreieck-Stern-Umwandlung", "Delta-star transformation"};
    c.summary = {"Δ ↔ Y für Widerstände oder Impedanzen, mit Zeichnung", "Δ ↔ Y for resistors or impedances, with drawing"};
    c.description = {"Δ → Y: R₁ = R₁₂ · R₃₁ / ΣR    R₂ = R₁₂ · R₂₃ / ΣR    R₃ = R₂₃ · R₃₁ / ΣR    (ΣR = R₁₂ + R₂₃ + R₃₁)\n"
                     "Y → Δ: R₁₂ = (R₁R₂ + R₂R₃ + R₃R₁) / R₃    R₂₃ = … / R₁    R₃₁ = … / R₂\n"
                     "Symmetrisch: R_Y = R_Δ / 3. Gilt ebenso für komplexe Impedanzen.",
                     "Δ → Y: R₁ = R₁₂ · R₃₁ / ΣR    R₂ = R₁₂ · R₂₃ / ΣR    R₃ = R₂₃ · R₃₁ / ΣR    (ΣR = R₁₂ + R₂₃ + R₃₁)\n"
                     "Y → Δ: R₁₂ = (R₁R₂ + R₂R₃ + R₃R₁) / R₃    R₂₃ = … / R₁    R₃₁ = … / R₂\n"
                     "Balanced: R_Y = R_Δ / 3. Also valid for complex impedances."};
    c.inputs = {Num::choice("dir", {"Umwandlung", "Transformation"}, {{"d2y", {"Dreieck → Stern", "Delta → star"}},
                                                                      {"y2d", {"Stern → Dreieck", "Star → delta"}}}, "d2y"),
                Num::number("R12", {"R₁₂ (zwischen 1 und 2)", "R₁₂ (between 1 and 2)"}, "Ω", 10).positive(),
                Num::number("R23", {"R₂₃ (zwischen 2 und 3)", "R₂₃ (between 2 and 3)"}, "Ω", 20).positive(),
                Num::number("R31", {"R₃₁ (zwischen 3 und 1)", "R₃₁ (between 3 and 1)"}, "Ω", 30).positive(),
                Num::number("R1", {"R₁ (an Knoten 1)", "R₁ (at node 1)"}, "Ω", 5).positive(),
                Num::number("R2", {"R₂ (an Knoten 2)", "R₂ (at node 2)"}, "Ω", 3.333).positive(),
                Num::number("R3", {"R₃ (an Knoten 3)", "R₃ (at node 3)"}, "Ω", 10).positive()};
    for (int i = 1; i <= 3; ++i) c.inputs[static_cast<std::size_t>(i)].showIf("dir", {"d2y"});
    for (int i = 4; i <= 6; ++i) c.inputs[static_cast<std::size_t>(i)].showIf("dir", {"y2d"});
    c.outputs = {Out::number("oR1", {"R₁", "R₁"}, "Ω", 3).primary(), Out::number("oR2", {"R₂", "R₂"}, "Ω", 3).primary(),
                 Out::number("oR3", {"R₃", "R₃"}, "Ω", 3).primary(), Out::number("oR12", {"R₁₂", "R₁₂"}, "Ω", 3).primary(),
                 Out::number("oR23", {"R₂₃", "R₂₃"}, "Ω", 3).primary(), Out::number("oR31", {"R₃₁", "R₃₁"}, "Ω", 3).primary()};
    c.keywords = {"dreieck", "stern", "umwandlung", "delta", "star", "wye", "transformation"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        if (in.choice("dir") == "d2y") {
            const auto y = deltaToStar(in.num("R12"), in.num("R23"), in.num("R31"));
            r.set("oR1", y[0]);
            r.set("oR2", y[1]);
            r.set("oR3", y[2]);
        } else {
            const auto d = starToDelta(in.num("R1"), in.num("R2"), in.num("R3"));
            r.set("oR12", d[0]);
            r.set("oR23", d[1]);
            r.set("oR31", d[2]);
        }
        return r;
    };
    c.chart = [](const Inputs& in, const Result& res, const Context&) {
        Sketch s;
        s.frame(100, 54, 0.54);
        const bool d2y = in.choice("dir") == "d2y";
        auto val = [&](const char* inputId, const char* outId) {
            const double v = d2y == (std::string(inputId).size() == 3) ? in.num(inputId)
                                                                       : (res.values.contains(outId) ? res.values.at(outId) : kNaN);
            return std::isfinite(v) ? numText(v, 2, "Ω") : Text{"", ""};
        };
        // delta (left)
        const P a{25, 46}, b{7, 12}, d{43, 12};
        s.closed("ink", {a, b, d});
        s.bar("bgfill", Sketch::along(a, b, 0.3), Sketch::along(a, b, 0.7), 4);
        s.bar("bgfill", Sketch::along(b, d, 0.3), Sketch::along(b, d, 0.7), 4);
        s.bar("bgfill", Sketch::along(d, a, 0.3), Sketch::along(d, a, 0.7), 4);
        s.label(9, 33, "R₁₂", "rb");
        s.label(9, 28, val("R12", "oR12"), "r");
        s.label(25, 7, "R₂₃", "cb");
        s.label(25, 2.5, val("R23", "oR23"), "c");
        s.label(41, 33, "R₃₁", "lb");
        s.label(41, 28, val("R31", "oR31"), "l");
        // star (right)
        const P cY{75, 24}, n1{75, 46}, n2{57, 12}, n3{93, 12};
        for (const P& n : {n1, n2, n3}) {
            s.line("ink", {n, cY});
            s.bar("bgfill", Sketch::along(n, cY, 0.2), Sketch::along(n, cY, 0.75), 4);
        }
        s.circle("inkfill", cY[0], cY[1], 0.9, 12);
        s.label(79, 37, "R₁", "lb");
        s.label(79, 32.5, val("R1", "oR1"), "l");
        s.label(62, 22, "R₂", "rb");
        s.label(62, 17.5, val("R2", "oR2"), "r");
        s.label(88, 22, "R₃", "lb");
        s.label(88, 17.5, val("R3", "oR3"), "l");
        // nodes
        for (const auto& [p, name] : {std::pair{a, "1"}, std::pair{b, "2"}, std::pair{d, "3"}, std::pair{n1, "1"}, std::pair{n2, "2"},
                                      std::pair{n3, "3"}}) {
            s.circle("bgfill", p[0], p[1], 2.2, 16);
            s.label(p[0], p[1], name, "cb");
        }
        s.label(50, 30, d2y ? "→" : "←", "cb");
        s.ch.note = {"Links Dreieck (Δ), rechts Stern (Y); Knoten 1, 2, 3 entsprechen sich.",
                     "Left delta (Δ), right star (Y); nodes 1, 2, 3 correspond."};
        return s.ch;
    };
    return c;
}

Calculator voltageConverter() {
    Calculator c;
    c.id = "conv_voltage";
    c.category = kCat;
    c.title = {"Spannung", "Voltage"};
    c.summary = {"Einheiten, Effektiv-/Scheitel-/Spitze-Spitze-Wert (Sinus) und dB-Pegel", "Units, RMS/peak/peak-to-peak (sine) and dB levels"};
    c.description = {"Sinus: Û = √2 · U_eff    U_ss = 2 · Û    Gleichrichtwert |ū| = 2Û/π = 0,9003 · U_eff\n"
                     "Pegel (Effektivwert): dBV = 20 · lg(U / 1 V), dBu = 20 · lg(U / 0,7746 V), dBmV = 20 · lg(U / 1 mV), "
                     "dBµV = 20 · lg(U / 1 µV).",
                     "Sine: Û = √2 · U_rms    U_pp = 2 · Û    rectified mean |ū| = 2Û/π = 0.9003 · U_rms\n"
                     "Levels (RMS): dBV = 20 · lg(U / 1 V), dBu = 20 · lg(U / 0.7746 V), dBmV = 20 · lg(U / 1 mV), "
                     "dBµV = 20 · lg(U / 1 µV)."};
    c.inputs = {Num::number("v", {"Wert", "Value"}, "", 230),
                Num::choice("u", {"Einheit", "Unit"},
                            {{"uV", "µV"}, {"mV", "mV"}, {"V", "V"}, {"kV", "kV"}, {"dBV", "dBV"}, {"dBu", "dBu"}, {"dBmV", "dBmV"}, {"dBuV", "dBµV"}},
                            "V"),
                Num::choice("form", {"Angabe als (Sinus)", "Given as (sine)"},
                            {{"rms", {"Effektivwert", "RMS value"}}, {"peak", {"Scheitelwert Û", "Peak value Û"}},
                             {"pp", {"Spitze-Spitze U_ss", "Peak-to-peak U_pp"}}, {"avg", {"Gleichrichtwert", "Rectified mean"}}},
                            "rms")};
    c.outputs = {Out::number("rms", {"Effektivwert", "RMS value"}, "V", 4).primary(),
                 Out::number("peak", {"Scheitelwert Û", "Peak value Û"}, "V", 4),
                 Out::number("pp", {"Spitze-Spitze", "Peak-to-peak"}, "V", 4),
                 Out::number("avg", {"Gleichrichtwert", "Rectified mean"}, "V", 4),
                 Out::number("mV", {"Effektivwert", "RMS value"}, "mV", 3),
                 Out::number("kV", {"Effektivwert", "RMS value"}, "kV", 6),
                 Out::number("dBV", {"Pegel", "Level"}, "dBV", 2),
                 Out::number("dBu", {"Pegel", "Level"}, "dBu", 2),
                 Out::number("dBmV", {"Pegel", "Level"}, "dBmV", 2),
                 Out::number("dBuV", {"Pegel", "Level"}, "dBµV", 2)};
    c.keywords = {"spannung", "effektivwert", "scheitelwert", "dbu", "dbv", "voltage", "rms", "peak"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double v = in.num("v");
        const auto& u = in.choice("u");
        double V = 0;
        if (u == "uV") V = v * 1e-6;
        else if (u == "mV") V = v * 1e-3;
        else if (u == "kV") V = v * 1e3;
        else if (u == "dBV") V = std::pow(10.0, v / 20.0);
        else if (u == "dBu") V = 0.774597 * std::pow(10.0, v / 20.0);
        else if (u == "dBmV") V = 1e-3 * std::pow(10.0, v / 20.0);
        else if (u == "dBuV") V = 1e-6 * std::pow(10.0, v / 20.0);
        else V = v;
        const auto& f = in.choice("form");
        const double avgFactor = 2.0 * std::numbers::sqrt2 / std::numbers::pi;
        double rms = V;
        if (f == "peak") rms = V / std::numbers::sqrt2;
        else if (f == "pp") rms = V / (2.0 * std::numbers::sqrt2);
        else if (f == "avg") rms = V / avgFactor;
        r.set("rms", rms);
        r.set("peak", rms * std::numbers::sqrt2);
        r.set("pp", 2.0 * rms * std::numbers::sqrt2);
        r.set("avg", rms * avgFactor);
        r.set("mV", rms * 1e3);
        r.set("kV", rms / 1e3);
        if (rms > 0) {
            r.set("dBV", 20.0 * std::log10(rms));
            r.set("dBu", 20.0 * std::log10(rms / 0.774597));
            r.set("dBmV", 20.0 * std::log10(rms / 1e-3));
            r.set("dBuV", 20.0 * std::log10(rms / 1e-6));
        }
        if (f != "rms") r.info({"Umrechnung nur für Sinusform gültig.", "Conversion valid for sine waves only."});
        return r;
    };
    return c;
}

Calculator trigAngle() {
    Calculator c;
    c.id = "trig_angle";
    c.category = kCat;
    c.title = {"Winkel und trigonometrische Funktionen", "Angles and trigonometric functions"};
    c.summary = {"Grad, Bogenmaß, Gon ↔ sin, cos, tan (auch Phasenwinkel cos φ)", "Degrees, radians, gon ↔ sin, cos, tan (also phase angle cos φ)"};
    c.description = {"φ [rad] = φ [°] · π/180    φ [gon] = φ [°] · 400/360\nsin, cos, tan, cot des Winkels; aus sin/cos/tan wird der Hauptwert "
                     "(arcsin −90…90°, arccos 0…180°, arctan −90…90°) bestimmt. Leistungsfaktor: cos φ = P/S, tan φ = Q/P.",
                     "φ [rad] = φ [°] · π/180    φ [gon] = φ [°] · 400/360\nsin, cos, tan, cot of the angle; from sin/cos/tan the principal "
                     "value is returned (arcsin −90…90°, arccos 0…180°, arctan −90…90°). Power factor: cos φ = P/S, tan φ = Q/P."};
    c.inputs = {Num::number("v", {"Wert", "Value"}, "", 30),
                Num::choice("u", {"Gegeben", "Given"},
                            {{"deg", {"Winkel in Grad (°)", "Angle in degrees (°)"}}, {"rad", {"Winkel im Bogenmaß (rad)", "Angle in radians"}},
                             {"gon", {"Winkel in Gon (gon)", "Angle in gon"}}, {"sin", "sin"}, {"cos", {"cos (z. B. cos φ)", "cos (e.g. cos φ)"}},
                             {"tan", {"tan (z. B. Q/P)", "tan (e.g. Q/P)"}}},
                            "deg")};
    c.outputs = {Out::number("deg", {"Winkel", "Angle"}, "°", 4).primary(),
                 Out::number("rad", {"Winkel", "Angle"}, "rad", 5),
                 Out::number("gon", {"Winkel", "Angle"}, "gon", 4),
                 Out::number("sin", {"sin", "sin"}, "", 5),
                 Out::number("cos", {"cos", "cos"}, "", 5),
                 Out::number("tan", {"tan", "tan"}, "", 5),
                 Out::number("cot", {"cot", "cot"}, "", 5)};
    c.keywords = {"winkel", "trigonometrie", "sinus", "cosinus", "tangens", "phasenwinkel", "cos phi", "grad", "bogenmaß", "gon", "angle"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double v = in.num("v");
        const auto& u = in.choice("u");
        double phi = 0;
        if (u == "deg") phi = v * std::numbers::pi / 180.0;
        else if (u == "rad") phi = v;
        else if (u == "gon") phi = v * std::numbers::pi / 200.0;
        else if (u == "tan") phi = std::atan(v);
        else {
            if (v < -1 || v > 1) throw CalcError({"sin bzw. cos muss zwischen −1 und 1 liegen.", "sin or cos must be between −1 and 1."});
            phi = u == "cos" ? std::acos(v) : std::asin(v);
        }
        r.set("deg", phi * 180.0 / std::numbers::pi);
        r.set("rad", phi);
        r.set("gon", phi * 200.0 / std::numbers::pi);
        const double s = std::sin(phi), co = std::cos(phi);
        r.set("sin", std::fabs(s) < 1e-15 ? 0.0 : s);
        r.set("cos", std::fabs(co) < 1e-15 ? 0.0 : co);
        if (std::fabs(co) > 1e-12) r.set("tan", s / co);
        if (std::fabs(s) > 1e-12) r.set("cot", co / s);
        return r;
    };
    return c;
}

Calculator kvarMicrofarad() {
    Calculator c;
    c.id = "conv_kvar_uf";
    c.category = kCat;
    c.title = {"var / µF Umrechnung", "var / µF conversion"};
    c.summary = {"Blindleistung eines Kondensators ↔ Kapazität", "Reactive power of a capacitor ↔ capacitance"};
    c.description = {"Q = ω · C · U_C² je Kondensator, ω = 2πf.\n1~: U_C = U. 3~ Stern: U_C = U/√3, Q_ges = 3 · ω · C · U²/3 = ω · C · U². "
                     "3~ Dreieck: U_C = U, Q_ges = 3 · ω · C · U² (U = Außenleiterspannung, C = Kapazität je Kondensator).",
                     "Q = ω · C · U_C² per capacitor, ω = 2πf.\n1~: U_C = U. 3~ star: U_C = U/√3, Q_tot = 3 · ω · C · U²/3 = ω · C · U². "
                     "3~ delta: U_C = U, Q_tot = 3 · ω · C · U² (U = line voltage, C = capacitance per capacitor)."};
    c.inputs = {Num::choice("dir", {"Gegeben", "Given"}, {{"q", {"Blindleistung Q", "Reactive power Q"}}, {"c", {"Kapazität C", "Capacitance C"}}}, "q"),
                Num::number("Q", {"Blindleistung Q (gesamt)", "Reactive power Q (total)"}, "kvar", 10).positive(),
                Num::number("C", {"Kapazität je Kondensator", "Capacitance per capacitor"}, "µF", 66.3).positive(),
                Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
                Num::choice("f", {"Frequenz", "Frequency"}, {{"50", "50 Hz"}, {"60", "60 Hz"}}, "50"),
                Num::choice("conn", {"Schaltung", "Connection"},
                            {{"delta", {"3~ Dreieck", "3~ delta"}}, {"star", {"3~ Stern", "3~ star"}}, {"ac1", {"1~ / einzelner Kondensator", "1~ / single capacitor"}}},
                            "delta")};
    c.inputs[1].showIf("dir", {"q"});
    c.inputs[2].showIf("dir", {"c"});
    c.outputs = {Out::number("Cout", {"Kapazität je Kondensator", "Capacitance per capacitor"}, "µF", 2).primary(),
                 Out::number("Qout", {"Blindleistung gesamt", "Total reactive power"}, "kvar", 3).primary(),
                 Out::number("I", {"Strom (Außenleiter)", "Current (line)"}, "A", 2),
                 Out::number("Uc", {"Spannung am Kondensator", "Voltage across capacitor"}, "V", 1)};
    c.keywords = {"kvar", "µf", "uf", "kondensator", "blindleistung", "capacitor", "var"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double w = 2.0 * std::numbers::pi * parseDouble(in.choice("f"));
        const double U = in.num("U");
        const auto& conn = in.choice("conn");
        const double k = conn == "delta" ? 3.0 : 1.0;  // Q_tot = k · ω · C · U²
        const double Uc = conn == "star" ? U / kSqrt3 : U;
        if (in.choice("dir") == "q") {
            const double C = in.num("Q") * 1e3 / (k * w * U * U);
            r.set("Cout", C * 1e6);
            r.set("I", conn == "ac1" ? in.num("Q") * 1e3 / U : in.num("Q") * 1e3 / (kSqrt3 * U));
        } else {
            const double Q = k * w * in.num("C") * 1e-6 * U * U;
            r.set("Qout", Q / 1e3);
            r.set("I", conn == "ac1" ? Q / U : Q / (kSqrt3 * U));
        }
        r.set("Uc", Uc);
        return r;
    };
    return c;
}

}  // namespace

void addElectricalUnits(std::vector<Calculator>& out) {
    out.push_back(deltaStar());
    out.push_back(voltageConverter());
    out.push_back(trigAngle());
    out.push_back(kvarMicrofarad());
}

}  // namespace elc::catalog
