// Calculators: Industrie & Netze / Industry & power
#include <cmath>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/motors.hpp"

namespace elc::catalog {

namespace {

Calculator pfCorrection() {
    Calculator c;
    c.id = "pf_correction";
    c.category = "industry";
    c.title = {"Blindleistungskompensation", "Power factor correction"};
    c.summary = {"Q_C und Kondensator von cos φ1 auf cos φ2", "Q_C and capacitor from cos φ1 to cos φ2"};
    c.description = {
        "Q_C = P · (tan φ1 − tan φ2)\nKapazität je Kondensator (U = Außenleiterspannung, ω = 2πf):\n"
        "Dreieck: C = Q_C / (3 · U² · ω)    Stern: C = Q_C / (U² · ω)    1~: C = Q_C / (U² · ω)\n"
        "Bei Oberschwingungen verdrosselte Anlagen vorsehen.",
        "Q_C = P · (tan φ1 − tan φ2)\nCapacitance per capacitor (U = line voltage, ω = 2πf):\n"
        "Delta: C = Q_C / (3 · U² · ω)    Star: C = Q_C / (U² · ω)    1~: C = Q_C / (U² · ω)\n"
        "Use detuned banks where harmonics are present."};
    c.inputs = {
        Num::number("P", {"Wirkleistung P", "Active power P"}, "kW", 100).positive(),
        Num::number("cos1", {"cos φ1 (Ist)", "cos φ1 (actual)"}, "", 0.75).rangeExclusiveMin(0, 1),
        Num::number("cos2", {"cos φ2 (Ziel)", "cos φ2 (target)"}, "", 0.95).rangeExclusiveMin(0, 1),
        Num::number("U", {"Spannung U", "Voltage U"}, "V", 400).positive(),
        Num::number("f", {"Frequenz f", "Frequency f"}, "Hz", 50).positive(),
        Num::choice("conn", {"Schaltung", "Connection"},
                    {{"delta", {"Drehstrom, Dreieck", "Three-phase, delta"}},
                     {"star", {"Drehstrom, Stern", "Three-phase, star"}},
                     {"single", {"Wechselstrom 1~", "Single-phase"}}},
                    "delta"),
    };
    c.outputs = {Out::number("Qc", {"Kompensationsleistung Q_C", "Compensation power Q_C"}, "kvar", 2).primary(),
                 Out::number("C", {"Kapazität je Kondensator", "Capacitance per capacitor"}, "µF", 1).primary(),
                 Out::number("S1", {"Scheinleistung vorher", "Apparent power before"}, "kVA", 2),
                 Out::number("S2", {"Scheinleistung nachher", "Apparent power after"}, "kVA", 2),
                 Out::number("I1", {"Strom vorher", "Current before"}, "A", 1),
                 Out::number("I2", {"Strom nachher", "Current after"}, "A", 1)};
    c.keywords = {"kompensation", "blindleistung", "cos phi", "kondensator", "power factor", "kvar"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double P = in.num("P") * 1e3;
        const double c1 = in.num("cos1");
        const double c2 = in.num("cos2");
        if (c2 <= c1) {
            throw CalcError({"cos φ2 muss größer als cos φ1 sein.", "cos φ2 must be greater than cos φ1."});
        }
        const auto& conn = in.choice("conn");
        const auto type = conn == "star" ? industry::CapConnection::Star
                                         : (conn == "single" ? industry::CapConnection::Single : industry::CapConnection::Delta);
        const double Qc = industry::compensationPower(P, c1, c2);
        const double k = type == industry::CapConnection::Single ? 1.0 : kSqrt3;
        r.set("Qc", Qc / 1e3);
        r.set("C", industry::capacitance(Qc, in.num("U"), in.num("f"), type) * 1e6);
        r.set("S1", P / c1 / 1e3);
        r.set("S2", P / c2 / 1e3);
        r.set("I1", P / c1 / (k * in.num("U")));
        r.set("I2", P / c2 / (k * in.num("U")));
        return r;
    };
    return c;
}

Calculator transformer() {
    Calculator c;
    c.id = "transformer";
    c.category = "industry";
    c.title = {"Transformator: Ströme & I_k", "Transformer: currents & I_k"};
    c.summary = {"Bemessungsströme, Übersetzung, Kurzschlussstrom", "Rated currents, ratio, short-circuit current"};
    c.description = {
        "I = S / (√3 · U) bei 3~ bzw. S / U bei 1~    ü = U1 / U2\n"
        "I_k ≈ c · I_r2 · 100 % / u_k   (starres Netz, c = 1,05);  Dauerkurzschlussstrom I_kD = I_r · 100 % / u_k (ohne c)\n"
        "Übliche u_k (Haag Band 05 Trafo, 2007): Verteiltrafos 10 … 20 kV 4 … 6 %, 110 kV 10 … 14 %, 380 kV 11 … 20 %. "
        "Beispiel Haag: 400 kVA, u_k = 4 % → I_r = 577 A, I_kD = 14,43 kA. „Spannungssteife“ Trafos (kleines u_k) liefern "
        "höhere Kurzschlussströme.",
        "I = S / (√3 · U) for 3~ or S / U for 1~    ratio = U1 / U2\n"
        "I_k ≈ c · I_r2 · 100 % / u_k   (infinite network, c = 1.05);  steady short-circuit current I_kD = I_r · 100 % / u_k (no c)\n"
        "Typical u_k (Haag vol. 05 transformers, 2007): distribution transformers 10 … 20 kV 4 … 6 %, 110 kV 10 … 14 %, "
        "380 kV 11 … 20 %. Haag example: 400 kVA, u_k = 4 % → I_r = 577 A, I_kD = 14.43 kA. “Stiff” transformers (small u_k) "
        "give higher short-circuit currents."};
    c.inputs = {
        Num::choice("sys", {"Ausführung", "Type"},
                    {{"ac3", {"Drehstrom", "Three-phase"}}, {"ac1", {"Einphasig (z. B. Steuertrafo)", "Single-phase (e.g. control)"}}},
                    "ac3"),
        Num::number("S", {"Bemessungsleistung S_r", "Rated power S_r"}, "kVA", 630).positive(),
        Num::number("U1", {"Oberspannung U_1", "Primary voltage U_1"}, "V", 20000).positive(),
        Num::number("U2", {"Unterspannung U_2", "Secondary voltage U_2"}, "V", 400).positive(),
        Num::number("uk", {"Kurzschlussspannung u_k", "Short-circuit voltage u_k"}, "%", 6).rangeExclusiveMin(0, 100),
    };
    c.outputs = {Out::number("I1", {"Bemessungsstrom primär", "Rated primary current"}, "A", 2),
                 Out::number("I2", {"Bemessungsstrom sekundär", "Rated secondary current"}, "A", 1).primary(),
                 Out::number("ratio", {"Übersetzung ü", "Ratio"}, "", 2),
                 Out::number("Ik", {"Kurzschlussstrom sekundär I_k", "Secondary short-circuit current I_k"}, "kA", 2).primary(),
                 Out::number("IkD", {"Dauerkurzschlussstrom I_kD (ohne c)", "Steady short-circuit current I_kD (no c)"}, "kA", 2)};
    c.keywords = {"trafo", "transformator", "transformer", "uk", "kva", "kurzschlussspannung", "haag"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const bool three = in.choice("sys") != "ac1";
        const double S = in.num("S") * 1e3;
        const double I2 = industry::transformerCurrent(S, in.num("U2"), three);
        r.set("I1", industry::transformerCurrent(S, in.num("U1"), three));
        r.set("I2", I2);
        r.set("ratio", in.num("U1") / in.num("U2"));
        r.set("Ik", 1.05 * I2 * 100.0 / in.num("uk") / 1e3);
        r.set("IkD", I2 * 100.0 / in.num("uk") / 1e3);
        return r;
    };
    return c;
}

Calculator shortCircuit() {
    Calculator c;
    c.id = "short_circuit";
    c.category = "industry";
    c.title = {"Kurzschlussstrom am Leitungsende", "Short-circuit current at cable end"};
    c.summary = {"I_k3 max und I_k1 min (Abschätzung IEC 60909)", "I_k3 max and I_k1 min (IEC 60909 estimate)"};
    c.description = {
        "Vereinfachte Abschätzung nach IEC 60909-0 (ohne Korrekturfaktor K_T):\n"
        "I_k3 = c_max · U_n / (√3 · |Z_Q + Z_T + Z_L|),  c_max = 1,05, Leiter bei 20 °C\n"
        "I_k1 = c_min · U_0 / |⅔Z_Q + Z_T + Z_L + Z_PE|,  c_min = 0,95, Leiter bei 80 °C (Dyn-Trafo)\n"
        "Z_Q = c · U_n² / S\"_kQ (X/R = 10), Z_T = u_k · U_n² / S_rT, R_T = u_R · U_n² / S_rT.\n"
        "Für die Planung verbindlich: Netzberechnungsprogramm bzw. Angaben des Netzbetreibers.",
        "Simplified estimate per IEC 60909-0 (without correction factor K_T):\n"
        "I_k3 = c_max · U_n / (√3 · |Z_Q + Z_T + Z_L|),  c_max = 1.05, conductors at 20 °C\n"
        "I_k1 = c_min · U_0 / |⅔Z_Q + Z_T + Z_L + Z_PE|,  c_min = 0.95, conductors at 80 °C (Dyn transformer)\n"
        "Z_Q = c · U_n² / S\"_kQ (X/R = 10), Z_T = u_k · U_n² / S_rT, R_T = u_R · U_n² / S_rT.\n"
        "For binding design use network calculation software or utility data."};
    c.inputs = {
        Num::number("S", {"Trafo-Bemessungsleistung S_rT", "Transformer rating S_rT"}, "kVA", 630).positive(),
        Num::number("uk", {"Kurzschlussspannung u_k", "Short-circuit voltage u_k"}, "%", 6).rangeExclusiveMin(0, 100),
        Num::number("uR", {"Ohmscher Anteil u_R", "Resistive part u_R"}, "%", 1).nonNegative(),
        Num::number("Un", {"Netzspannung U_n", "Nominal voltage U_n"}, "V", 400).positive(),
        Num::number("SkQ", {"Netzkurzschlussleistung S\"_kQ (leer = starr)", "Network fault level S\"_kQ (empty = infinite)"},
                    "MVA", 500)
            .opt()
            .positive(),
        Num::number("L", {"Leitungslänge L", "Cable length L"}, "m", 50).nonNegative(),
        Num::number("A", {"Querschnitt Außenleiter", "Phase cross-section"}, "mm²", 95).positive(),
        Num::number("Ape", {"Querschnitt PE/PEN", "PE/PEN cross-section"}, "mm²", 50).positive(),
        Num::choice("mat", {"Leitermaterial", "Conductor material"}, materialOptions(), "cu"),
        Num::number("x", {"Reaktanzbelag X'", "Reactance per length X'"}, "mΩ/m", 0.08).nonNegative(),
    };
    c.outputs = {Out::number("Ik3T", {"I_k3 an Trafoklemmen", "I_k3 at transformer terminals"}, "kA", 2),
                 Out::number("Ik3", {"I_k3 max am Leitungsende", "I_k3 max at cable end"}, "kA", 2).primary(),
                 Out::number("Ik1", {"I_k1 min am Leitungsende", "I_k1 min at cable end"}, "kA", 3).primary(),
                 Out::number("ZT", {"Impedanz Trafo Z_T", "Transformer impedance Z_T"}, "mΩ", 2),
                 Out::number("ZL", {"Impedanz Leitung Z_L (20 °C)", "Cable impedance Z_L (20 °C)"}, "mΩ", 2)};
    c.keywords = {"kurzschluss", "ik", "iec 60909", "short circuit", "ik3", "ik1"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        industry::ShortCircuitInput sc{in.num("S"),  in.num("uk"),  in.num("uR"), in.num("Un"),
                                       in.maybe("SkQ"), in.num("L"), in.num("A"),  in.num("Ape"),
                                       parseMaterial(in.choice("mat")), in.num("x")};
        if (sc.L_m <= 0) sc.L_m = 1e-9;
        const auto res = industry::shortCircuit(sc);
        r.set("Ik3T", res.Ik3Trafo_kA);
        r.set("Ik3", res.Ik3End_kA);
        r.set("Ik1", res.Ik1End_kA);
        r.set("ZT", res.ZT_mOhm);
        r.set("ZL", res.ZL_mOhm);
        r.info({"I_k3 max für Schaltvermögen der Geräte, I_k1 min für die Abschaltbedingung verwenden.",
                "Use I_k3 max for breaking capacity, I_k1 min for the disconnection check."});
        return r;
    };
    return c;
}

Calculator enclosureCooling() {
    Calculator c;
    c.id = "enclosure_cooling";
    c.category = "industry";
    c.title = {"Schaltschrank-Wärmebilanz", "Enclosure heat balance"};
    c.summary = {"Innentemperatur, Kühlleistung, Lüfter", "Internal temperature, cooling, fan"};
    c.description = {
        "Wirksame Oberfläche nach IEC/TR 60890 (DIN VDE 0660-507):\n"
        "freistehend: A = 1,8 · H · (B + T) + 1,4 · B · T    Wandmontage: A = 1,4 · B · (H + T) + 1,8 · T · H\n"
        "ΔT = P_V / (k · A)    Kühlleistung Q_K = P_V − k · A · (θ_i,max − θ_u)\n"
        "Lüfter: V̇ = 3,1 · Q / ΔT  [m³/h] (Meereshöhe). k ≈ 5,5 W/(m²·K) für Stahlblech.",
        "Effective surface per IEC/TR 60890:\n"
        "free-standing: A = 1.8 · H · (W + D) + 1.4 · W · D    wall-mounted: A = 1.4 · W · (H + D) + 1.8 · D · H\n"
        "ΔT = P_loss / (k · A)    Cooling demand Q_c = P_loss − k · A · (θ_i,max − θ_a)\n"
        "Fan: V̇ = 3.1 · Q / ΔT  [m³/h] (sea level). k ≈ 5.5 W/(m²·K) for sheet steel."};
    c.inputs = {
        Num::number("Pv", {"Verlustleistung P_V", "Heat loss P_loss"}, "W", 500).nonNegative(),
        Num::number("H", {"Höhe H", "Height H"}, "m", 2.0).positive(),
        Num::number("W", {"Breite B", "Width W"}, "m", 0.8).positive(),
        Num::number("D", {"Tiefe T", "Depth D"}, "m", 0.6).positive(),
        Num::choice("mount", {"Aufstellung", "Mounting"},
                    {{"free", {"Freistehend (allseitig frei)", "Free-standing"}},
                     {"wall", {"Wandmontage", "Wall-mounted"}}},
                    "free"),
        Num::number("k", {"Wärmedurchgangskoeffizient k", "Heat transfer coefficient k"}, "W/(m²·K)", 5.5).positive(),
        Num::number("Ta", {"Umgebungstemperatur θ_u", "Ambient temperature θ_a"}, "°C", 30).range(-40, 70),
        Num::number("Ti", {"Max. Innentemperatur θ_i", "Max. internal temperature θ_i"}, "°C", 35).range(0, 80),
    };
    c.outputs = {Out::number("A", {"Wirksame Oberfläche A", "Effective surface A"}, "m²", 2),
                 Out::number("dT", {"Übertemperatur ohne Kühlung ΔT", "Temperature rise without cooling ΔT"}, "K", 1),
                 Out::number("Tin", {"Innentemperatur ohne Kühlung", "Internal temperature without cooling"}, "°C", 1),
                 Out::number("Qk", {"Erforderliche Kühlleistung", "Required cooling capacity"}, "W", 0).primary(),
                 Out::number("fan", {"Lüfter-Volumenstrom (falls θ_u < θ_i)", "Fan airflow (if θ_a < θ_i)"}, "m³/h", 0)};
    c.keywords = {"schaltschrank", "klima", "kühlung", "wärme", "lüfter", "enclosure", "cooling", "rittal"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const double A = industry::enclosureSurface(in.num("H"), in.num("W"), in.num("D"), in.choice("mount") == "wall");
        const double kA = in.num("k") * A;
        const double dT = in.num("Pv") / kA;
        const double allowed = in.num("Ti") - in.num("Ta");
        const double Qk = in.num("Pv") - kA * allowed;
        r.set("A", A);
        r.set("dT", dT);
        r.set("Tin", in.num("Ta") + dT);
        r.set("Qk", std::max(0.0, Qk));
        if (Qk <= 0) {
            r.ok({"Keine aktive Kühlung erforderlich.", "No active cooling required."});
        } else if (allowed > 0) {
            r.set("fan", 3.1 * Qk / allowed);
            r.warn({"Kühlung nötig: Filterlüfter (Volumenstrom siehe oben) oder Kühlgerät.",
                    "Cooling required: filter fan (airflow above) or cooling unit."});
        } else {
            r.warn({"Umgebung ≥ zulässige Innentemperatur – Kühlgerät erforderlich (Lüfter reicht nicht).",
                    "Ambient ≥ allowed internal temperature – cooling unit required (fan is not sufficient)."});
        }
        return r;
    };
    return c;
}

}  // namespace

void addIndustry(std::vector<Calculator>& out) {
    out.push_back(pfCorrection());
    out.push_back(transformer());
    out.push_back(shortCircuit());
    out.push_back(enclosureCooling());
}

}  // namespace elc::catalog
