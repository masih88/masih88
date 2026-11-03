// Motor feeder for motors, pumps and fans (v1.10): what the outgoing circuit needs – protective device, contactors,
// starter or drive, cable, isolator, RCD type and accessories – sized from the motor current.
// Sources: Eaton Schaltungsbuch 06/11 (motor currents and fuses p. 10-43/44, trip classes p. 8-6, star-delta p. 8-37),
// Eaton PKZM0/PKZM4 setting ranges, Eaton "Starten und Steuern von Drehstrom-Asynchronmotoren" (starting currents),
// Danfoss "Wissenswertes über Frequenzumrichter" (RCD for drives, EN 50178), DIN VDE 0298-4 / IEC 60364-5-52 (cable).
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cables.hpp"
#include "elc/formulas/motors.hpp"

namespace elc::catalog {

using namespace elc::motors;

namespace {

std::vector<Option> methods() {
    return {{"C", {"C – direkt auf der Wand", "C – clipped direct on wall"}},
            {"E", {"E – gelochte Kabelrinne / frei in Luft", "E – perforated tray / free air"}},
            {"B2", {"B2 – im Rohr/Kanal auf der Wand", "B2 – in conduit/trunking on wall"}},
            {"D1", {"D1 – Kabel im Rohr im Erdreich", "D1 – cable in duct in the ground"}}};
}

cables::Method methodOf(const std::string& v) {
    if (v == "C") return cables::Method::C;
    if (v == "B2") return cables::Method::B2;
    if (v == "D1") return cables::Method::D1;
    return cables::Method::E;
}

struct CableChoice {
    double A = kNaN;
    double Iz = kNaN;
};

/// smallest Cu cross-section (PVC, 3 loaded conductors) with I_z ≥ I
CableChoice cableFor(double I, const Inputs& in) {
    const auto m = methodOf(in.choice("method"));
    const auto arr = in.choice("method") == "E" ? cables::Arrangement::PerforatedTray : cables::Arrangement::Bunched;
    const double f = cables::ambientFactor(cables::Insulation::PVC, m, in.num("Ta")) *
                     cables::groupingFactor(arr, static_cast<int>(std::round(in.num("groups"))));
    for (const double a : cables::kAmpacityAreas) {
        const auto base = cables::baseAmpacity(cables::Insulation::PVC, m, 3, a);
        if (base && *base * f >= I) return {a, *base * f};
    }
    return {};
}

Text amps(double v) { return numText(v, v < 10 ? 2 : 1, "A"); }

Calculator motorFeeder() {
    Calculator c;
    c.id = "motor_feeder";
    c.category = "motors";
    c.title = {"Motorabgang: Motor, Pumpe, Ventilator", "Motor feeder: motor, pump, fan"};
    c.summary = {"Was der Abgang braucht: MSS/LS, Schütze, Sanftanlasser/FU, Leitung, Reparaturschalter, RCD",
                 "What the feeder needs: MPCB/MCB, contactors, soft starter/drive, cable, isolator, RCD"};
    c.description = {
        "Motorstrom: Richtwerte für 4-polige Käfigläufer (Eaton Schaltungsbuch 06/11, S. 10-43/44) oder Typenschild.\n"
        "Anlaufstrom: direkt ≤ 6 · I_N (t_A ≤ 5 s), Stern-Dreieck ≤ 2 · I_N (t_A ≤ 15 s), Sanftanlauf ca. 2–6 · I_N, "
        "FU ≈ 1 (…2) · I_N.\n"
        "Überlastschutz I_r = I_N; Stern-Dreieck: Relais in der Motorleitung 0,58 · I_N (t_A ≤ 15 s), in der Netzzuleitung I_N "
        "(15–40 s), darüber in der Dreieckverbindung (im Anlauf kein Schutz). Schütze Y-Δ: Netz- und Dreieckschütz 0,58 · I_N, "
        "Sternschütz 0,33 · I_N (Eaton S. 8-37).\n"
        "Auslöseklasse IEC 60947-4-1 bei 7,2 · I_r: CLASS 10 > 4 s, 20 > 6 s, 30 > 9 s – die Hochlaufzeit muss darunter liegen.\n"
        "Motorschutzschalter: Einstellbereiche Eaton PKZM0 (0,1–32 A, kurzschlussfest bis 50 kA bei 400 V) und PKZM4 (bis 65 A).\n"
        "Leitung Cu/PVC mit I_z ≥ I_r (DIN VDE 0298-4, Temperatur- und Häufungsfaktor); bei FU/EC für den Leitungsschutz I_n.\n"
        "RCD bei Umrichtern mit B6-Gleichrichter: Typ B (allstromsensitiv), da glatte Gleichfehlerströme möglich sind "
        "(EN 50178, DIN VDE 0100-530). Herstellerangaben (Koordinationstabellen Typ 1/2) haben Vorrang.",
        "Motor current: guide values for 4-pole squirrel-cage motors (Eaton wiring manual 06/11, p. 10-43/44) or nameplate.\n"
        "Starting current: DOL ≤ 6 · I_N (t_A ≤ 5 s), star-delta ≤ 2 · I_N (t_A ≤ 15 s), soft start approx. 2–6 · I_N, "
        "drive ≈ 1 (…2) · I_N.\n"
        "Overload I_r = I_N; star-delta: relay in the motor line 0.58 · I_N (t_A ≤ 15 s), in the supply I_N (15–40 s), above "
        "that in the delta connection (no protection while starting). Y-Δ contactors: line and delta 0.58 · I_N, star 0.33 · I_N "
        "(Eaton p. 8-37).\n"
        "Trip class IEC 60947-4-1 at 7.2 · I_r: CLASS 10 > 4 s, 20 > 6 s, 30 > 9 s – the run-up time must stay below.\n"
        "Motor protective circuit-breaker: setting ranges Eaton PKZM0 (0.1–32 A, short-circuit proof up to 50 kA at 400 V) and "
        "PKZM4 (up to 65 A).\n"
        "Cable Cu/PVC with I_z ≥ I_r (IEC 60364-5-52, ambient and grouping factor); for drives/EC sized for the MCB I_n.\n"
        "RCD for drives with B6 rectifier: type B (all-current sensitive) since smooth DC fault currents are possible "
        "(EN 50178, IEC 60364-5-53). Manufacturer coordination tables (type 1/2) take precedence."};
    c.inputs = {
        Num::choice("app", {"Anwendung", "Application"},
                    {{"pump", {"Pumpe", "Pump"}}, {"fan", {"Ventilator", "Fan"}},
                     {"smoke", {"Entrauchungsventilator", "Smoke extraction fan"}}, {"motor", {"Antrieb allgemein", "General drive"}}},
                    "pump"),
        Num::choice("start", {"Anlaufart", "Starting method"},
                    {{"dol", {"Direktanlauf", "Direct-on-line"}},
                     {"yd", {"Stern-Dreieck-Anlauf", "Star-delta"}},
                     {"soft", {"Sanftanlasser", "Soft starter"}},
                     {"vfd", {"Frequenzumrichter (FU)", "Frequency converter (VFD)"}},
                     {"ec", {"EC-Motor (integrierte Elektronik)", "EC motor (integrated electronics)"}}},
                    "dol"),
        Num::choice("dir", {"Drehrichtung", "Direction"},
                    {{"one", {"eine Drehrichtung", "one direction"}}, {"two", {"Wendeschaltung (2 Drehrichtungen)", "reversing (2 directions)"}}},
                    "one"),
        Num::choice("concept", {"Schutzkonzept", "Protection concept"},
                    {{"mpcb", {"Motorschutzschalter (MSS)", "Motor protective circuit-breaker (MPCB)"}},
                     {"fuse", {"Sicherung + Schütz + Überlastrelais", "Fuse + contactor + overload relay"}}},
                    "mpcb"),
        Num::number("P", {"Motorleistung P_N", "Motor power P_N"}, "kW", 7.5).positive()
            .startsSection({"Motor", "Motor"}),
        Num::choice("U", {"Netz", "Supply"}, {{"400", "3~ 400 V"}, {"230", "3~ 230 V"}}, "400"),
        Num::choice("data", {"Motordaten", "Motor data"},
                    {{"table", {"Richtwerte (Eaton, 4-polig)", "Guide values (Eaton, 4-pole)"}}, {"plate", {"Typenschild", "Nameplate"}}},
                    "table"),
        Num::number("IN", {"Bemessungsstrom I_N (Typenschild)", "Rated current I_N (nameplate)"}, "A", 15.2).positive(),
        Num::number("cosphi", {"Leistungsfaktor cos φ (Typenschild)", "Power factor cos φ (nameplate)"}, "", 0.82).rangeExclusiveMin(0, 1),
        Num::number("ta", {"Hochlaufzeit t_A", "Run-up time t_A"}, "s", 3).positive()
            .withHint({"Pumpe meist < 3 s, Ventilator mit großem Laufrad deutlich länger", "Pumps usually < 3 s, large fans much longer"}),
        Num::number("L", {"Leitungslänge (einfach)", "Cable length (one way)"}, "m", 30).positive()
            .startsSection({"Leitung", "Cable"}),
        Num::choice("method", {"Verlegeart", "Installation method"}, methods(), "E"),
        Num::number("groups", {"Gehäufte Stromkreise", "Grouped circuits"}, "", 1).range(1, 20),
        Num::number("Ta", {"Umgebungstemperatur", "Ambient temperature"}, "°C", 30).range(-10, 60),
    };
    c.inputs[2].showIf("start", {"dol", "yd"});
    c.inputs[3].showIf("start", {"dol", "yd", "soft"});
    c.inputs[7].showIf("data", {"plate"});
    c.inputs[8].showIf("data", {"plate"});
    c.inputs[9].showIf("start", {"dol", "yd", "soft"});
    c.outputs = {
        Out::number("IN", {"Motorbemessungsstrom I_N", "Rated motor current I_N"}, "A", 1).primary(),
        Out::number("IA", {"Anlaufstrom (ca.)", "Starting current (approx.)"}, "A", 0),
        Out::number("Ir", {"Einstellwert Überlastschutz I_r", "Overload setting I_r"}, "A", 2).primary(),
        Out::text("prot", {"Kurzschluss- und Überlastschutz", "Short-circuit and overload protection"}),
        Out::number("fuse", {"Kleinste Vorsicherung gL/gG (Richtwert)", "Smallest gL/gG fuse (guide value)"}, "A", 0),
        Out::number("In", {"Leitungsschutzschalter I_n", "Circuit-breaker I_n"}, "A", 0),
        Out::text("contactor", {"Schütze (AC-3)", "Contactors (AC-3)"}),
        Out::text("starter", {"Anlaufgerät / Umrichter", "Starter / drive"}),
        Out::number("A", {"Leiterquerschnitt Cu", "Conductor cross-section Cu"}, "mm²", 1).primary(),
        Out::number("Iz", {"Belastbarkeit I_z", "Current rating I_z"}, "A", 1),
        Out::number("dU", {"Spannungsfall bei I_N", "Voltage drop at I_N"}, "%", 2),
        Out::text("cable", {"Leitung", "Cable"}),
        Out::text("switch", {"Reparaturschalter", "Local isolator"}),
        Out::text("rcd", {"Fehlerstromschutz (RCD)", "Residual current device (RCD)"}),
        Out::text("tclass", {"Auslöseklasse / Anordnung", "Trip class / arrangement"}),
        Out::text("extra", {"Zubehör und Hinweise", "Accessories and notes"}),
    };
    c.keywords = {"motorabgang", "abgang", "motorschutzschalter", "mss", "schütz", "überlastrelais", "pumpe", "ventilator", "lüfter",
                  "frequenzumrichter", "fu", "sanftanlasser", "stern-dreieck", "reparaturschalter", "motor feeder", "mpcb"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& start = in.choice("start");
        const auto& app = in.choice("app");
        const bool u400 = in.choice("U") == "400";
        const double U = u400 ? 400.0 : 230.0;
        double IN = 0, cosPhi = 0.85, fuseTable = kNaN;
        if (in.choice("data") == "table") {
            const auto m = motorGuide(in.num("P"));
            if (!m) throw CalcError({"Richtwerte nur bis 450 kW – Typenschilddaten eingeben.", "Guide values only up to 450 kW – enter nameplate data."});
            IN = u400 ? m->I400 : m->I230;
            if (!(IN > 0)) throw CalcError({"Für 230 V keine Richtwerte über 200 kW – Typenschilddaten eingeben.",
                                            "No 230 V guide values above 200 kW – enter nameplate data."});
            cosPhi = m->cosPhi;
            fuseTable = start == "yd" ? (u400 ? m->fuseYd400 : m->fuseYd230) : (u400 ? m->fuseDol400 : m->fuseDol230);
            if (std::fabs(m->P_kW - in.num("P")) > 1e-9) {
                r.info(cat(Text{"Richtwerte für die nächste Normleistung ", "Guide values for the next standard power "}, numText(m->P_kW, 2, "kW"),
                           Text{" (Eaton, 4-polig).", " (Eaton, 4-pole)."}));
            }
        } else {
            IN = in.num("IN");
            cosPhi = in.num("cosphi");
            for (const auto& m : motorGuideTable()) {  // fuse of the first table motor with at least this current
                const double I = u400 ? m.I400 : m.I230;
                if (I >= IN) {
                    fuseTable = start == "yd" ? (u400 ? m.fuseYd400 : m.fuseYd230) : (u400 ? m.fuseDol400 : m.fuseDol230);
                    break;
                }
            }
        }
        r.set("IN", IN);
        const double kA = start == "dol" ? 6.0 : start == "yd" ? 2.0 : start == "soft" ? 3.5 : 1.0;
        r.set("IA", kA * IN);
        const bool smoke = app == "smoke";
        const bool reversing = in.choice("dir") == "two" && (start == "dol" || start == "yd");
        const double ta = in.num("ta");

        // ---- overload / short-circuit protection
        double Ir = IN;
        Text prot, tclass;
        const bool useMpcb = in.choice("concept") == "mpcb";
        const auto mpcbText = [&](double I) {
            if (const auto rg = mpcbRange(I)) {
                return cat(Text{"Motorschutzschalter ", "Motor protective circuit-breaker "}, numText(rg->lo, rg->lo < 1 ? 2 : 1),
                           Text{"–", "–"}, numText(rg->hi, rg->hi < 1 ? 2 : 1, "A"), Text{" (z. B. Eaton PKZM), eingestellt auf ", " (e.g. Eaton PKZM), set to "},
                           amps(I));
            }
            return cat(Text{"Leistungsschalter für Motorschutz, I_r = ", "Circuit-breaker for motor protection, I_r = "}, amps(I));
        };
        if (start == "yd") {
            if (ta <= 15) {
                Ir = 0.58 * IN;
                tclass = {"Position A (Eaton): Überlastrelais in der Motorleitung, voller Schutz in Stern und Dreieck (t_A ≤ 15 s)",
                          "Position A (Eaton): overload relay in the motor line, full protection in star and delta (t_A ≤ 15 s)"};
                prot = cat(Text{"Überlastrelais F2 in der Motorleitung I_r = ", "Overload relay F2 in the motor line I_r = "}, amps(Ir),
                           useMpcb ? cat(Text{" + Kurzschlussschutz: ", " + short-circuit protection: "}, mpcbText(IN))
                                   : cat(Text{" + Sicherung gL/gG ≥ ", " + fuse gL/gG ≥ "}, numText(fuseTable, 0, "A")));
            } else if (ta <= 40) {
                Ir = IN;
                tclass = {"Position B (Eaton): Schutz in der Netzzuleitung (t_A 15–40 s), in Stern nur bedingt",
                          "Position B (Eaton): protection in the supply (t_A 15–40 s), only partly in star"};
                prot = useMpcb ? mpcbText(IN) : cat(Text{"Sicherung gL/gG ≥ ", "Fuse gL/gG ≥ "}, numText(fuseTable, 0, "A"),
                                                     Text{" + Überlastrelais in der Netzzuleitung I_r = ", " + overload relay in the supply I_r = "}, amps(IN));
            } else {
                Ir = 0.58 * IN;
                tclass = {"Position C (Eaton): Relais in der Dreieckverbindung – im Anlauf kein Motorschutz (Schweranlauf > 40 s)",
                          "Position C (Eaton): relay in the delta connection – no motor protection while starting (heavy start > 40 s)"};
                prot = cat(Text{"Überlastrelais in der Verbindung Dreieck-/Sternschütz I_r = ", "Overload relay between delta and star contactor I_r = "},
                           amps(Ir), Text{" + Kurzschlussschutz", " + short-circuit protection"});
                r.warn({"Hochlaufzeit > 40 s: Sanftanlasser oder FU prüfen.", "Run-up > 40 s: consider a soft starter or drive."});
            }
        } else if (start == "dol" || start == "soft") {
            const int cls = tripClassFor(ta);
            tclass = cls == 10   ? Text{"CLASS 10 (Normalanlauf, t_A < 4 s)", "CLASS 10 (normal start, t_A < 4 s)"}
                     : cls == 20 ? Text{"CLASS 20 (t_A < 6 s) – MSS/Relais für Schweranlauf", "CLASS 20 (t_A < 6 s) – MPCB/relay for heavy starting"}
                     : cls == 30 ? Text{"CLASS 30 (t_A < 9 s) – Schweranlauf", "CLASS 30 (t_A < 9 s) – heavy starting"}
                                 : Text{"Hochlauf > 9 s: Sanftanlasser/FU oder Sonderlösung", "Run-up > 9 s: soft starter/drive or special solution"};
            if (cls == 0) r.warn({"Hochlaufzeit über CLASS 30 (9 s) – Anlaufverfahren prüfen.", "Run-up time above CLASS 30 (9 s) – check the starting method."});
            prot = useMpcb ? mpcbText(IN)
                           : cat(Text{"Sicherung gL/gG ≥ ", "Fuse gL/gG ≥ "}, numText(fuseTable, 0, "A"),
                                 Text{" + Schütz + Überlastrelais I_r = ", " + contactor + overload relay I_r = "}, amps(IN),
                                 Text{" (max. Vorsicherung des Relais beachten)", " (observe max. back-up fuse of the relay)"});
            if (start == "soft") {
                prot = cat(prot, Text{"; Sanftanlasser: Koordination Typ 1/2 nach Herstellertabelle", "; soft starter: type 1/2 coordination per manufacturer table"});
            }
        } else {  // vfd, ec
            tclass = start == "vfd" ? Text{"Motorschutz elektronisch im FU (Motornennstrom parametrieren) + Kaltleiter (PTC)",
                                           "Motor protection electronically in the drive (set rated current) + thermistor (PTC)"}
                                    : Text{"Motorschutz in der EC-Elektronik integriert", "Motor protection integrated in the EC electronics"};
            const auto mcb = nextAtLeast(cables::ratedCurrents(cables::Device::MCB_C), IN);
            if (mcb) {
                r.set("In", *mcb);
                prot = cat(Text{"Leitungsschutzschalter C ", "Circuit-breaker C "}, numText(*mcb, 0, "A"),
                           Text{" oder gG nach Hersteller-Handbuch (Leitungsschutz der Zuleitung)", " or gG per manufacturer manual (protection of the supply cable)"});
            } else {
                const auto gg = nextAtLeast(cables::ratedCurrents(cables::Device::FuseGG), IN);
                if (gg) r.set("In", *gg);
                prot = cat(Text{"Sicherung gG/gR nach Hersteller-Handbuch, mindestens ", "Fuse gG/gR per manufacturer manual, at least "},
                           numText(gg.value_or(IN), 0, "A"));
            }
        }
        if (smoke) {
            prot = cat(prot, Text{"; Brandfall: Überlast nur melden, nicht abschalten", "; fire mode: signal overload, do not trip"});
        }
        r.set("Ir", Ir);
        r.setText("prot", prot);
        r.setText("tclass", tclass);
        if (fuseTable > 0 && (start == "dol" || start == "yd")) r.set("fuse", fuseTable);

        // ---- contactors and starter
        Text contactor, starter;
        if (start == "dol") {
            contactor = reversing ? cat(Text{"Wendeschützkombination K1/K2 je AC-3 ≥ ", "Reversing contactors K1/K2 each AC-3 ≥ "}, amps(IN),
                                        Text{", elektrisch und mechanisch verriegelt", ", electrically and mechanically interlocked"})
                                  : cat(Text{"Schütz K1 AC-3 ≥ ", "Contactor K1 AC-3 ≥ "}, amps(IN));
            starter = {"– (Direktanlauf)", "– (direct-on-line)"};
        } else if (start == "yd") {
            contactor = cat(Text{"Netzschütz K1 und Dreieckschütz K3 je AC-3 ≥ ", "Line contactor K1 and delta contactor K3 each AC-3 ≥ "},
                            amps(0.58 * IN), Text{", Sternschütz K2 ≥ ", ", star contactor K2 ≥ "}, amps(0.33 * IN),
                            reversing ? Text{"; Wendung: zweites Netzschütz", "; reversing: second line contactor"} : Text{"", ""});
            starter = {"Stern-Dreieck-Zeitrelais; Motor für Dreieck an der Netzspannung (z. B. 400/690 V an 400 V)",
                       "Star-delta timer relay; motor rated for delta at the supply voltage (e.g. 400/690 V on 400 V)"};
        } else if (start == "soft") {
            contactor = cat(Text{"optional Netzschütz AC-3 ≥ ", "optional line contactor AC-3 ≥ "}, amps(IN),
                            Text{" (Freischalten); Bypass meist im Sanftanlasser integriert", " (isolation); bypass usually integrated"});
            starter = cat(Text{"Sanftanlasser I_e ≥ ", "Soft starter I_e ≥ "}, amps(IN), Text{" für ", " for "}, numText(in.num("P"), 2, "kW"),
                          app == "pump" ? Text{", mit Sanftauslauf gegen Druckstöße", ", with soft stop against water hammer"} : Text{"", ""});
        } else if (start == "vfd") {
            contactor = {"kein Motorschütz; optional Netzschütz vor dem FU (nicht zum Takten verwenden)",
                         "no motor contactor; optional line contactor ahead of the drive (not for frequent switching)"};
            starter = cat(Text{"Frequenzumrichter I_aus ≥ ", "Drive I_out ≥ "}, amps(IN), Text{", P ≥ ", ", P ≥ "}, numText(in.num("P"), 2, "kW"),
                          app == "motor" ? Text{" (Überlast nach Lastprofil)", " (overload per load profile)"}
                                         : Text{" (Normal Duty, quadratisches Moment)", " (normal duty, quadratic torque)"},
                          Text{"; Netz- bzw. Zwischenkreisdrossel, EMV-Filter nach EN 61800-3", "; line or DC-link choke, EMC filter per EN 61800-3"});
        } else {
            contactor = {"kein Schütz – Freigabe und Sollwert über Steuereingang (0–10 V / Modbus)",
                         "no contactor – enable and set point via control input (0–10 V / Modbus)"};
            starter = {"EC-Motor mit integrierter Kommutierungselektronik", "EC motor with integrated commutation electronics"};
        }
        r.setText("contactor", contactor);
        r.setText("starter", starter);

        // ---- cable
        double Icable = start == "yd" ? IN : Ir;  // star-delta: supply carries I_N
        if (start == "vfd" || start == "ec") Icable = r.values.contains("In") ? r.values.at("In") : IN;  // protected by I_n
        const auto cab = cableFor(Icable, in);
        if (!(cab.A > 0)) throw CalcError({"Strom zu groß für die Belastbarkeitstabelle – Parallelkabel planen.",
                                           "Current too high for the ampacity table – plan parallel cables."});
        r.set("A", cab.A);
        r.set("Iz", cab.Iz);
        const double rho = cables::resistivity(cables::Material::Cu, 70.0);
        const double dU = cables::voltageDrop(System::AC3, in.num("L"), IN, cab.A, rho, cosPhi, 0.08e-3) / U * 100.0;
        r.set("dU", dU);
        Text cable = cat(Text{"Cu ", "Cu "}, numText(cab.A, cab.A < 10 ? 1 : 0, "mm²"), Text{" (I_z = ", " (I_z = "}, amps(cab.Iz),
                         Text{")", ")"});
        if (start == "yd") {
            const auto mot = cableFor(0.58 * IN, in);
            cable = cat(Text{"Zuleitung ", "Supply "}, cable, Text{"; Motorleitungen 2 × 3 Adern für 0,58 · I_N: Cu ", "; motor cables 2 × 3 cores for 0.58 · I_N: Cu "},
                        numText(mot.A, mot.A < 10 ? 1 : 0, "mm²"));
        }
        if (start == "vfd") {
            const auto mot = cableFor(IN, in);
            cable = cat(Text{"Netzzuleitung ", "Supply "}, cable, Text{"; Motorleitung geschirmt (EMV, Schirm 360° auflegen), Cu ", "; motor cable screened (EMC, 360° screen bonding), Cu "},
                        numText(mot.A, mot.A < 10 ? 1 : 0, "mm²"));
        }
        if (smoke) cable = cat(cable, Text{"; mit Funktionserhalt (DIN 4102-12, E30/E90)", "; with circuit integrity (DIN 4102-12, E30/E90)"});
        r.setText("cable", cable);
        if (dU > 4.0) r.warn({"Spannungsfall > 4 % – Querschnitt erhöhen (Anlauf beachten).", "Voltage drop > 4 % – increase the cross-section (mind the start)."});

        // ---- isolator, RCD, accessories
        Text sw = cat(Text{"Lasttrennschalter am Motor, AC-23A ≥ ", "Load-break switch at the motor, AC-23A ≥ "}, amps(IN), Text{", abschließbar", ", lockable"});
        if (start == "yd") sw = cat(sw, Text{" (6-polig oder in der Zuleitung vor dem Starter)", " (6-pole or in the supply ahead of the starter)"});
        if (start == "vfd" || start == "ec")
            sw = cat(sw, Text{"; voreilender Hilfskontakt auf die Freigabe (nicht unter Last schalten)", "; early-break auxiliary contact to the enable (do not switch under load)"});
        r.setText("switch", sw);
        r.setText("rcd", start == "vfd" || start == "ec"
                             ? Text{"Falls RCD gefordert: Typ B (allstromsensitiv) – Umrichter/EC mit B6-Gleichrichter können glatte Gleichfehlerströme "
                                    "erzeugen; Ableitströme der EMV-Filter beachten.",
                                    "If an RCD is required: type B (all-current sensitive) – drives/EC with B6 rectifier can cause smooth DC fault "
                                    "currents; mind the leakage currents of EMC filters."}
                             : Text{"Im TN-System meist Fehlerschutz durch Abschaltung (kein RCD nötig); falls gefordert (TT, Zusatzschutz): Typ A.",
                                    "In TN systems fault protection usually by disconnection (no RCD needed); if required (TT, additional protection): "
                                    "type A."});
        Text extra;
        if (app == "pump") {
            extra = {"Trockenlaufschutz (Druck-/Strömungswächter), Betriebs- und Störmeldung an die GA, Kaltleiter bei FU oder häufigem Schalten.",
                     "Dry-run protection (pressure/flow switch), run and fault signal to the BMS, thermistor with drive or frequent switching."};
        } else if (app == "fan") {
            extra = {"Hochlauf großer Laufräder dauert lange (Trägheitsmoment) – Auslöseklasse prüfen oder FU; Keilriemen-/Strömungsüberwachung "
                     "(Differenzdruckwächter), Reparaturschalter am Gerät.",
                     "Large impellers run up slowly (inertia) – check the trip class or use a drive; belt/airflow monitoring (differential "
                     "pressure switch), isolator at the unit."};
        } else if (app == "smoke") {
            extra = {"Entrauchung: Überlastschutz soll im Brandfall nicht abschalten, nur melden (DIN VDE 0100-430 Abschnitt 433.3.3, DIN VDE 0100-560); "
                     "Funktionserhalt der Leitung, Vorrang der Brandfallsteuerung – mit Brandschutzkonzept abstimmen.",
                     "Smoke extraction: overload protection should not trip in a fire, only signal (IEC 60364-4-43 clause 433.3.3, IEC 60364-5-56); "
                     "circuit integrity of the cable, priority of fire control – agree with the fire protection concept."};
            r.warn({"Entrauchungsventilator: Sonderanforderungen an Schutz und Leitung beachten.", "Smoke extraction fan: observe special requirements for protection and cable."});
        } else {
            extra = {"Kaltleiter bei Schweranlauf oder FU; bei Maschinen Not-Halt und Steuerung nach EN 60204-1.",
                     "Thermistor for heavy starting or drives; for machinery emergency stop and control per EN 60204-1."};
        }
        r.setText("extra", extra);
        return r;
    };
    return c;
}

Table motorGuideTableDef() {
    std::vector<std::vector<Text>> rows;
    const auto cell = [](double v, int d) { return std::isnan(v) ? Text{"–", "–"} : formatNumber(v, d); };
    for (const auto& m : motorGuideTable()) {
        rows.push_back({formatNumber(m.P_kW, 2), formatNumber(m.cosPhi, 2), formatNumber(m.eta_pct, 0), cell(m.I400, 2), cell(m.fuseDol400, 0),
                        cell(m.fuseYd400, 0), cell(m.I230, 2), cell(m.fuseDol230, 0), cell(m.fuseYd230, 0)});
    }
    Table t;
    t.id = "motor_rated_currents";
    t.category = "motors";
    t.title = {"Motorbemessungsströme und Absicherung", "Rated motor currents and fuses"};
    t.summary = {"Richtwerte 4-polige Käfigläufer 400 V / 230 V, Sicherung direkt und Stern-Dreieck",
                 "Guide values 4-pole squirrel-cage motors 400 V / 230 V, fuse DOL and star-delta"};
    t.description = {"Eaton Schaltungsbuch 06/11, S. 10-43/10-44: Richtwerte für innen- und oberflächengekühlte Drehstrommotoren mit "
                     "1500 min⁻¹. Kleinstmögliche Kurzschlusssicherung („träge“ bzw. gL, DIN VDE 0636) für Direktanlauf (I_A ≤ 6 · I_N, "
                     "t_A ≤ 5 s) und Stern-Dreieck (I_A ≤ 2 · I_N, t_A ≤ 15 s); der Höchstwert richtet sich nach Schaltgerät bzw. "
                     "Motorschutzrelais. Bei höherem Anlaufstrom oder längerer Anlaufzeit größere Sicherung; aM-Sicherung = Bemessungsstrom.",
                     "Eaton wiring manual 06/11, p. 10-43/10-44: guide values for internally and surface cooled three-phase motors at "
                     "1500 rpm. Smallest short-circuit fuse (slow / gL) for direct-on-line (I_A ≤ 6 · I_N, t_A ≤ 5 s) and star-delta "
                     "(I_A ≤ 2 · I_N, t_A ≤ 15 s); the maximum depends on the switching device or overload relay. Larger fuse for higher "
                     "starting current or longer run-up; aM fuse = rated current."};
    t.columns = {{"P in kW", "P in kW"}, {"cos φ", "cos φ"}, {"η in %", "η in %"}, {"I_N 400 V", "I_N 400 V"},
                 {"Sicherung direkt 400 V", "Fuse DOL 400 V"}, {"Sicherung Y-Δ 400 V", "Fuse Y-Δ 400 V"}, {"I_N 230 V", "I_N 230 V"},
                 {"Sicherung direkt 230 V", "Fuse DOL 230 V"}, {"Sicherung Y-Δ 230 V", "Fuse Y-Δ 230 V"}};
    t.rows = rows;
    return t;
}

}  // namespace

void addMotorFeeder(std::vector<Calculator>& out) { out.push_back(motorFeeder()); }
void addMotorFeederTables(std::vector<Table>& out) { out.push_back(motorGuideTableDef()); }

}  // namespace elc::catalog
