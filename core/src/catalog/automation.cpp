// Calculators: Gebäudeautomation & MSR / Building automation & I&C
#include <array>
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/heating.hpp"
#include "elc/formulas/automation.hpp"
#include "elc/formulas/cables.hpp"

namespace elc::catalog {

using namespace elc::automation;

namespace {

struct SignalDef {
    const char* id;
    double lo, hi;
    const char* unit;
};
constexpr std::array<SignalDef, 5> kSignals{{
    {"4-20", 4, 20, "mA"},
    {"0-20", 0, 20, "mA"},
    {"0-10", 0, 10, "V"},
    {"2-10", 2, 10, "V"},
    {"1-5", 1, 5, "V"},
}};

const SignalDef& signal(const std::string& id) {
    for (const auto& s : kSignals) {
        if (id == s.id) return s;
    }
    return kSignals[0];
}

struct RawDef {
    const char* id;
    double lo, hi;
};
constexpr std::array<RawDef, 5> kRaw{{
    {"none", 0, 0},
    {"s7", 0, 27648},
    {"12bit", 0, 4095},
    {"15bit", 0, 32767},
    {"16bit", 0, 65535},
}};

const RawDef& raw(const std::string& id) {
    for (const auto& r : kRaw) {
        if (id == r.id) return r;
    }
    return kRaw[0];
}

Calculator analogScaling() {
    Calculator c;
    c.id = "analog_scaling";
    c.category = "automation";
    c.title = {"Analogsignal skalieren", "Analog signal scaling"};
    c.summary = {"4–20 mA / 0–10 V ↔ Messwert ↔ Rohwert (SPS)", "4–20 mA / 0–10 V ↔ value ↔ raw (PLC)"};
    c.description = {
        "Lineare Skalierung: y = y_min + (x − x_min) · (y_max − y_min) / (x_max − x_min)\n"
        "Rohwerte: Siemens S7 0…27648 (Nennbereich), 12 Bit 0…4095, 15 Bit 0…32767, 16 Bit 0…65535.\n"
        "NAMUR NE 43 (4–20 mA): Messbereich 3,8…20,5 mA, Ausfallsignal ≤ 3,6 mA bzw. ≥ 21 mA.",
        "Linear scaling: y = y_min + (x − x_min) · (y_max − y_min) / (x_max − x_min)\n"
        "Raw values: Siemens S7 0…27648 (nominal), 12 bit 0…4095, 15 bit 0…32767, 16 bit 0…65535.\n"
        "NAMUR NE 43 (4–20 mA): measuring range 3.8…20.5 mA, failure ≤ 3.6 mA or ≥ 21 mA."};
    c.inputs = {
        Num::choice("sig", {"Signal", "Signal"},
                    {{"4-20", "4–20 mA"}, {"0-20", "0–20 mA"}, {"0-10", "0–10 V"}, {"2-10", "2–10 V"}, {"1-5", "1–5 V"}}, "4-20"),
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"sig", {"Signalwert (mA/V)", "Signal value (mA/V)"}},
                     {"eng", {"Messwert (physikalisch)", "Engineering value"}},
                     {"raw", {"Rohwert (SPS)", "Raw value (PLC)"}}},
                    "sig"),
        Num::number("x", {"Wert", "Value"}, "", 12),
        Num::number("lo", {"Messbereich Anfang", "Range start"}, "", 0),
        Num::number("hi", {"Messbereich Ende", "Range end"}, "", 100),
        Num::choice("raw", {"Rohwertformat", "Raw format"},
                    {{"none", {"keins", "none"}}, {"s7", "Siemens S7 (0…27648)"}, {"12bit", "12 Bit (0…4095)"},
                     {"15bit", "15 Bit (0…32767)"}, {"16bit", "16 Bit (0…65535)"}},
                    "s7"),
    };
    c.outputs = {Out::number("pct", {"Anteil am Messbereich", "Percentage of span"}, "%", 2),
                 Out::number("sig", {"Signal", "Signal"}, "mA", 3).primary(),
                 Out::number("eng", {"Messwert", "Engineering value"}, "", 3).primary(),
                 Out::number("raw", {"Rohwert", "Raw value"}, "", 0)};
    c.keywords = {"4-20", "4 20 ma", "0-10 v", "skalierung", "analog", "s7", "27648", "ne43", "scaling"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& s = signal(in.choice("sig"));
        const auto& rw = raw(in.choice("raw"));
        const bool hasRaw = std::string(rw.id) != "none";
        const double lo = in.num("lo");
        const double hi = in.num("hi");
        if (lo == hi) throw CalcError({"Messbereich Anfang und Ende dürfen nicht gleich sein.", "Range start and end must differ."});
        const double x = in.num("x");
        double pct = 0;
        const auto& dir = in.choice("dir");
        if (dir == "eng") {
            pct = scale(x, lo, hi, 0, 100);
        } else if (dir == "raw") {
            if (!hasRaw) throw CalcError({"Bitte ein Rohwertformat wählen.", "Please select a raw format."});
            pct = scale(x, rw.lo, rw.hi, 0, 100);
        } else {
            pct = scale(x, s.lo, s.hi, 0, 100);
        }
        const double sigVal = scale(pct, 0, 100, s.lo, s.hi);
        r.set("pct", pct);
        r.set("sig", sigVal, s.unit);
        r.set("eng", scale(pct, 0, 100, lo, hi));
        if (hasRaw) r.set("raw", std::round(scale(pct, 0, 100, rw.lo, rw.hi)));
        if (std::string(s.id) == "4-20") {
            if (sigVal <= 3.6 || sigVal >= 21.0) {
                r.error({"NE 43: Ausfallsignal (≤ 3,6 mA oder ≥ 21 mA) – Drahtbruch/Kurzschluss/Gerätefehler.",
                         "NE 43: failure signal (≤ 3.6 mA or ≥ 21 mA) – open/short circuit or device fault."});
            } else if (sigVal < 3.8 || sigVal > 20.5) {
                r.warn({"NE 43: außerhalb des Messbereichs 3,8…20,5 mA.", "NE 43: outside measuring range 3.8…20.5 mA."});
            }
        }
        if (pct < 0 || pct > 100) r.warn({"Wert außerhalb des Nennbereichs.", "Value outside nominal range."});
        return r;
    };
    return c;
}

Calculator loopBurden() {
    Calculator c;
    c.id = "loop_burden";
    c.category = "automation";
    c.title = {"4–20-mA-Bürde / Schleife", "4–20 mA loop burden"};
    c.summary = {"Max. Bürde, Leitungswiderstand, max. Länge", "Max. load, cable resistance, max. length"};
    c.description = {
        "R_max = (U_S − U_min,Transmitter) / I_max\nR_Leitung = 2 · ρ · L / A    Reserve = R_max − R_Last − R_Leitung\n"
        "Typisch: U_S = 24 V, Transmitter 10…12 V, AI-Bürde 250 Ω (HART ≥ 230 Ω). "
        "I_max 20 mA (bzw. 21…22 mA für NE-43-Fehlersignal).",
        "R_max = (U_S − U_min,transmitter) / I_max\nR_cable = 2 · ρ · L / A    Margin = R_max − R_load − R_cable\n"
        "Typical: U_S = 24 V, transmitter 10…12 V, AI burden 250 Ω (HART ≥ 230 Ω). "
        "I_max 20 mA (or 21…22 mA for NE 43 failure signal)."};
    c.inputs = {
        Num::number("Us", {"Speisespannung U_S", "Supply voltage U_S"}, "V", 24).positive(),
        Num::number("Umin", {"Min. Transmitterspannung", "Min. transmitter voltage"}, "V", 12).nonNegative(),
        Num::number("Imax", {"Max. Schleifenstrom", "Max. loop current"}, "mA", 20).positive(),
        Num::number("Rload", {"Bürde Auswertung (AI, Anzeiger …)", "Load (AI, indicator …)"}, "Ω", 250).nonNegative(),
        Num::number("L", {"Leitungslänge (einfach)", "Cable length (one way)"}, "m", 200).nonNegative(),
        Num::number("A", {"Querschnitt", "Cross-section"}, "mm²", 0.5).positive(),
    };
    c.outputs = {Out::number("Rmax", {"Max. Gesamtbürde", "Max. total load"}, "Ω", 0),
                 Out::number("Rcable", {"Leitungswiderstand (Hin + Rück)", "Cable resistance (loop)"}, "Ω", 1),
                 Out::number("reserve", {"Reserve", "Margin"}, "Ω", 0).primary(),
                 Out::number("Lmax", {"Max. Leitungslänge", "Max. cable length"}, "m", 0)};
    c.keywords = {"bürde", "4-20", "stromschleife", "loop", "burden", "hart", "transmitter"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double I = in.num("Imax") / 1e3;
        const double Rmax = (in.num("Us") - in.num("Umin")) / I;
        if (Rmax <= 0) throw CalcError({"Speisespannung kleiner als Transmitter-Mindestspannung.", "Supply below transmitter minimum voltage."});
        const double rho = cables::resistivity(cables::Material::Cu, 30.0);
        const double Rc = 2.0 * rho * in.num("L") / in.num("A");
        const double reserve = Rmax - in.num("Rload") - Rc;
        r.set("Rmax", Rmax);
        r.set("Rcable", Rc);
        r.set("reserve", reserve);
        r.set("Lmax", std::max(0.0, (Rmax - in.num("Rload")) * in.num("A") / (2.0 * rho)));
        if (reserve >= 0) {
            r.ok({"Schleife funktioniert – Bürde ausreichend.", "Loop OK – load within limit."});
        } else {
            r.error({"Bürde überschritten – höhere Speisespannung, größerer Querschnitt oder kleinere Last.",
                     "Load exceeded – raise supply voltage, increase cross-section or reduce load."});
        }
        return r;
    };
    return c;
}

std::vector<Option> rtdOptions() {
    return {{"pt100", "Pt100 (IEC 60751)"},
            {"pt500", "Pt500 (IEC 60751)"},
            {"pt1000", "Pt1000 (IEC 60751)"},
            {"ni1000", "Ni1000 (DIN 43760, TK 6180)"},
            {"ni1000tk5000", "Ni1000 TK5000 (LG-Ni 1000)"},
            {"cu10", {"Cu10 (Kupfer, α = 0,00427)", "Cu10 (copper, α = 0.00427)"}},
            {"cu50", {"Cu50 (Kupfer, α = 0,00428)", "Cu50 (copper, α = 0.00428)"}},
            {"cu100", {"Cu100 (Kupfer, α = 0,00428)", "Cu100 (copper, α = 0.00428)"}}};
}

Rtd parseRtd(const std::string& v) {
    if (v == "pt100") return Rtd::Pt100;
    if (v == "pt500") return Rtd::Pt500;
    if (v == "ni1000") return Rtd::Ni1000;
    if (v == "ni1000tk5000") return Rtd::Ni1000TK5000;
    if (v == "cu10") return Rtd::Cu10;
    if (v == "cu50") return Rtd::Cu50;
    if (v == "cu100") return Rtd::Cu100;
    return Rtd::Pt1000;
}

Calculator rtd() {
    Calculator c;
    c.id = "rtd";
    c.category = "automation";
    c.title = {"Widerstandsthermometer Pt/Ni/Cu", "RTD Pt/Ni/Cu"};
    c.summary = {"Temperatur ↔ Widerstand für Pt100/Pt1000/Ni1000/Cu100", "Temperature ↔ resistance for Pt100/Pt1000/Ni1000/Cu100"};
    c.description = {
        "Pt (IEC 60751): R = R0 · (1 + A·T + B·T² + C·(T − 100)·T³), C nur unter 0 °C\n"
        "A = 3,9083·10⁻³, B = −5,775·10⁻⁷, C = −4,183·10⁻¹²\n"
        "Ni1000 (DIN 43760): R = R0 · (1 + A·T + B·T² + D·T⁴ + F·T⁶)\n"
        "Ni1000 TK5000: R = R0 · (1 + 4,427·10⁻³·T + 5,172·10⁻⁶·T² + 5,585·10⁻⁹·T³)\n"
        "Cu10/Cu50/Cu100: R = R0 · (1 + α · T), linear −50 … 150 °C; α = 0,00427 1/K (Cu10, US-Praxis), 0,00428 1/K (GOST 6651)",
        "Pt (IEC 60751): R = R0 · (1 + A·T + B·T² + C·(T − 100)·T³), C below 0 °C only\n"
        "A = 3.9083·10⁻³, B = −5.775·10⁻⁷, C = −4.183·10⁻¹²\n"
        "Ni1000 (DIN 43760): R = R0 · (1 + A·T + B·T² + D·T⁴ + F·T⁶)\n"
        "Ni1000 TK5000: R = R0 · (1 + 4.427·10⁻³·T + 5.172·10⁻⁶·T² + 5.585·10⁻⁹·T³)\n"
        "Cu10/Cu50/Cu100: R = R0 · (1 + α · T), linear −50 … 150 °C; α = 0.00427 1/K (Cu10, US practice), 0.00428 1/K (GOST 6651)"};
    c.inputs = {
        Num::choice("type", {"Fühlertyp", "Sensor type"}, rtdOptions(), "pt1000"),
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"T", {"Temperatur", "Temperature"}}, {"R", {"Widerstand", "Resistance"}}}, "T"),
        Num::number("T", {"Temperatur T", "Temperature T"}, "°C", 20).range(-250, 900).showIf("dir", {"T"}),
        Num::number("R", {"Widerstand R", "Resistance R"}, "Ω", 1077.9).positive().showIf("dir", {"R"}),
    };
    c.outputs = {Out::number("R", {"Widerstand R", "Resistance R"}, "Ω", 3),
                 Out::number("T", {"Temperatur T", "Temperature T"}, "°C", 2).primary(),
                 Out::number("S", {"Empfindlichkeit dR/dT", "Sensitivity dR/dT"}, "Ω/K", 4)};
    c.keywords = {"pt100", "pt1000", "ni1000", "cu100", "cu50", "kupfer", "fühler", "temperatur", "rtd", "widerstandsthermometer", "lg-ni"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto type = parseRtd(in.choice("type"));
        const double T = in.choice("dir") == "R" ? rtdTemperature(type, in.num("R")) : in.num("T");
        r.set("T", T);
        r.set("R", rtdResistance(type, T));
        r.set("S", rtdSensitivity(type, T));
        const auto range = rtdRange(type);
        if (T < range[0] || T > range[1]) {
            r.warn(cat(Text{"Außerhalb des Kennlinienbereichs ", "Outside characteristic range "}, numText(range[0], 0),
                       Text{" … ", " … "}, numText(range[1], 0, "°C"), Text{".", "."}));
        }
        return r;
    };
    return c;
}

Calculator rtdWireError() {
    Calculator c;
    c.id = "rtd_wire_error";
    c.category = "automation";
    c.title = {"Leitungsfehler Pt/Ni-Fühler", "RTD lead wire error"};
    c.summary = {"Messfehler durch Zuleitung bei 2-Leiter-Anschluss", "Error from leads in 2-wire connection"};
    c.description = {
        "2-Leiter: Der Leitungswiderstand R_L = 2 · ρ · L / A addiert sich zum Fühler.\n"
        "Fehler ΔT = R_L / (dR/dT). 3-Leiter kompensiert bei gleichen Adern, 4-Leiter vollständig.\n"
        "Pt100 im 2-Leiter-Anschluss nur bei sehr kurzen Leitungen; GA meist Pt1000/Ni1000.",
        "2-wire: lead resistance R_L = 2 · ρ · L / A adds to the sensor.\n"
        "Error ΔT = R_L / (dR/dT). 3-wire compensates with equal leads, 4-wire completely.\n"
        "Use Pt100 2-wire only with very short leads; BMS usually uses Pt1000/Ni1000."};
    c.inputs = {
        Num::choice("type", {"Fühlertyp", "Sensor type"}, rtdOptions(), "pt1000"),
        Num::choice("wires", {"Anschluss", "Connection"},
                    {{"2", {"2-Leiter", "2-wire"}}, {"3", {"3-Leiter", "3-wire"}}, {"4", {"4-Leiter", "4-wire"}}}, "2"),
        Num::number("L", {"Leitungslänge (einfach)", "Cable length (one way)"}, "m", 50).nonNegative(),
        Num::number("A", {"Querschnitt", "Cross-section"}, "mm²", 0.5).positive(),
        Num::number("T", {"Messtemperatur", "Measured temperature"}, "°C", 20).range(-200, 850),
    };
    c.outputs = {Out::number("RL", {"Leitungswiderstand (Hin + Rück)", "Lead resistance (loop)"}, "Ω", 3),
                 Out::number("dT", {"Messfehler ΔT", "Measurement error ΔT"}, "K", 2).primary()};
    c.keywords = {"leitungsfehler", "2-leiter", "zweileiter", "pt100", "pt1000", "lead wire"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto type = parseRtd(in.choice("type"));
        const double RL = 2.0 * cables::resistivity(cables::Material::Cu, 20.0) * in.num("L") / in.num("A");
        const double err = in.choice("wires") == "2" ? RL / rtdSensitivity(type, in.num("T")) : 0.0;
        r.set("RL", RL);
        r.set("dT", err);
        if (in.choice("wires") != "2") {
            r.info({"Leitungswiderstand wird kompensiert (gleiche Adern vorausgesetzt).", "Lead resistance is compensated (equal leads assumed)."});
        } else if (err > 0.5) {
            r.warn({"Fehler > 0,5 K – Leitungsabgleich, 3-/4-Leiter oder Pt1000 verwenden.",
                    "Error > 0.5 K – compensate, use 3/4-wire or Pt1000."});
        } else {
            r.ok({"Fehler ≤ 0,5 K.", "Error ≤ 0.5 K."});
        }
        return r;
    };
    return c;
}

Calculator ntc() {
    Calculator c;
    c.id = "ntc";
    c.category = "automation";
    c.title = {"NTC-Thermistor (β-Modell)", "NTC thermistor (beta model)"};
    c.summary = {"Temperatur ↔ Widerstand für NTC 10k, 20k …", "Temperature ↔ resistance for NTC 10k, 20k …"};
    c.description = {"R(T) = R25 · exp(β · (1/T − 1/298,15 K))\nTypisch: NTC10k β = 3435 K oder 3950 K, NTC20k β = 4250 K (Herstellerangaben!).",
                     "R(T) = R25 · exp(β · (1/T − 1/298.15 K))\nTypical: NTC10k β = 3435 K or 3950 K, NTC20k β = 4250 K (check datasheet!)."};
    c.inputs = {
        Num::number("R25", {"Nennwiderstand R25", "Nominal resistance R25"}, "kΩ", 10).positive(),
        Num::number("B", {"β-Wert (B25/85)", "Beta value (B25/85)"}, "K", 3950).positive(),
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"T", {"Temperatur", "Temperature"}}, {"R", {"Widerstand", "Resistance"}}}, "T"),
        Num::number("T", {"Temperatur T", "Temperature T"}, "°C", 20).range(-60, 200).showIf("dir", {"T"}),
        Num::number("R", {"Widerstand R", "Resistance R"}, "kΩ", 12.0).positive().showIf("dir", {"R"}),
    };
    c.outputs = {Out::number("R", {"Widerstand R", "Resistance R"}, "kΩ", 3),
                 Out::number("T", {"Temperatur T", "Temperature T"}, "°C", 2)};
    c.keywords = {"ntc", "thermistor", "10k", "temperatur", "beta"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double R25 = in.num("R25");
        const double B = in.num("B");
        if (in.choice("dir") == "R") {
            const double T = ntcTemperature(R25, B, in.num("R"));
            r.set("T", T);
            r.set("R", in.num("R"));
        } else {
            r.set("T", in.num("T"));
            r.set("R", ntcResistance(R25, B, in.num("T")));
        }
        return r;
    };
    return c;
}

Calculator psu24() {
    Calculator c;
    c.id = "psu_24v";
    c.category = "automation";
    c.title = {"24-V-Versorgung dimensionieren", "24 V supply sizing"};
    c.summary = {"Trafo (AC) oder Netzteil (DC) für Feldgeräte", "Transformer (AC) or PSU (DC) for field devices"};
    c.description = {
        "P_erf = Σ(n · P) · g · (1 + Reserve)\n"
        "Stellantriebe nach Dimensionierungsleistung (VA) laut Datenblatt, nicht nach Betriebsleistung. "
        "AC-Trafo-Normgrößen: 25 … 1000 VA; DC-Netzteile 2,5 … 40 A bei 24 V.",
        "P_req = Σ(n · P) · g · (1 + margin)\n"
        "Size actuators by their dimensioning power (VA) from the datasheet, not operating power. "
        "AC transformer sizes: 25 … 1000 VA; DC PSUs 2.5 … 40 A at 24 V."};
    c.inputs = {
        Num::choice("type", {"Versorgung", "Supply"},
                    {{"ac", {"24 V AC Trafo (VA)", "24 V AC transformer (VA)"}}, {"dc", {"24 V DC Netzteil (W)", "24 V DC PSU (W)"}}},
                    "ac"),
        Num::number("n1", {"Anzahl Gruppe 1", "Quantity group 1"}, "", 10).nonNegative(),
        Num::number("p1", {"Leistung je Gerät Gr. 1", "Power per device gr. 1"}, "VA / W", 5).nonNegative(),
        Num::number("n2", {"Anzahl Gruppe 2", "Quantity group 2"}, "", 6).nonNegative(),
        Num::number("p2", {"Leistung je Gerät Gr. 2", "Power per device gr. 2"}, "VA / W", 3).nonNegative(),
        Num::number("n3", {"Anzahl Gruppe 3", "Quantity group 3"}, "", 0).nonNegative(),
        Num::number("p3", {"Leistung je Gerät Gr. 3", "Power per device gr. 3"}, "VA / W", 0).nonNegative(),
        Num::number("g", {"Gleichzeitigkeitsfaktor g", "Simultaneity factor g"}, "", 1.0).rangeExclusiveMin(0, 1),
        Num::number("res", {"Reserve", "Margin"}, "%", 25).range(0, 200),
    };
    c.outputs = {Out::number("sum", {"Summe angeschlossen", "Connected total"}, "VA", 1),
                 Out::number("req", {"Erforderlich", "Required"}, "VA", 1),
                 Out::number("size", {"Empfohlene Normgröße", "Recommended standard size"}, "VA", 0).primary(),
                 Out::number("I", {"Strom bei 24 V (Normgröße)", "Current at 24 V (standard size)"}, "A", 2)};
    c.keywords = {"24v", "trafo", "netzteil", "versorgung", "stellantrieb", "power supply", "transformer"};
    c.compute = [](const Inputs& in, const Context&) {
        static constexpr std::array<double, 10> kTrafo{25, 40, 63, 100, 160, 250, 400, 630, 1000, 1600};
        static constexpr std::array<double, 6> kPsuA{2.5, 5, 10, 20, 30, 40};
        Result r;
        const bool dc = in.choice("type") == "dc";
        const std::string u = dc ? "W" : "VA";
        const double sum = in.num("n1") * in.num("p1") + in.num("n2") * in.num("p2") + in.num("n3") * in.num("p3");
        const double req = sum * in.num("g") * (1.0 + in.num("res") / 100.0);
        r.set("sum", sum, u);
        r.set("req", req, u);
        if (dc) {
            const auto a = nextAtLeast(kPsuA, req / 24.0);
            if (a) {
                r.set("size", *a * 24.0, u);
                r.set("I", *a);
            }
        } else {
            const auto s = nextAtLeast(kTrafo, req);
            if (s) {
                r.set("size", *s, u);
                r.set("I", *s / 24.0);
            }
        }
        if (!r.values.contains("size")) {
            r.warn({"Größer als die größte Normstufe – auf mehrere Versorgungen aufteilen.",
                    "Larger than the largest standard size – split into several supplies."});
        }
        return r;
    };
    return c;
}

Calculator valveKv() {
    Calculator c;
    c.id = "valve_kv";
    c.category = "automation";
    c.title = {"Ventil kv/kvs & Ventilautorität", "Valve kv/kvs & authority"};
    c.summary = {"kv-Wert, kvs-Auswahl, Δp, Autorität – auch aus Leistung/ΔT", "kv value, kvs selection, Δp, authority – also from power/ΔT"};
    c.description = {
        "V̇ = P · 3600 / (ρ · c · ΔT)   (Angaben der Ventilliste nach VDI 3814 Blatt 4.1, Tab. 8: Leistung, VL/RL, ΔT)\n"
        "kv = V̇ · √(ρ/1000 / Δp[bar])    Δp_V100 = (V̇ / kvs)² · ρ/1000 · 100 kPa\n"
        "Ventilautorität a = Δp_V100 / (Δp_V100 + Δp_MV)\n"
        "Δp_MV = Druckverlust der mengenvariablen Strecke. Empfehlung a ≥ 0,5 (mind. 0,3).\n"
        "Wasser: ρ = 1000 kg/m³, c = 4,187 kJ/(kg·K). Glykolgemische: ρ und c aus dem Datenblatt des Herstellers.\n"
        "Kleinste regelbare Menge: V̇_min/V̇_100 = 1/√(1 + a·(S_v² − 1)), S_v = Stellverhältnis kvs/kvr; Wärmeleistung mit "
        "Wärmetauscher-Kennwert a_WT: Q̇/Q̇_100 = 1/(1 + a_WT·(V̇_100/V̇ − 1)). Beispiel a = 0,5, S_v = 50: ≈ 3 % (ohne WT), "
        "≈ 4 % (a_WT = 0,7). Zwei parallele Ventile 1/3 + 2/3 in Sequenz senken die minimale Leistung auf etwa ein Drittel "
        "(QM Holzheizwerke, FAQ 37, 2015).",
        "V̇ = P · 3600 / (ρ · c · ΔT)   (data of the valve list per VDI 3814 Part 4.1, Tab. 8: power, flow/return, ΔT)\n"
        "kv = V̇ · √(ρ/1000 / Δp[bar])    Δp_V100 = (V̇ / kvs)² · ρ/1000 · 100 kPa\n"
        "Valve authority a = Δp_V100 / (Δp_V100 + Δp_MV)\n"
        "Δp_MV = pressure drop of the variable-flow section. Recommended a ≥ 0.5 (min. 0.3).\n"
        "Water: ρ = 1000 kg/m³, c = 4.187 kJ/(kg·K). Glycol mixtures: take ρ and c from the manufacturer's data sheet.\n"
        "Smallest controllable flow: V̇_min/V̇_100 = 1/√(1 + a·(S_v² − 1)), S_v = rangeability kvs/kvr; heat output with heat "
        "exchanger characteristic a_HX: Q̇/Q̇_100 = 1/(1 + a_HX·(V̇_100/V̇ − 1)). Example a = 0.5, S_v = 50: ≈ 3 % (no HX), "
        "≈ 4 % (a_HX = 0.7). Two parallel valves 1/3 + 2/3 in sequence reduce the minimum output to about one third "
        "(QM Holzheizwerke, FAQ 37, 2015)."};
    c.inputs = {
        Num::choice("mode", {"Auslegung aus", "Sizing from"},
                    {{"flow", {"Volumenstrom", "Flow rate"}}, {"power", {"Leistung und ΔT (Ventilliste)", "Power and ΔT (valve list)"}}},
                    "flow"),
        Num::number("Q", {"Volumenstrom V̇", "Flow rate V̇"}, "m³/h", 2.5).positive().showIf("mode", {"flow"}),
        Num::number("P", {"Leistung P", "Power P"}, "kW", 50).positive().showIf("mode", {"power"}),
        Num::number("dT", {"Temperaturdifferenz ΔT (VL − RL)", "Temperature difference ΔT (flow − return)"}, "K", 20)
            .positive()
            .showIf("mode", {"power"}),
        Num::choice("medium", {"Medium", "Medium"},
                    {{"water", {"Wasser", "Water"}}, {"custom", {"Eigene Stoffwerte (z. B. Glykol)", "Custom (e.g. glycol)"}}}, "water"),
        Num::number("rho", {"Dichte ρ", "Density ρ"}, "kg/m³", 1040).positive().showIf("medium", {"custom"}),
        Num::number("c", {"Spez. Wärmekapazität c", "Specific heat c"}, "kJ/(kg·K)", 3.8).positive().showIf("medium", {"custom"})
            .withHint({"Nur für Auslegung aus Leistung", "Only for sizing from power"}),
        Num::number("dp", {"Gewünschter Δp am Ventil", "Desired Δp across valve"}, "kPa", 10).positive(),
        Num::number("dpmv", {"Δp mengenvariable Strecke", "Δp variable-flow section"}, "kPa", 10).nonNegative(),
        Num::number("sv", {"Stellverhältnis S_v (kvs/kvr)", "Rangeability S_v (kvs/kvr)"}, "", 50)
            .range(5, 1000)
            .startsSection({"Kleinste regelbare Leistung", "Smallest controllable output"})
            .withHint({"Datenblatt; typisch 50 … 100", "Data sheet; typically 50 … 100"}),
        Num::number("awt", {"Wärmetauscher-Kennwert a_WT", "Heat exchanger characteristic a_HX"}, "", 1.0)
            .rangeExclusiveMin(0, 1)
            .withHint({"1 = ohne Wärmetauscher; Heizregister ≈ 0,7", "1 = no heat exchanger; heating coil ≈ 0.7"}),
    };
    c.outputs = {Out::number("Qv", {"Volumenstrom V̇", "Flow rate V̇"}, "m³/h", 3),
                 Out::number("kv", {"Berechneter kv-Wert", "Calculated kv"}, "m³/h", 2),
                 Out::number("kvs", {"Gewählter kvs-Wert (Normreihe)", "Selected kvs (standard series)"}, "m³/h", 2).primary(),
                 Out::number("dpv", {"Δp bei kvs (voll offen)", "Δp at kvs (fully open)"}, "kPa", 2),
                 Out::number("a", {"Ventilautorität a", "Valve authority a"}, "", 2).primary(),
                 Out::number("vmin", {"Kleinster regelbarer Volumenstrom", "Smallest controllable flow"}, "%", 1),
                 Out::number("qmin", {"Kleinste regelbare Wärmeleistung", "Smallest controllable heat output"}, "%", 1)};
    c.keywords = {"ventil", "kvs", "kv", "autorität", "hydraulik", "valve", "authority", "hlk", "ventilliste", "vdi 3814", "glykol",
                  "stellverhältnis", "1/3-2/3", "rangeability"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const bool custom = in.choice("medium") == "custom";
        const double rho = custom ? in.num("rho") : 1000.0;
        const double cp = custom ? in.num("c") : 4.187;
        const double Q = in.choice("mode") == "power" ? volumeFlowFromPower(in.num("P"), in.num("dT"), rho, cp) : in.num("Q");
        const double kv = kvValue(Q, in.num("dp"), rho);
        const double kvs = nearestLog(kKvsSeries, kv);
        const double dpv = pressureDrop(Q, kvs, rho);
        const double a = dpv / (dpv + in.num("dpmv"));
        r.set("Qv", Q);
        r.set("kv", kv);
        r.set("kvs", kvs);
        r.set("dpv", dpv);
        r.set("a", a);
        const double vmin = valves::minFlowRatio(a, in.num("sv"));
        const double qmin = valves::heatRatio(vmin, in.num("awt"));
        r.set("vmin", 100.0 * vmin);
        r.set("qmin", 100.0 * qmin);
        if (qmin > 0.05) {
            r.info({"Kleinste regelbare Leistung > 5 %: darunter Auf-Zu-Betrieb. Abhilfe z. B. zwei Ventile 1/3 + 2/3 in Sequenz.",
                    "Smallest controllable output > 5 %: below that on/off operation. Remedy e.g. two valves 1/3 + 2/3 in sequence."});
        }
        if (a >= 0.5) {
            r.ok({"Ventilautorität gut (a ≥ 0,5).", "Good valve authority (a ≥ 0.5)."});
        } else if (a >= 0.3) {
            r.warn({"Ventilautorität ausreichend (0,3 ≤ a < 0,5).", "Acceptable valve authority (0.3 ≤ a < 0.5)."});
        } else {
            r.error({"Ventilautorität zu klein (a < 0,3) – kleineres kvs wählen.", "Valve authority too low (a < 0.3) – choose smaller kvs."});
        }
        return r;
    };
    return c;
}

Calculator heatFlow() {
    Calculator c;
    c.id = "heat_flow";
    c.category = "automation";
    c.title = {"Wärmeleistung ↔ Volumenstrom", "Heat output ↔ flow rate"};
    c.summary = {"Wasser oder Luft: P = V̇ · ρ · c · ΔT", "Water or air: P = V̇ · ρ · c · ΔT"};
    c.description = {
        "P = V̇ · ρ · c · ΔT\nWasser: ρ = 1000 kg/m³, c = 4,187 kJ/(kg·K) → V̇ [m³/h] = P [kW] / (1,163 · ΔT)\n"
        "Luft: ρ = 1,2 kg/m³, c = 1,005 kJ/(kg·K) → P [W] ≈ 0,335 · V̇ [m³/h] · ΔT",
        "P = V̇ · ρ · c · ΔT\nWater: ρ = 1000 kg/m³, c = 4.187 kJ/(kg·K) → V̇ [m³/h] = P [kW] / (1.163 · ΔT)\n"
        "Air: ρ = 1.2 kg/m³, c = 1.005 kJ/(kg·K) → P [W] ≈ 0.335 · V̇ [m³/h] · ΔT"};
    c.inputs = {
        Num::choice("medium", {"Medium", "Medium"},
                    {{"water", {"Wasser", "Water"}}, {"air", {"Luft", "Air"}}, {"custom", {"Eigene Stoffwerte (z. B. Glykol)", "Custom (e.g. glycol)"}}},
                    "water"),
        Num::choice("dir", {"Gegeben", "Given"},
                    {{"P", {"Leistung P", "Power P"}}, {"V", {"Volumenstrom V̇", "Flow rate V̇"}}}, "P"),
        Num::number("P", {"Leistung P", "Power P"}, "kW", 10).positive().showIf("dir", {"P"}),
        Num::number("V", {"Volumenstrom V̇", "Flow rate V̇"}, "m³/h", 0.43).positive().showIf("dir", {"V"}),
        Num::number("dT", {"Temperaturdifferenz ΔT", "Temperature difference ΔT"}, "K", 20).positive(),
        Num::number("rho", {"Dichte ρ", "Density ρ"}, "kg/m³", 1040).positive().showIf("medium", {"custom"}),
        Num::number("c", {"Spez. Wärmekapazität c", "Specific heat c"}, "kJ/(kg·K)", 3.8).positive().showIf("medium", {"custom"}),
    };
    c.outputs = {Out::number("P", {"Leistung P", "Power P"}, "kW", 3),
                 Out::number("V", {"Volumenstrom V̇", "Flow rate V̇"}, "m³/h", 3).primary(),
                 Out::number("Vls", {"Volumenstrom V̇", "Flow rate V̇"}, "l/s", 3),
                 Out::number("m", {"Massenstrom ṁ", "Mass flow ṁ"}, "kg/h", 1)};
    c.keywords = {"volumenstrom", "wärmeleistung", "heizung", "kälte", "luft", "wasser", "flow", "heat"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        double rho = 1000.0, cp = 4.187;
        if (in.choice("medium") == "air") {
            rho = 1.2;
            cp = 1.005;
        } else if (in.choice("medium") == "custom") {
            rho = in.num("rho");
            cp = in.num("c");
        }
        double P = 0, V = 0;
        if (in.choice("dir") == "V") {
            V = in.num("V");
            P = powerFromVolumeFlow(V, in.num("dT"), rho, cp);
        } else {
            P = in.num("P");
            V = volumeFlowFromPower(P, in.num("dT"), rho, cp);
        }
        r.set("P", P);
        r.set("V", V);
        r.set("Vls", V / 3.6);
        r.set("m", V * rho);
        return r;
    };
    return c;
}

Calculator daliBus() {
    Calculator c;
    c.id = "dali_bus";
    c.category = "automation";
    c.title = {"DALI-Leitung (Spannungsfall)", "DALI line (voltage drop)"};
    c.summary = {"Querschnitt für max. 2 V Spannungsfall", "Cross-section for max. 2 V drop"};
    c.description = {
        "DALI (IEC 62386): max. 2 V Spannungsfall auf der Busleitung, max. 250 mA, max. 300 m.\n"
        "A_min = 2 · L · I · ρ / ΔU_max\nRichtwerte: bis 100 m 0,5 mm², bis 150 m 0,75 mm², bis 300 m 1,5 mm².",
        "DALI (IEC 62386): max. 2 V drop on the bus line, max. 250 mA, max. 300 m.\n"
        "A_min = 2 · L · I · ρ / ΔU_max\nGuide values: up to 100 m 0.5 mm², 150 m 0.75 mm², 300 m 1.5 mm²."};
    c.inputs = {
        Num::number("L", {"Leitungslänge (einfach)", "Line length (one way)"}, "m", 150).positive(),
        Num::number("I", {"Busstrom", "Bus current"}, "mA", 250).positive(),
        Num::number("dU", {"Max. Spannungsfall", "Max. voltage drop"}, "V", 2).positive(),
    };
    c.outputs = {Out::number("Amin", {"Min. Querschnitt", "Min. cross-section"}, "mm²", 3),
                 Out::number("A", {"Empfohlener Querschnitt", "Recommended cross-section"}, "mm²", 2).primary(),
                 Out::number("dUsel", {"Spannungsfall bei Empfehlung", "Voltage drop at recommendation"}, "V", 2)};
    c.keywords = {"dali", "bus", "beleuchtung", "lighting"};
    c.compute = [](const Inputs& in, const Context&) {
        static constexpr std::array<double, 5> kAreas{0.5, 0.75, 1.0, 1.5, 2.5};
        Result r;
        const double rho = cables::resistivity(cables::Material::Cu, 30.0);
        const double I = in.num("I") / 1e3;
        const double Amin = 2.0 * in.num("L") * I * rho / in.num("dU");
        r.set("Amin", Amin);
        if (const auto A = nextAtLeast(kAreas, Amin)) {
            r.set("A", *A);
            r.set("dUsel", 2.0 * in.num("L") * I * rho / *A);
        }
        if (in.num("L") > 300) r.error({"DALI: max. 300 m Leitungslänge.", "DALI: max. 300 m line length."});
        if (in.num("I") > 250) r.error({"DALI: max. 250 mA Busstrom.", "DALI: max. 250 mA bus current."});
        return r;
    };
    return c;
}

}  // namespace

void addAutomation(std::vector<Calculator>& out) {
    out.push_back(analogScaling());
    out.push_back(loopBurden());
    out.push_back(rtd());
    out.push_back(rtdWireError());
    out.push_back(ntc());
    out.push_back(psu24());
    out.push_back(valveKv());
    out.push_back(heatFlow());
    out.push_back(daliBus());
}

}  // namespace elc::catalog
