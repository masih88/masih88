// Area "Überspannungs- & Blitzschutz" (v1.11): applications after the DEHN white papers
//  - WPX035/DE/0122 "Blitz- und Überspannungsschutz für explosionsgefährdete Bereiche" (Ex i loops, Tabelle 1/2, Bild 4–7)
//  - WPX050/DE/0422 "Blitz- und Überspannungsschutzkonzept für Rechenzentren" (Tabelle 1–3, Bild 11/12/15, data sheets)
// Calculators: SPD in intrinsically safe loops, induced voltage in installation loops. Tables: SPD components with data
// sheet values, data centre protection proposal and requirements, Ex requirements, discharge capacity per LPL.
#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "catalog/surge_drawings.hpp"

namespace elc::catalog {

namespace {

constexpr const char* kCat = "surge";

// ------------------------------------------------------------------ SPD in intrinsically safe loops

struct ExZone {
    const char* id;
    const char* epl;
    std::vector<std::string> allowed;  // types of protection "i" permitted in the zone
};

const std::vector<ExZone>& exZones() {
    static const std::vector<ExZone> z{{"0", "Ga", {"ia"}},       {"1", "Gb", {"ia", "ib"}}, {"2", "Gc", {"ia", "ib", "ic"}},
                                       {"20", "Da", {"ia"}},      {"21", "Db", {"ia", "ib"}}, {"22", "Dc", {"ia", "ib", "ic"}}};
    return z;
}

Calculator spdEx() {
    Calculator c;
    c.id = "spd_ex";
    c.category = kCat;
    c.title = {"Überspannungsschutz im Ex-Bereich (eigensicherer Messkreis)", "Surge protection in hazardous areas (intrinsically safe loop)"};
    c.summary = {"Ableiter für Ex-i-Messkreise: Zündschutzart, U_i/I_i gegen U_o/I_o, Erdfreiheit, Einbau – nach DEHN WPX035",
                 "Arresters for Ex i loops: type of protection, U_i/I_i against U_o/I_o, floating circuits, installation – after DEHN WPX035"};
    c.description = {
        "Nach DEHN WPX035 „Blitz- und Überspannungsschutz für explosionsgefährdete Bereiche“ und DIN EN 60079-11/-14/-25, "
        "DIN EN 62305-4:\n"
        "• Einbauort: Übergang LPZ 0B → 1 am Feldgerät (z. B. Temperatur-Messumformer am Tank) und am anderen Ende der Feldleitung "
        "mit gleichem Ableitvermögen; nach dem Trennverstärker in der Warte (LPZ 2) ist kein weiteres SPD nötig, wenn der Schirm "
        "beidseitig aufgelegt ist.\n"
        "• Ableitvermögen mindestens 10 Impulse à 10 kA (8/20 µs) ohne Defekt; Ableiter im metallisch geschirmten Gehäuse, Erdung "
        "mindestens 4 mm² Cu; Leitung Ableiter → Betriebsmittel im beidseitig geerdeten Metallrohr oder geschirmt mit höchstens 1 m.\n"
        "• Zündschutzart: Zone 0/20 ia, Zone 1/21 ia oder ib, Zone 2/22 ia, ib oder ic; ein nach ia bescheinigter Ableiter eignet "
        "sich auch für ib/ic.\n"
        "• Zusammenschaltung: U_i (bzw. U_C) des Ableiters ≥ U_o des Speisegeräts, I_i (Nennstrom) ≥ I_o; innere Kapazität C_i und "
        "Induktivität L_i in den Nachweis der Eigensicherheit aufnehmen.\n"
        "• Erdfreiheit: Messumformer mit Isolationsfestigkeit ≥ 500 V AC gelten als erdfrei – der Ableiter darf das nicht "
        "aufheben; bei < 500 V AC gilt der Kreis als geerdet, dann Schutzpegel U_p (Ader–PG) bei 10 kA (8/20 µs) unterhalb der "
        "Isolationsfestigkeit (z. B. ≤ 35 V).\n"
        "• Endgeräte nach NAMUR NE 21: 0,5 kV Ader/Ader, 1 kV Ader/Erde (IEC 61000-4-5).\n"
        "Vorgabewerte = Beispiel Tabelle 2 (Messumformer U_o 29,4 V, I_o 130 mA; BLITZDUCTOR XT ML4 BD EX 24: ia, U_i 30 V, "
        "I_i 500 mA, 20 kA, U_p ≤ 52 V). Produktangaben gegen aktuelle Bescheinigung prüfen.",
        "After DEHN WPX035 \"Lightning and surge protection for hazardous areas\" and IEC 60079-11/-14/-25, IEC 62305-4:\n"
        "• Location: transition LPZ 0B → 1 at the field device (e.g. temperature transmitter on the tank) and at the other end of "
        "the field cable with the same discharge capacity; after the isolating amplifier in the control room (LPZ 2) no further "
        "SPD is needed if the screen is bonded at both ends.\n"
        "• Discharge capacity at least 10 impulses of 10 kA (8/20 µs) without defect; arrester in a metal-shielded enclosure, "
        "earthing at least 4 mm² Cu; line arrester → equipment in a metal conduit earthed at both ends or shielded and at most "
        "1 m long.\n"
        "• Type of protection: zone 0/20 ia, zone 1/21 ia or ib, zone 2/22 ia, ib or ic; an arrester certified ia also suits ib/ic.\n"
        "• Interconnection: U_i (or U_C) of the arrester ≥ U_o of the supply, I_i (rated current) ≥ I_o; include internal "
        "capacitance C_i and inductance L_i in the intrinsic safety verification.\n"
        "• Floating: transmitters with insulation ≥ 500 V AC count as floating – the arrester must not cancel this; below 500 V AC "
        "the loop counts as earthed, then protection level U_p (core–PG) at 10 kA (8/20 µs) below the insulation (e.g. ≤ 35 V).\n"
        "• Equipment per NAMUR NE 21: 0.5 kV core/core, 1 kV core/earth (IEC 61000-4-5).\n"
        "Defaults = example table 2 (transmitter U_o 29.4 V, I_o 130 mA; BLITZDUCTOR XT ML4 BD EX 24: ia, U_i 30 V, I_i 500 mA, "
        "20 kA, U_p ≤ 52 V). Check product data against the current certificate."};
    c.inputs = {
        Num::choice("zone", {"Ex-Zone am Feldgerät", "Hazardous zone at the field device"},
                    {{"0", "Zone 0"}, {"1", "Zone 1"}, {"2", "Zone 2"}, {"20", {"Zone 20 (Staub)", "Zone 20 (dust)"}},
                     {"21", {"Zone 21 (Staub)", "Zone 21 (dust)"}}, {"22", {"Zone 22 (Staub)", "Zone 22 (dust)"}}},
                    "1"),
        Num::number("Uo", {"U_o des Speisegeräts / Messkreises", "U_o of the supply / loop"}, "V", 29.4).positive()
            .withHint({"höchste Leerlaufspannung laut Bescheinigung", "maximum open-circuit voltage per certificate"}),
        Num::number("Io", {"I_o (Kurzschlussstrom im Fehlerfall)", "I_o (short-circuit current on fault)"}, "mA", 130).positive(),
        Num::choice("spdcat", {"Zündschutzart des Ableiters", "Type of protection of the arrester"}, {{"ia", "ia"}, {"ib", "ib"}, {"ic", "ic"}}, "ia"),
        Num::number("Ui", {"U_i bzw. U_C des Ableiters", "U_i or U_C of the arrester"}, "V", 30).positive(),
        Num::number("Ii", {"I_i bzw. Nennstrom des Ableiters", "I_i or rated current of the arrester"}, "mA", 500).positive(),
        Num::choice("iso", {"Isolation Messumformer gegen Erde", "Insulation transmitter to earth"},
                    {{"yes", {"≥ 500 V AC (erdfrei)", "≥ 500 V AC (floating)"}}, {"no", {"< 500 V AC (geerdet)", "< 500 V AC (earthed)"}}}, "yes"),
        Num::number("len", {"Leitung Ableiter → Feldgerät", "Line arrester → field device"}, "m", 1).nonNegative(),
        Num::choice("shield", {"Verlegung dieser Leitung", "Routing of this line"},
                    {{"shielded", {"geschirmt, Schirm beidseitig aufgelegt", "shielded, screen bonded at both ends"}},
                     {"pipe", {"im beidseitig geerdeten Metallrohr", "in a metal conduit earthed at both ends"}},
                     {"none", {"ungeschirmt, frei verlegt", "unshielded, open"}}},
                    "shielded"),
    };
    c.outputs = {Out::text("cat", {"Geforderte Zündschutzart", "Required type of protection"}).primary(),
                 Out::number("UiMin", {"U_i des Ableiters mindestens", "U_i of the arrester at least"}, "V", 1),
                 Out::number("IiMin", {"I_i des Ableiters mindestens", "I_i of the arrester at least"}, "mA", 0),
                 Out::text("lpz", {"Einbauort", "Location"}),
                 Out::text("capacity", {"Ableitvermögen", "Discharge capacity"}),
                 Out::text("up", {"Schutzpegel / Erdfreiheit", "Protection level / floating"}),
                 Out::text("line", {"Leitung zum Feldgerät", "Line to the field device"}),
                 Out::text("earth", {"Erdung und Schirm", "Earthing and screen"}),
                 Out::text("dehn", {"Beispiele DEHN", "DEHN examples"})};
    c.keywords = {"ex", "atex", "eigensicher", "ex i", "ia", "ib", "blitzductor", "dehnpipe", "tank", "messumformer", "namur",
                  "ne 21", "intrinsically safe", "hazardous area", "überspannungsschutz"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& zid = in.choice("zone");
        const ExZone* zone = &exZones().front();
        for (const auto& z : exZones()) {
            if (zid == z.id) zone = &z;
        }
        std::string allowed;
        for (std::size_t i = 0; i < zone->allowed.size(); ++i) allowed += (i == 0 ? "" : i + 1 == zone->allowed.size() ? " / " : ", ") + zone->allowed[i];
        r.setText("cat", {"Zone " + std::string(zone->id) + " → EPL " + zone->epl + ": " + allowed,
                          "Zone " + std::string(zone->id) + " → EPL " + zone->epl + ": " + allowed});
        const auto& sc = in.choice("spdcat");
        bool fail = false;
        if (std::find(zone->allowed.begin(), zone->allowed.end(), sc) == zone->allowed.end()) {
            r.error({"Ableiter „" + sc + "“ ist in Zone " + zid + " nicht zulässig – " + zone->allowed.front() + " erforderlich.",
                     "Arrester \"" + sc + "\" is not permitted in zone " + zid + " – " + zone->allowed.front() + " required."});
            fail = true;
        }
        const double uo = in.num("Uo"), io = in.num("Io");
        r.set("UiMin", uo);
        r.set("IiMin", io);
        if (in.num("Ui") < uo) {
            r.error({"U_i des Ableiters kleiner als U_o – Ableiter würde im Betrieb ansprechen bzw. überlastet.",
                     "U_i of the arrester below U_o – the arrester would conduct or be overloaded in operation."});
            fail = true;
        }
        if (in.num("Ii") < io) {
            r.error({"I_i des Ableiters kleiner als I_o – unzulässige Erwärmung im Fehlerfall, Eigensicherheit nicht nachgewiesen.",
                     "I_i of the arrester below I_o – inadmissible heating on fault, intrinsic safety not verified."});
            fail = true;
        }
        r.setText("lpz", {"Übergang LPZ 0B → 1: am Feldgerät und am Eintritt in die Messwarte (gleiches Ableitvermögen); in der Warte "
                          "nach dem Trennverstärker kein weiteres SPD, wenn der Schirm beidseitig aufgelegt ist",
                          "Transition LPZ 0B → 1: at the field device and at the entry to the control room (same discharge capacity); "
                          "no further SPD after the isolating amplifier if the screen is bonded at both ends"});
        r.setText("capacity", {"mindestens 10 Impulse à 10 kA (8/20 µs) ohne Defekt (DIN EN 60079-14)",
                               "at least 10 impulses of 10 kA (8/20 µs) without defect (IEC 60079-14)"});
        if (in.choice("iso") == "yes") {
            r.setText("up", {"Messumformer erdfrei (≥ 500 V AC): Ableiter mit Erdfreiheit 500 V verwenden, Schutzpegel Ader/Ader z. B. ≤ 52 V",
                             "Transmitter floating (≥ 500 V AC): use an arrester rated floating 500 V, protection level core/core e.g. ≤ 52 V"});
        } else {
            r.setText("up", {"Kreis gilt als geerdet: U_p (Ader–PG) bei 10 kA (8/20 µs) unter der Isolationsfestigkeit des Messumformers, z. B. ≤ 35 V",
                             "Loop counts as earthed: U_p (core–PG) at 10 kA (8/20 µs) below the transmitter insulation, e.g. ≤ 35 V"});
        }
        const auto& sh = in.choice("shield");
        const double len = in.num("len");
        if (sh == "pipe") {
            r.setText("line", {"im beidseitig geerdeten Metallrohr – Länge nicht begrenzt", "in a metal conduit earthed at both ends – length not limited"});
        } else if (sh == "shielded" && len <= 1.0) {
            r.setText("line", {"geschirmt, höchstens 1 m – eingehalten", "shielded, at most 1 m – met"});
        } else {
            r.setText("line", {"nicht eingehalten: geschirmt mit höchstens 1 m oder im beidseitig geerdeten Metallrohr verlegen",
                               "not met: shielded with at most 1 m or route in a metal conduit earthed at both ends"});
            r.warn({"Leitung zwischen Ableiter und Feldgerät zu lang oder ungeschirmt – eingekoppelte Überspannungen erreichen das Gerät.",
                    "Line between arrester and field device too long or unshielded – coupled surges reach the device."});
        }
        r.setText("earth", {"Ableiter im metallisch geschirmten Gehäuse, Erdung ≥ 4 mm² Cu; Erdungsanlage vermascht 10 × 10 m; Schirm "
                            "beidseitig nur mit parallelem isoliertem PA-Leiter ≥ 4 mm² (besser 16 mm²) an derselben PA-Schiene",
                            "Arrester in a metal-shielded enclosure, earthing ≥ 4 mm² Cu; meshed earth termination 10 × 10 m; screen at "
                            "both ends only with a parallel insulated bonding conductor ≥ 4 mm² (better 16 mm²) to the same bar"});
        r.setText("dehn", {"BLITZDUCTOR XT ML4 BD EX 24 (ia, U_i 30 V, I_i 500 mA, 20 kA, U_p ≤ 52 V, C_i/L_i vernachlässigbar) mit "
                           "Basisteil BXT BAS EX; am Feldgerät DEHNpipe DPI MD EX 24 M2",
                           "BLITZDUCTOR XT ML4 BD EX 24 (ia, U_i 30 V, I_i 500 mA, 20 kA, U_p ≤ 52 V, C_i/L_i negligible) with base "
                           "part BXT BAS EX; at the field device DEHNpipe DPI MD EX 24 M2"});
        if (!fail) {
            r.ok({"Ableiter passt zum Messkreis (U_i ≥ U_o, I_i ≥ I_o, Zündschutzart).", "Arrester matches the loop (U_i ≥ U_o, I_i ≥ I_o, type of protection)."});
        }
        r.info({"C_i und L_i des Ableiters in den Nachweis der Eigensicherheit aufnehmen; Endgerät nach NAMUR NE 21 (0,5 kV Ader/Ader, "
                "1 kV Ader/Erde).",
                "Include C_i and L_i of the arrester in the intrinsic safety verification; equipment per NAMUR NE 21 (0.5 kV core/core, "
                "1 kV core/earth)."});
        return r;
    };
    c.chart = [](const Inputs& in, const Result&, const Context&) {
        const auto& sh = in.choice("shield");
        const double len = in.num("len");
        const bool ok = sh == "pipe" || (sh == "shielded" && len <= 1.0);
        const auto& z = in.choice("zone");
        return surgepics::exCircuit(len, ok, {"Ex-Zone " + z, "Ex zone " + z});
    };
    return c;
}

// ------------------------------------------------------------------ induced voltage in installation loops

Calculator lempLoop() {
    Calculator c;
    c.id = "lemp_loop";
    c.category = kCat;
    c.title = {"Induzierte Spannung in Installationsschleifen", "Induced voltage in installation loops"};
    c.summary = {"U = M · Δi/Δt einer Leiterschleife neben der Ableitung (LEMP) – Beispiel DEHN WPX050: 720 kV",
                 "U = M · Δi/Δt of a conductor loop next to a down conductor (LEMP) – DEHN WPX050 example: 720 kV"};
    c.description = {
        "Induzierte Rechteckspannung in einer offenen oder geschlossenen Leiterschleife (z. B. Alarm- oder Versorgungsleitung) "
        "neben einer blitzstromführenden Ableitung: U = M · Δi/Δt (DEHN WPX050 Bild 14/15).\n"
        "M = Gegeninduktivität der Schleife (aus dem DEHN-Diagramm für quadratische Schleifen a × a im Abstand s; Beispiel a = 10 m, "
        "s = 3 m → M₂ ≈ 4,8 µH). Δi/Δt = Steilheit des Blitzstroms nach IEC 62305-1 Tabelle 3: erster Stoß 20/15/10 kA/µs, "
        "Folgestoß 200/150/100 kA/µs für Blitzschutzklasse I/II/III–IV.\n"
        "Beispiel WPX050: 4,8 µH · 150 kA/µs = 720 kV. Abhilfe: Schleifen klein halten (Hin- und Rückleiter gemeinsam verlegen), "
        "Abstand zu Ableitungen, Schirmung (beidseitig aufgelegt) und SPD an den Schleifenenden.",
        "Induced square-wave voltage in an open or closed conductor loop (e.g. alarm or supply line) next to a down conductor "
        "carrying lightning current: U = M · Δi/Δt (DEHN WPX050 Bild 14/15).\n"
        "M = mutual inductance of the loop (from the DEHN chart for square loops a × a at distance s; example a = 10 m, s = 3 m → "
        "M₂ ≈ 4.8 µH). Δi/Δt = steepness of the lightning current per IEC 62305-1 table 3: first stroke 20/15/10 kA/µs, "
        "subsequent stroke 200/150/100 kA/µs for LPL I/II/III–IV.\n"
        "WPX050 example: 4.8 µH · 150 kA/µs = 720 kV. Remedy: keep loops small (route go and return together), distance to down "
        "conductors, shielding (bonded at both ends) and SPD at the loop ends."};
    c.inputs = {
        Num::number("M", {"Gegeninduktivität M der Schleife", "Mutual inductance M of the loop"}, "µH", 4.8).positive()
            .withHint({"aus DEHN-Diagramm (WPX050 Bild 15), z. B. a = 10 m, s = 3 m → 4,8 µH", "from the DEHN chart (WPX050 Bild 15), e.g. a = 10 m, s = 3 m → 4.8 µH"}),
        Num::choice("lpl", {"Blitzschutzklasse", "Lightning protection level"},
                    {{"1", "I"}, {"2", "II"}, {"3", "III"}, {"4", "IV"}}, "2"),
        Num::choice("stroke", {"Blitzstrom-Anteil", "Lightning current component"},
                    {{"first", {"erster Stoß (10/350 µs)", "first stroke (10/350 µs)"}},
                     {"subsequent", {"Folgestoß (0,25/100 µs, steilster)", "subsequent stroke (0.25/100 µs, steepest)"}}},
                    "subsequent"),
        Num::number("custom", {"Eigene Steilheit Δi/Δt", "Own steepness Δi/Δt"}, "kA/µs", std::numeric_limits<double>::quiet_NaN()).positive().opt(),
    };
    c.outputs = {Out::number("U", {"Induzierte Spannung U", "Induced voltage U"}, "kV", 0).primary(),
                 Out::number("didt", {"Steilheit Δi/Δt", "Steepness Δi/Δt"}, "kA/µs", 0)};
    c.keywords = {"lemp", "induktion", "induzierte spannung", "schleife", "gegeninduktivität", "induced voltage", "loop", "blitz"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        double didt = 0;
        if (const auto own = in.maybe("custom")) {
            didt = *own;
        } else {
            const auto& lpl = in.choice("lpl");
            const double first = lpl == "1" ? 20.0 : lpl == "2" ? 15.0 : 10.0;  // IEC 62305-1 table 3: I / T1 = 200 kA / 10 µs …
            didt = in.choice("stroke") == "first" ? first : 10.0 * first;    // subsequent: 200 / 150 / 100 kA/µs
        }
        r.set("didt", didt);
        const double u = in.num("M") * didt;  // µH · kA/µs = kV
        r.set("U", u);
        r.warn({"Die Spannung übersteigt jede Stoßfestigkeit (U_W ≤ 6 kV) – Schleife verkleinern, schirmen oder an den Enden mit SPD beschalten.",
                "The voltage exceeds any withstand (U_W ≤ 6 kV) – reduce the loop, shield it or fit SPDs at its ends."});
        return r;
    };
    c.chart = [](const Inputs& in, const Result& r, const Context&) {
        return surgepics::loop(in.num("M"), r.values.contains("didt") ? r.values.at("didt") : 0.0, r.values.contains("U") ? r.values.at("U") : 0.0);
    };
    return c;
}

// ------------------------------------------------------------------ tables

Table make(std::string id, Text title, Text summary, Text description, std::vector<Text> columns, std::vector<std::vector<Text>> rows) {
    Table t;
    t.id = std::move(id);
    t.category = kCat;
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

Table productsPower() {
    return make("spd_products_power", {"SPD Energietechnik: Kenndaten (DEHN)", "SPD for power: ratings (DEHN)"},
                {"Typ 1+2, 2, 3 und PV – U_C, I_imp, I_n, U_p, Vorsicherung", "Type 1+2, 2, 3 and PV – U_C, I_imp, I_n, U_p, back-up fuse"},
                {"Datenblattwerte aus DEHN WPX050 (Schutzvorschlag Rechenzentren, 04/2022). Typ nach EN 61643-11 (PV: EN 61643-31). "
                 "„Koordiniert ≤ 10 m“ = energetisch koordinierte Schutzwirkung zum Endgerät bis 10 m Leitung. Beispiele – gegen aktuellen "
                 "Katalog prüfen.",
                 "Data sheet values from DEHN WPX050 (data centre protection proposal, 04/2022). Type per EN 61643-11 (PV: EN 61643-31). "
                 "\"Coordinated ≤ 10 m\" = energy-coordinated protection of the terminal equipment up to 10 m of cable. Examples – check "
                 "against the current catalogue."},
                {{"Gerät (Art.-Nr.)", "Device (part no.)"}, {"Typ", "Type"}, {"Netz", "System"}, "U_C", "I_imp (10/350 µs)",
                 "I_n (8/20 µs)", "U_p", {"Vorsicherung / Kurzschluss", "Back-up fuse / short circuit"}, {"Bauform", "Design"}},
                {{"DEHNvenCI DVCI 1 255 FM (961 205)", {"Typ 1 + Typ 2 (+ Typ 3 koordiniert ≤ 10 m)", "type 1 + type 2 (+ type 3 coordinated ≤ 10 m)"},
                  {"230 V, 1-polig", "230 V, 1-pole"}, "255 V", "25 kA", "25 kA", "≤ 1,5 kV",
                  {"nicht nötig (integriert, 100 kA); I_fi 50 kA", "not needed (integrated, 100 kA); I_fi 50 kA"}, {"2 TE, IP 20", "2 mod., IP 20"}},
                 {"DEHNventil DV M2 TNS 255 FM (954 405)", {"Typ 1 + 2 + 3", "type 1 + 2 + 3"}, "TN-S 230/400 V", "255 V",
                  {"25 kA je Pol, 100 kA gesamt", "25 kA per pole, 100 kA total"}, "25 / 100 kA", "≤ 1,5 kV",
                  {"max. 250 A gG; I_SCCR 50 kA (100 kA geprüft)", "max. 250 A gG; I_SCCR 50 kA (100 kA tested)"}, {"4 TE, IP 20", "4 mod., IP 20"}},
                 {"DEHNguard DG M TNS ACI 275 FM (952 440)", {"Typ 2 (+ Typ 3 koordiniert ≤ 10 m)", "type 2 (+ type 3 coordinated ≤ 10 m)"},
                  "TN-S 230/400 V", {"275 V L–PE, 255 V N–PE", "275 V L–PE, 255 V N–PE"}, "–", "20 kA", "≤ 1,5 kV",
                  {"keine externe Sicherung (ACI); I_SCCR 25 kA", "no external fuse (ACI); I_SCCR 25 kA"}, {"4 TE, IP 20", "4 mod., IP 20"}},
                 {"DEHNrail DR M 2P 255 FM (953 205)", {"Typ 3", "type 3"}, {"230 V, I_L 25 A", "230 V, I_L 25 A"}, "255 V AC/DC", "–",
                  {"3 kA (gesamt 5 kA); U_OC 6 kV", "3 kA (total 5 kA); U_OC 6 kV"}, {"≤ 1,25 kV L–N, ≤ 1,5 kV L/N–PE", "≤ 1.25 kV L–N, ≤ 1.5 kV L/N–PE"},
                  {"25 A gG oder B 25; I_SCCR 6 kA", "25 A gG or B 25; I_SCCR 6 kA"}, {"1 TE, IP 20", "1 mod., IP 20"}},
                 {"DEHNcube DCU YPV SCI 1000 2M (900 920)", {"Typ 2 PV (EN 61643-31)", "type 2 PV (EN 61643-31)"},
                  {"PV-Gleichspannung, 2 MPP", "PV DC, 2 MPP"}, "U_CPV 1000 V", "–", {"12,5 kA (I_max 25 kA, gesamt 40 kA)", "12.5 kA (I_max 25 kA, total 40 kA)"},
                  {"≤ 4 kV (≤ 3,5 kV bei 5 kA)", "≤ 4 kV (≤ 3.5 kV at 5 kA)"}, "I_SCPV 1000 A", "IP 65, 130 × 94 × 81 mm"}});
}

Table productsSignal() {
    return make("spd_products_signal", {"SPD MSR und Daten: Kenndaten (DEHN)", "SPD for I&C and data: ratings (DEHN)"},
                {"BLITZDUCTORconnect, DEHNpatch, Ex-i-Ableiter – U_C, I_imp, I_n, U_p, f_G", "BLITZDUCTORconnect, DEHNpatch, Ex i arrester – U_C, I_imp, I_n, U_p, f_G"},
                {"Datenblattwerte aus DEHN WPX050 (04/2022) und WPX035 Tabelle 2 (01/2022); Prüfkategorien nach IEC 61643-21 (D1 = "
                 "10/350 µs, C2 = 8/20 µs). Unsymmetrisch (BE) = Einzeladern mit gemeinsamem Bezug, z. B. 4–20 mA gegen 0 V; symmetrisch "
                 "(BD) = erdpotentialfreie Doppelader. BLITZDUCTORconnect ist für Einbau in Ex-Zone 2 zugelassen (II 3G Ex ec IIC T4 Gc). "
                 "Beispiele – gegen aktuellen Katalog prüfen.",
                 "Data sheet values from DEHN WPX050 (04/2022) and WPX035 table 2 (01/2022); test categories per IEC 61643-21 (D1 = "
                 "10/350 µs, C2 = 8/20 µs). Unbalanced (BE) = single cores with a common reference, e.g. 4–20 mA against 0 V; balanced "
                 "(BD) = floating pair. BLITZDUCTORconnect is approved for installation in Ex zone 2 (II 3G Ex ec IIC T4 Gc). Examples – "
                 "check against the current catalogue."},
                {{"Gerät (Art.-Nr.)", "Device (part no.)"}, {"Schaltung", "Circuit"}, "U_N / U_C", "I_L", {"I_imp D1 je Ader", "I_imp D1 per core"},
                 {"I_n C2 je Ader", "I_n C2 per core"}, {"U_p Ader–Ader / Ader–PG", "U_p core–core / core–PG"}, "f_G", {"Einsatz", "Use"}},
                {{"BLITZDUCTORconnect BCO ML2 BE 24 (927 224)", {"2 Einzeladern, gemeinsamer Bezug (unsymmetrisch)", "2 single cores, common reference (unbalanced)"},
                  "24 V / 33 V DC", "0,75 A", {"1,5 kA (3 kA gesamt)", "1.5 kA (3 kA total)"}, {"5 kA (10 kA gesamt)", "5 kA (10 kA total)"},
                  {"≤ 90 V / ≤ 75 V (C3: ≤ 45 V Ader–PG)", "≤ 90 V / ≤ 75 V (C3: ≤ 45 V core–PG)"}, "3,4 MHz",
                  {"4–20 mA, 0–10 V, Binär 24 V; Dachaufbauten, Brandmeldeanlage", "4–20 mA, 0–10 V, binary 24 V; roof units, fire alarm"}},
                 {"BLITZDUCTORconnect BCO ML2 BD 24 (927 244)", {"1 Doppelader, erdpotentialfrei (symmetrisch)", "1 pair, floating (balanced)"},
                  "24 V / 36 V DC", "0,75 A", "1,5 kA", "5 kA", "≤ 57 V / ≤ 600 V", "5,8 MHz",
                  {"erdfreie Signale und Meldungen; NSHV, Notstrom", "floating signals and alarms; LV main, standby power"}},
                 {"BLITZDUCTORconnect BCO ML2 BD HF 5 (927 271)", {"1 Doppelader, hochfrequent (symmetrisch)", "1 pair, high frequency (balanced)"},
                  "5 V / 8,5 V DC", "0,75 A", "1,5 kA", "5 kA", {"≤ 42 V / ≤ 600 V (C3: ≤ 15 V Ader–Ader)", "≤ 42 V / ≤ 600 V (C3: ≤ 15 V core–core)"},
                  "100 MHz", {"Bussysteme (z. B. RS-485); Serverraum, Sicherheitstechnik", "bus systems (e.g. RS-485); server room, security"}},
                 {"DEHNpatch DPA CLE IP66 (929 221)", {"RJ45, alle 4 Paare, Klasse E", "RJ45, all 4 pairs, class E"},
                  {"5 V / 8,5 V DC; PoE 60 V Paar–Paar", "5 V / 8.5 V DC; PoE 60 V pair–pair"}, "1 A", {"0,8 kA (4 kA gesamt)", "0.8 kA (4 kA total)"},
                  {"2,5 kA Ader–PG (10 kA gesamt)", "2.5 kA core–PG (10 kA total)"}, {"≤ 170 V / ≤ 600 V; PoE ≤ 120 V", "≤ 170 V / ≤ 600 V; PoE ≤ 120 V"},
                  "250 MHz", {"Gigabit-Ethernet und PoE++ im Freien (IP 66), IP-Kameras", "Gigabit Ethernet and PoE++ outdoors (IP 66), IP cameras"}},
                 {"BLITZDUCTOR XT BXT ML4 BD EX 24", {"eigensicher ia, erdfrei 500 V", "intrinsically safe ia, floating 500 V"},
                  "U_i 30 V", "I_i 500 mA", "–", {"20 kA gesamt (8/20 µs)", "20 kA total (8/20 µs)"}, {"≤ 52 V Ader–Ader", "≤ 52 V core–core"}, "7,7 MHz",
                  {"4–20 mA/HART in Ex-Zone 1 (ATEX, IECEx); C_i/L_i vernachlässigbar", "4–20 mA/HART in Ex zone 1 (ATEX, IECEx); C_i/L_i negligible"}}});
}

Table datacenterProposal() {
    auto t = make("spd_datacenter", {"Rechenzentrum: Schutzvorschlag nach Anwendung", "Data centre: protection proposal by application"},
                  {"Blitzschutzzonen und Ableiter für NSHV, Notstrom, Server, BMA, Kameras, PV, Dach (DEHN)",
                   "Protection zones and arresters for LV main, standby power, servers, fire alarm, cameras, PV, roof (DEHN)"},
                  {"Nach DEHN WPX050 Bild 11 (Rechenzentrum, Blitzschutzklasse I). NSHV: Kombi-Ableiter auf Funkenstreckenbasis mit "
                   "100 kA (10/350 µs) gesamt bei LPL I; Unterverteilungen wegen Leitungslängen > 10 m mit Typ 2 (DEHNguard ACI ohne "
                   "Vorsicherung). Serverraum: nicht geschirmt Typ 2 im Bodentank ausreichend, größere Räume zusätzlich Typ 3 im Abgang "
                   "der Stromschiene oder in der Rack-Stromversorgung; Raum = LPZ 2, Servergehäuse kann LPZ 3 sein (nachweisbare Dämpfung, "
                   "Anbindung ≥ 6 mm² Cu). Kenndaten siehe Tabellen „SPD Energietechnik“ und „SPD MSR und Daten“. Beispiele – gegen "
                   "aktuellen Katalog prüfen.",
                   "After DEHN WPX050 Bild 11 (data centre, LPL I). LV main: spark-gap based combined arrester with 100 kA (10/350 µs) "
                   "total for LPL I; sub-distributions with type 2 because of cable lengths > 10 m (DEHNguard ACI without back-up fuse). "
                   "Server room: unscreened type 2 in the floor box suffices, larger rooms additionally type 3 in the busbar tap-offs or "
                   "the rack power supply; room = LPZ 2, server enclosure may be LPZ 3 (proven attenuation, bonding ≥ 6 mm² Cu). Ratings "
                   "see tables \"SPD for power\" and \"SPD for I&C and data\". Examples – check against the current catalogue."},
                  {{"Nr.", "No."}, {"Schutz für", "Protection of"}, {"Zone", "Zone"}, {"Energietechnik", "Power"}, {"Informationstechnik", "IT / I&C"},
                   {"Art.-Nr.", "Part no."}},
                  {{"1", {"Niederspannungsschaltanlage (NSHV)", "LV switchgear (main)"}, "LPZ 0A → 1",
                    {"Kombi-Ableiter Typ 1+2(+3): DEHNvenCI DVCI 1 255 FM, alternativ DEHNventil DV M2 TNS 255 FM",
                     "combined arrester type 1+2(+3): DEHNvenCI DVCI 1 255 FM, alternatively DEHNventil DV M2 TNS 255 FM"},
                    {"BLITZDUCTORconnect BCO ML2 BD 24; Netzanalyse DEHNrecord DRC SD 2 1", "BLITZDUCTORconnect BCO ML2 BD 24; power quality DEHNrecord DRC SD 2 1"},
                    "961 205 / 954 405; 927 244; 910 921"},
                   {"2", {"Notstromversorgung", "Standby power supply"}, "LPZ 1", {"Typ 2: DEHNguard DG M TNS ACI 275 FM", "type 2: DEHNguard DG M TNS ACI 275 FM"},
                    "BLITZDUCTORconnect BCO ML2 BD 24", "952 440; 927 244"},
                   {"3", {"Serverraum", "Server room"}, {"LPZ 1 → 2 (Server LPZ 3)", "LPZ 1 → 2 (server LPZ 3)"},
                    {"Typ 2 im Bodentank: DEHNguard ACI; große Räume zusätzlich Typ 3", "type 2 in the floor box: DEHNguard ACI; large rooms additionally type 3"},
                    "BLITZDUCTORconnect BCO ML2 BD HF 5", "952 440; 927 271"},
                   {"4", {"Brandmeldeanlage", "Fire alarm system"}, "LPZ 1 → 2", {"Typ 3: DEHNrail DR M 2P 255", "type 3: DEHNrail DR M 2P 255"},
                    "BLITZDUCTORconnect BCO ML2 BE 24", "953 205; 927 224"},
                   {"5", {"Sicherheitszentrale, Büros, IP-Kameras", "Security centre, offices, IP cameras"}, "LPZ 0B → 1, 1 → 2",
                    {"Typ 3: DEHNrail DR M 2P 255", "type 3: DEHNrail DR M 2P 255"},
                    {"DEHNpatch DPA CL8 EA 4PPOE, DEHNpatch Outdoor DPA CLE IP66, BCO ML2 BD HF 5", "DEHNpatch DPA CL8 EA 4PPOE, DEHNpatch Outdoor DPA CLE IP66, BCO ML2 BD HF 5"},
                    "953 205; 929 161; 929 221; 927 271"},
                   {"6", {"PV-Anlage", "PV system"}, "LPZ 0B → 1", {"Typ 2 PV: DEHNcube DCU YPV SCI 1000 2M", "type 2 PV: DEHNcube DCU YPV SCI 1000 2M"}, "–",
                    "900 920"},
                   {"7", {"Dachaufbauten (Heizung, Klima, Lüftung)", "Roof units (heating, air conditioning, ventilation)"}, "LPZ 0B → 1",
                    {"Typ 2: DEHNguard ACI (I_n 20 kA je Pol bei LPL I)", "type 2: DEHNguard ACI (I_n 20 kA per pole for LPL I)"},
                    "BLITZDUCTORconnect BCO ML2 BE 24", "952 440; 927 224"}});
    t.drawing = surgepics::datacenter();
    return t;
}

Table datacenterLightning() {
    return make("datacenter_lightning", {"Rechenzentrum: Blitzschutz, Erdung, Schirmung", "Data centre: lightning protection, earthing, shielding"},
                {"Blitzschutzklasse, Maschenweiten, Ringerder, Zonen, Serverraum (DEHN WPX050)", "LPL, mesh sizes, ring earth, zones, server room (DEHN WPX050)"},
                {"Planungsgrundsätze aus DEHN WPX050 „Blitz- und Überspannungsschutzkonzept für Rechenzentren“ (04/2022) mit EN 62305-2/-3/-4, "
                 "EN 50600-1/-2-2, IEC 60364-5-53, DIN VDE 0100-444. Gegen gültige Normausgabe prüfen.",
                 "Design principles from DEHN WPX050 \"Lightning and surge protection concept for data centres\" (04/2022) with "
                 "EN 62305-2/-3/-4, EN 50600-1/-2-2, IEC 60364-5-53, IEC 60364-4-44. Check against the edition in force."},
                {{"Thema", "Topic"}, {"Anforderung", "Requirement"}, {"Grundlage", "Basis"}},
                {{{"Blitzschutzklasse", "Protection level"},
                  {"in der Regel LPL I (Risikoanalyse mit dem Betreiber abstimmen); Stoßströme bis 200 kA (10/350 µs)",
                   "usually LPL I (agree the risk assessment with the operator); impulse currents up to 200 kA (10/350 µs)"},
                  "EN 62305-2, EN 50600"},
                 {{"Äquipotentialflächen", "Equipotential planes"},
                  {"Bewehrung mit Maschen 5 × 5 m, alle 1 m mit der Bewehrung verbinden; verkürzt den Trennungsabstand und senkt die Impedanz",
                   "reinforcement with 5 × 5 m meshes, bonded to the rebar every 1 m; shortens the separation distance and lowers the impedance"},
                  "EN 62305-4"},
                 {{"Erdung", "Earthing"},
                  {"ein gemeinsames Erdungssystem (Blitzschutz, MS, NS, TK, Schirmung, Potentialausgleich); Ringerder mit Masche 5 × 5 m "
                   "unter oder außerhalb des Fundamenterders, alle 5 m damit verbunden",
                   "one common earthing system (lightning, MV, LV, telecom, shielding, bonding); ring earth with 5 × 5 m mesh below or "
                   "outside the foundation earth, connected to it every 5 m"},
                  "EN 62305-4, EN 50600-2-2"},
                 {{"Raumschirmung", "Spatial shielding"},
                  {"reine Gitterstruktur 5 × 5 m dämpft das Blitzfeld etwa um Faktor 2 (6 dB); Durchbrüche minimieren, Fenster und Türen "
                   "einbeziehen; Bewertung gegen Prüfschärfegrad 3 (EN 61000-4-9/-10)",
                   "a plain 5 × 5 m grid attenuates the lightning field by about a factor of 2 (6 dB); minimise openings, include windows "
                   "and doors; assess against test level 3 (EN 61000-4-9/-10)"},
                  "EN 62305-4"},
                 {{"Potentialausgleichsnetzwerk", "Bonding network"},
                  {"dreidimensional vermascht: Rohre, Bewehrung, Gitterroste, Kabel- und Lüftungskanäle, Aufzugsschienen, Fenster/Türen; "
                   "Racks kurz anbinden (möglichst kein Runddraht)",
                   "meshed in three dimensions: pipes, rebar, gratings, cable and air ducts, lift rails, windows/doors; bond racks with "
                   "short connections (preferably no round wire)"},
                  "EN 62305-4, DIN VDE 0100-444"},
                 {{"Dachaufbauten", "Roof units"},
                  {"getrennte Fangeinrichtung (HVI-Leitung) hält den Trennungsabstand; sonst Leitungen über SPD einbinden: Typ 2 mit I_n "
                   "20 kA je Pol (LPL I), 15 kA (LPL II), 10 kA (LPL III/IV), Informationstechnik 10 kA je Ader",
                   "isolated air-termination (HVI conductor) keeps the separation distance; otherwise bond the lines via SPD: type 2 with "
                   "I_n 20 kA per pole (LPL I), 15 kA (LPL II), 10 kA (LPL III/IV), data lines 10 kA per core"},
                  {"WPX050 Tabelle 1", "WPX050 table 1"}},
                 {{"Blitzschutzzonen", "Protection zones"},
                  {"NSHV-Einspeisung LPZ 0A → 1 (Typ 1/Kombi-Ableiter); Unterverteilungen Typ 2 (Leitungen > 10 m: Induktion); "
                   "Serverraum LPZ 2, Servergehäuse LPZ 3 möglich",
                   "LV main supply LPZ 0A → 1 (type 1/combined arrester); sub-distributions type 2 (cables > 10 m: induction); server "
                   "room LPZ 2, server enclosure LPZ 3 possible"},
                  "EN 62305-4"},
                 {{"Anschluss SPD", "SPD connection"},
                  {"Anschlusslänge ≤ 0,5 m; Einspeisungen über Kuppelschalter (mehrere Trafos) gleich beschalten; Typ 2 auch in den "
                   "Abgängen großer NSHV",
                   "lead length ≤ 0.5 m; protect every infeed of coupled transformers alike; type 2 also in the outgoing feeders of large "
                   "LV main boards"},
                  "IEC 60364-5-53"},
                 {{"Serverraum", "Server room"},
                  {"nicht geschirmt: Typ 2 im Bodentank ausreichend; größere Räume zusätzlich Typ 3 im Abgang der Stromschiene oder in "
                   "der Rack-Stromversorgung; Servergehäuse als LPZ 3 nur mit nachgewiesener Dämpfung, Anbindung ≥ 6 mm² Cu",
                   "unscreened: type 2 in the floor box suffices; larger rooms additionally type 3 in the busbar tap-offs or the rack power "
                   "supply; server enclosure as LPZ 3 only with proven attenuation, bonding ≥ 6 mm² Cu"},
                  "WPX050"}});
}

Table exRequirements() {
    auto t = make("ex_lightning_protection", {"Ex-Bereich: Blitz- und Überspannungsschutz", "Hazardous areas: lightning and surge protection"},
                  {"Anforderungen an Fangeinrichtung, Ableitungen, SPD, Erdung und Schirm (DEHN WPX035)",
                   "Requirements for air-termination, down conductors, SPD, earthing and screen (DEHN WPX035)"},
                  {"Aus DEHN WPX035 „Blitz- und Überspannungsschutz für explosionsgefährdete Bereiche“ (01/2022) mit DIN EN 62305-3/-4, "
                   "DIN EN 60079-11/-14/-25, NAMUR NE 21, DIN 18014, DIN VDE 0151. Auswahl der Ableiter mit dem Rechner „Überspannungsschutz "
                   "im Ex-Bereich“. Gegen gültige Normausgabe prüfen.",
                   "From DEHN WPX035 \"Lightning and surge protection for hazardous areas\" (01/2022) with IEC 62305-3/-4, IEC 60079-11/-14/-25, "
                   "NAMUR NE 21, DIN 18014, DIN VDE 0151. Select arresters with the calculator \"Surge protection in hazardous areas\". "
                   "Check against the edition in force."},
                  {{"Thema", "Topic"}, {"Anforderung", "Requirement"}, {"Grundlage", "Basis"}},
                  {{{"Blitzschutzklasse", "Protection level"},
                    {"mindestens II (Blitzkugel 30 m, Masche 10 × 10 m); Abweichung nur begründet oder nach Risikoanalyse",
                     "at least II (rolling sphere 30 m, mesh 10 × 10 m); deviation only if justified or per risk assessment"},
                    "DIN EN 62305-3"},
                   {{"Fangeinrichtung", "Air-termination"},
                    {"außerhalb der Ex-Zonen errichten (Funken am Einschlagpunkt); natürliche Teile (Dach, Rohre, Behälter) nur ab 5 mm Stahl "
                     "und ohne gefährliche Erwärmung",
                     "install outside the Ex zones (sparks at the point of strike); natural parts (roof, pipes, vessels) only from 5 mm steel "
                     "and without dangerous heating"},
                    {"DIN EN 62305-3 Anhang D", "IEC 62305-3 annex D"}},
                   {{"Ableitungen", "Down conductors"},
                    {"je 10 m Umfang der Dachaußenkante eine, mindestens 2; Potentialausgleich am Boden und alle 20 m Höhe; durchverbundene "
                     "Bewehrung zulässig",
                     "one per 10 m of roof perimeter, at least 2; bonding at ground level and every 20 m of height; continuous rebar permitted"},
                    "DIN EN 62305-3"},
                   {{"Trennungsabstand", "Separation distance"},
                    {"an der Näherungsstelle berechnen (Aufteilung des Blitzstroms auf die Ableitungen)",
                     "calculate at the point of proximity (lightning current divided over the down conductors)"},
                    "DIN EN 62305-3"},
                   {{"Gebäudeschirmung", "Building shielding"},
                    {"Metallfassade und Bewehrung zum Schirmkäfig verbinden: Magnetfeld 10- bis 300-mal kleiner",
                     "connect metal façade and rebar into a screening cage: magnetic field 10 to 300 times smaller"},
                    "DIN EN 62305-4"},
                   {{"SPD-Einbauort", "SPD location"},
                    {"Übergang LPZ 0B → 1, bevor Überspannungen Ex-Zone 0/20 erreichen; am anderen Ende der Feldleitung gleiches Ableitvermögen",
                     "transition LPZ 0B → 1, before surges reach Ex zone 0/20; same discharge capacity at the other end of the field cable"},
                    "DIN EN 62305-4"},
                   {{"SPD-Ableitvermögen", "SPD discharge capacity"},
                    {"mindestens 10 Impulse à 10 kA (8/20 µs) ohne Defekt", "at least 10 impulses of 10 kA (8/20 µs) without defect"},
                    "DIN EN 60079-11/-14/-25"},
                   {{"SPD-Gehäuse", "SPD enclosure"},
                    {"metallisch geschirmtes Gehäuse, Erdung ≥ 4 mm² Cu", "metal-shielded enclosure, earthing ≥ 4 mm² Cu"}, "DIN EN 60079-14"},
                   {{"Leitung SPD → Gerät", "Line SPD → device"},
                    {"im beidseitig geerdeten Metallrohr oder geschirmt mit höchstens 1 m", "in a metal conduit earthed at both ends or shielded, at most 1 m"},
                    "DIN EN 60079-14"},
                   {{"Erdfreiheit", "Floating circuits"},
                    {"Messumformer ≥ 500 V AC gegen Erde = erdfrei, darf durch das SPD nicht aufgehoben werden; < 500 V AC = geerdet: U_p "
                     "(Ader–PG) bei 10 kA unter der Isolationsfestigkeit, z. B. ≤ 35 V",
                     "transmitter ≥ 500 V AC to earth = floating, the SPD must not cancel this; < 500 V AC = earthed: U_p (core–PG) at 10 kA "
                     "below the insulation, e.g. ≤ 35 V"},
                    "DIN EN 60079-11"},
                   {{"Zündschutzart", "Type of protection"},
                    {"SPD mit ia-Bescheinigung auch für ib/ic; U_i ≥ U_o und I_i ≥ I_o des Speisegeräts; C_i/L_i in den Nachweis aufnehmen",
                     "SPD certified ia also suits ib/ic; U_i ≥ U_o and I_i ≥ I_o of the supply; include C_i/L_i in the verification"},
                    "DIN EN 60079-11/-25"},
                   {{"Störfestigkeit", "Immunity"},
                    {"NAMUR NE 21: 0,5 kV Ader/Ader, 1 kV Ader/Erde (IEC 61000-4-5); Prüfschärfegrad 1–4 auf den Schutzgeräten (z. B. P1)",
                     "NAMUR NE 21: 0.5 kV core/core, 1 kV core/earth (IEC 61000-4-5); test level 1–4 on the protective devices (e.g. P1)"},
                    "NE 21, IEC 61000-4-5"},
                   {{"Erdungsanlage", "Earth termination"},
                    {"je Gebäude/Anlagenteil eigene Erdung, alle vermascht; Masche 20 × 20 m, im Ex-Bereich und bei Elektronik 10 × 10 m",
                     "own earthing per building/plant section, all meshed; mesh 20 × 20 m, 10 × 10 m in Ex areas and with electronics"},
                    "DIN 18014, DIN EN 62305, DIN VDE 0151"},
                   {{"Potentialausgleich", "Equipotential bonding"},
                    {"konsequent einschließlich Stützen, Rohre, Behälter; Anschlüsse gegen Selbstlockern sichern; zusätzlicher PA nach "
                     "DIN EN 60079-14 (DIN VDE 0100-410/-540); Erdleiter am SPD ≥ 4 mm² Cu",
                     "consistently including columns, pipes, vessels; secure connections against loosening; supplementary bonding per "
                     "IEC 60079-14 (IEC 60364-4-41/-5-54); earth conductor at the SPD ≥ 4 mm² Cu"},
                    "DIN EN 60079-14"},
                   {{"Kabelschirm", "Cable screen"},
                    {"beidseitig erden, wenn die Erdung vermascht ist (10 × 10 m) und ein isolierter PA-Leiter ≥ 4 mm² (besser 16 mm²) "
                     "parallel zur Leitung an dieselbe PA-Schiene geführt wird",
                     "earth at both ends if the earth termination is meshed (10 × 10 m) and an insulated bonding conductor ≥ 4 mm² (better "
                     "16 mm²) runs parallel to the line to the same bonding bar"},
                    "DIN EN 60079-14"},
                   {{"Warte (LPZ 1 → 2)", "Control room (LPZ 1 → 2)"},
                    {"geschirmte Leitung beidseitig aufgelegt: am Übergang LPZ 1 → 2 kein weiteres SPD nötig",
                     "shielded line bonded at both ends: no further SPD needed at the transition LPZ 1 → 2"},
                    "WPX035"}});
    t.drawing = surgepics::exCircuit(1.0, true, {"Ex-Zone 1", "Ex zone 1"});
    return t;
}

Table dischargeCapacity() {
    return make("spd_discharge_capacity", {"Ableitvermögen nach Blitzschutzklasse", "Discharge capacity by protection level"},
                {"I_imp je Pol/Ader (VdS 2833), Typ-2-Empfehlung bei fehlendem Trennungsabstand, Ex", "I_imp per pole/core (VdS 2833), type 2 recommendation without separation distance, Ex"},
                {"Nach DEHN WPX050 Tabelle 1–3 (VdS 2833) und WPX035. LPL I = höchste Blitzschutzklasse (Rechenzentren). Für andere "
                 "Klassen I_imp mit dem Rechner „Überspannungsschutz 230/400 V“ (Stromaufteilung) bestimmen. Gegen gültige Ausgabe prüfen.",
                 "After DEHN WPX050 tables 1–3 (VdS 2833) and WPX035. LPL I = highest protection level (data centres). For other levels "
                 "determine I_imp with the calculator \"Surge protection 230/400 V\" (current split). Check against the edition in force."},
                {{"Anwendung", "Application"}, {"Schaltung / Leitung", "Connection / line"}, {"Ableitvermögen", "Discharge capacity"}, {"Quelle", "Source"}},
                {{{"Energietechnik, LPL I", "Power, LPL I"}, {"4+0: L–PE und N–PE", "4+0: L–PE and N–PE"}, {"je 25 kA (10/350 µs)", "25 kA each (10/350 µs)"},
                  {"VdS 2833 (WPX050 Tab. 2)", "VdS 2833 (WPX050 tab. 2)"}},
                 {{"Energietechnik, LPL I", "Power, LPL I"}, "3+1: L–N / N–PE", "25 kA / 100 kA (10/350 µs)", {"VdS 2833 (WPX050 Tab. 2)", "VdS 2833 (WPX050 tab. 2)"}},
                 {{"Informationstechnik, LPL I", "Data lines, LPL I"}, {"< 3 Doppeladern", "< 3 pairs"}, {"2,5 kA je Ader (10/350 µs)", "2.5 kA per core (10/350 µs)"},
                  {"VdS 2833 (WPX050 Tab. 3)", "VdS 2833 (WPX050 tab. 3)"}},
                 {{"Informationstechnik, LPL I", "Data lines, LPL I"}, {"≥ 3 Doppeladern", "≥ 3 pairs"}, {"1 kA je Ader", "1 kA per core"},
                  {"VdS 2833 (WPX050 Tab. 3)", "VdS 2833 (WPX050 tab. 3)"}},
                 {{"Informationstechnik, LPL I", "Data lines, LPL I"}, {"≥ 10 Doppeladern", "≥ 10 pairs"}, {"0,5 kA je Ader", "0.5 kA per core"},
                  {"VdS 2833 (WPX050 Tab. 3)", "VdS 2833 (WPX050 tab. 3)"}},
                 {{"Typ 2 ohne Trennungsabstand, LPL I", "Type 2 without separation distance, LPL I"}, {"Energie / Informationstechnik", "power / data"},
                  {"I_n 20 kA je Pol / 10 kA je Ader (8/20 µs)", "I_n 20 kA per pole / 10 kA per core (8/20 µs)"}, {"WPX050 Tab. 1", "WPX050 tab. 1"}},
                 {{"Typ 2 ohne Trennungsabstand, LPL II", "Type 2 without separation distance, LPL II"}, {"Energie / Informationstechnik", "power / data"},
                  {"I_n 15 kA je Pol / 10 kA je Ader", "I_n 15 kA per pole / 10 kA per core"}, {"WPX050 Tab. 1", "WPX050 tab. 1"}},
                 {{"Typ 2 ohne Trennungsabstand, LPL III/IV", "Type 2 without separation distance, LPL III/IV"}, {"Energie / Informationstechnik", "power / data"},
                  {"I_n 10 kA je Pol / 10 kA je Ader", "I_n 10 kA per pole / 10 kA per core"}, {"WPX050 Tab. 1", "WPX050 tab. 1"}},
                 {{"Ex-Bereich, eigensicherer Kreis", "Hazardous area, intrinsically safe loop"}, {"jede Ader", "every core"},
                  {"10 Impulse à 10 kA (8/20 µs) ohne Defekt", "10 impulses of 10 kA (8/20 µs) without defect"}, {"DIN EN 60079-14 (WPX035)", "IEC 60079-14 (WPX035)"}}});
}

}  // namespace

void addSurgeApplications(std::vector<Calculator>& out) {
    out.push_back(spdEx());
    out.push_back(lempLoop());
}

void addSurgeApplicationTables(std::vector<Table>& out) {
    out.push_back(productsPower());
    out.push_back(productsSignal());
    out.push_back(datacenterProposal());
    out.push_back(datacenterLightning());
    out.push_back(exRequirements());
    out.push_back(dischargeCapacity());
}

}  // namespace elc::catalog
