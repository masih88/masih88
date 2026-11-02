// Area "Elektrotechnik" (v1.7): reference tables and the groups of the category page.
#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/electrical.hpp"
#include "elc/formulas/units.hpp"

namespace elc::catalog {

using namespace elc::electrical;

namespace {

using Row = std::vector<Text>;

Table make(std::string id, Text title, Text summary, Text description, Row columns, std::vector<Row> rows) {
    Table t;
    t.id = std::move(id);
    t.category = "electrical";
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

Text num(double v, int d) { return formatNumber(v, d); }
Text dash() { return {"–", "–"}; }

/// 2,98 · 10⁴ style
Text sci(double v, int digits = 3) {
    static const char* sup[] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
    const int e = static_cast<int>(std::floor(std::log10(v)));
    const Text m = formatNumber(v / std::pow(10.0, e), digits - 1);
    std::string exp;
    for (const char ch : std::to_string(e)) exp += ch == '-' ? std::string("⁻") : std::string(sup[ch - '0']);
    return {m.de + " · 10" + exp, m.en + " · 10" + exp};
}

// ------------------------------------------------------------------ cables, busbars

Table letThrough() {
    std::vector<Row> rows;
    for (const double a : cables::kAmpacityAreas) {
        const double kCuPvc = a > 300 ? 103 : 115;
        rows.push_back({num(a, 1), sci(kCuPvc * kCuPvc * a * a), sci(143.0 * 143 * a * a), sci(76.0 * 76 * a * a), sci(94.0 * 94 * a * a)});
    }
    return make("let_through_table", {"Zulässige spezifische Durchlassenergie k²S²", "Permissible let-through energy k²S²"},
                {"Grenze I²t ≤ k²S² für Kabel und Leitungen in A²s", "Limit I²t ≤ k²S² for cables in A²s"},
                {"Adiabatische Erwärmung (IEC 60364-4-43, 434.5.2): Die Durchlassenergie I²t der Schutzeinrichtung darf k²S² nicht "
                 "überschreiten (gilt für t ≤ 5 s). k nach Tab. 43A: Cu/PVC 115 (> 300 mm²: 103), Cu/VPE-EPR 143, Al/PVC 76, Al/VPE 94. "
                 "I²t der Schutzeinrichtung aus dem Herstellerdatenblatt. Gegen die gültige Ausgabe prüfen.",
                 "Adiabatic heating (IEC 60364-4-43, 434.5.2): the let-through energy I²t of the protective device must not exceed "
                 "k²S² (valid for t ≤ 5 s). k per Tab. 43A: Cu/PVC 115 (> 300 mm²: 103), Cu/XLPE-EPR 143, Al/PVC 76, Al/XLPE 94. "
                 "I²t of the device from the manufacturer's data sheet. Check against the edition in force."},
                {{"S in mm²", "S in mm²"}, {"Cu PVC", "Cu PVC"}, {"Cu VPE/EPR", "Cu XLPE/EPR"}, {"Al PVC", "Al PVC"}, {"Al VPE", "Al XLPE"}}, rows);
}

Table busbarTable() {
    std::vector<Row> rows;
    for (const auto& b : busbars()) {
        const std::string sz = formatNumber(b.width, 0, Lang::De) + " × " + formatNumber(b.thickness, 0, Lang::De);
        rows.push_back({{sz, sz}, num(b.area, 0), num(b.kgPerM, 3), num(b.i1, 0), b.i2 > 0 ? num(b.i2, 0) : dash()});
    }
    return make("busbar_table", {"Kupfer-Sammelschienen: Dauerstrom", "Copper busbars: continuous current"},
                {"E-Cu-Flachschienen, 1 und 2 Schienen je Leiter", "E-Cu flat bars, 1 and 2 bars per conductor"},
                {"Innenanlagen, 25 °C Lufttemperatur um den Leiter, max. 65 °C Schienentemperatur, Wechselstrom, hochkant "
                 "(Striebel & John, Kap. 10 Sammelschienensysteme, nach DIN 43671). Bei Abweichungen Korrekturfaktoren der DIN 43671.",
                 "Indoor, 25 °C air around the conductor, max. 65 °C bar temperature, AC, on edge (Striebel & John catalogue ch. 10, "
                 "after DIN 43671). For other conditions apply the DIN 43671 correction factors."},
                {{"B × D in mm", "W × T in mm"}, {"A in mm²", "A in mm²"}, {"kg/m", "kg/m"}, {"1 Schiene in A", "1 bar in A"}, {"2 Schienen in A", "2 bars in A"}},
                rows);
}

Table miTable() {
    struct Cfg {
        MiConfig cfg;
        Text name;
        const char* cores;
    };
    const std::vector<Cfg> cfgs{{MiConfig::TwoCore, {"2 Adern, 1~/DC", "2 cores, 1~/DC"}, "2"},
                                {MiConfig::ThreeCore3ph, {"3 Adern, 3~", "3 cores, 3~"}, "3"},
                                {MiConfig::FourCore3ph, {"4 Adern, 3~", "4 cores, 3~"}, "4"},
                                {MiConfig::SingleCore1ph, {"Einleiter, 1~", "Single core, 1~"}, "1"},
                                {MiConfig::SingleCore3ph, {"Einleiter, 3~", "Single core, 3~"}, "1"},
                                {MiConfig::SevenCore, {"7 Adern", "7 cores"}, "7"},
                                {MiConfig::TwelveCore, {"12 Adern", "12 cores"}, "12"},
                                {MiConfig::NineteenCore, {"19 Adern", "19 cores"}, "19"}};
    std::vector<Row> rows;
    for (const auto duty : {MiDuty::Light500V, MiDuty::Heavy750V}) {
        for (const auto& c : cfgs) {
            for (const double a : miAreas(duty, c.cfg)) {
                const std::string ref = std::string(c.cores) + (duty == MiDuty::Light500V ? "L" : "H") + formatNumber(a, 1, Lang::En);
                rows.push_back({duty == MiDuty::Light500V ? Text{"500 V (leicht)", "500 V (light)"} : Text{"750 V (schwer)", "750 V (heavy)"},
                                c.name, num(a, 1), num(*miAmpacity(duty, c.cfg, a), 1), {ref, ref}});
            }
        }
    }
    return make("mi_cable_table", {"Mineralisolierte Kabel: Belastbarkeit", "Mineral-insulated cables: current rating"},
                {"Cu-Mantel 70 °C, Verlegeart C, 30 °C", "Cu sheath 70 °C, method C, 30 °C"},
                {"BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 (Herstellerdatenblatt): PVC-umhüllt oder blank berührbar, "
                 "direkt auf der Wand, alle Adern belastet. Gegen die gültige Ausgabe prüfen.",
                 "BS 7671 Tab. 4G1A = IEC 60364-5-52 Tab. B.52.6 (manufacturer data sheet): PVC covered or bare exposed to touch, "
                 "clipped direct, all cores loaded. Check against the edition in force."},
                {{"Ausführung", "Grade"}, {"Aderzahl", "Cores"}, {"mm²", "mm²"}, {"I_z in A", "I_z in A"}, {"Bezeichnung", "Reference"}}, rows);
}

// ------------------------------------------------------------------ fuses

Table fuseClasses() {
    return make("fuse_classes", {"Sicherungen: Betriebsklassen", "Fuses: utilisation categories"},
                {"Kennzeichnung gG, aM, gR … nach IEC 60269", "Marking gG, aM, gR … per IEC 60269"},
                {"IEC 60269-1/-2/-4/-6 (DIN VDE 0636). 1. Buchstabe: g = Ganzbereich (Überlast und Kurzschluss), a = Teilbereich "
                 "(nur Kurzschluss, Überlastschutz durch andere Geräte). 2. Buchstabe: Schutzobjekt.",
                 "IEC 60269-1/-2/-4/-6. 1st letter: g = full range (overload and short circuit), a = partial range (short circuit only, "
                 "overload protection by other devices). 2nd letter: protected object."},
                {{"Klasse", "Class"}, {"Bereich", "Range"}, {"Schutzobjekt / Einsatz", "Protected object / use"}},
                {{"gG", {"Ganzbereich", "full range"}, {"Kabel und Leitungen, allgemein", "cables, general purpose"}},
                 {"gL", {"Ganzbereich", "full range"}, {"alte deutsche Bezeichnung für Leitungsschutz, heute gG", "old German marking for cable protection, now gG"}},
                 {"aM", {"Teilbereich", "partial range"}, {"Motorstromkreise (Kurzschlussschutz, Überlast durch Motorschutzrelais)", "motor circuits (short circuit, overload by relay)"}},
                 {"gR", {"Ganzbereich", "full range"}, {"Halbleiter (sehr flink)", "semiconductors (very fast)"}},
                 {"gS", {"Ganzbereich", "full range"}, {"Halbleiter und Leitungen", "semiconductors and cables"}},
                 {"aR", {"Teilbereich", "partial range"}, {"Halbleiter (Kurzschlussschutz)", "semiconductors (short circuit)"}},
                 {"gTr", {"Ganzbereich", "full range"}, {"Transformatoren (NS-Seite)", "transformers (LV side)"}},
                 {"gPV", {"Ganzbereich", "full range"}, {"Photovoltaik-Strings (DC, IEC 60269-6)", "photovoltaic strings (DC, IEC 60269-6)"}}});
}

Table fuseSizes() {
    const Text neozed{"Neozed (D0)", "Neozed (D0)"};
    const Text diazed{"Diazed (D)", "Diazed (D)"};
    const Text nh{"NH (Messer)", "NH (blade)"};
    return make("fuse_d_nh", {"D- und NH-Sicherungen: Größen", "D and NH fuses: sizes"},
                {"Neozed, Diazed und NH-Größen mit Bemessungsströmen", "Neozed, Diazed and NH sizes with rated currents"},
                {"IEC 60269-3 (D-System, für Laien bedienbar) und IEC 60269-2 (NH, nur Fachkräfte). Größte Bemessungsströme je Größe; "
                 "Passschrauben/-hülsen verhindern beim D-System zu große Einsätze. Herstellerangaben prüfen.",
                 "IEC 60269-3 (D system, usable by ordinary persons) and IEC 60269-2 (NH, skilled persons only). Highest rated currents "
                 "per size; gauge screws/rings prevent oversized D cartridges. Check manufacturer data."},
                {{"Größe", "Size"}, {"System", "System"}, {"Gewinde / Bauform", "Thread / design"}, {"Bemessungsstrom", "Rated current"}, {"Spannung", "Voltage"}},
                {{"D01", neozed, "E14", "2–16 A", "400 V AC"},
                 {"D02", neozed, "E18", "20–63 A", "400 V AC"},
                 {"D03", neozed, "M30 × 2", "80–100 A", "400 V AC"},
                 {"DII", diazed, "E27", "2–25 A", "500 V AC"},
                 {"DIII", diazed, "E33", "35–63 A", "500 V AC"},
                 {"DIV", diazed, "R 1¼\"", "80–100 A", "500 V AC"},
                 {"NH000", nh, {"Größe 000", "size 000"}, {"bis 100 A", "up to 100 A"}, "500/690 V AC"},
                 {"NH00", nh, {"Größe 00", "size 00"}, {"bis 160 A", "up to 160 A"}, "500/690 V AC"},
                 {"NH0", nh, {"Größe 0 (nicht für Neuanlagen)", "size 0 (not for new installations)"}, {"bis 160 A", "up to 160 A"}, "500/690 V AC"},
                 {"NH1", nh, {"Größe 1", "size 1"}, {"bis 250 A", "up to 250 A"}, "500/690 V AC"},
                 {"NH2", nh, {"Größe 2", "size 2"}, {"bis 400 A", "up to 400 A"}, "500/690 V AC"},
                 {"NH3", nh, {"Größe 3", "size 3"}, {"bis 630 A", "up to 630 A"}, "500/690 V AC"},
                 {"NH4a", nh, {"Größe 4a", "size 4a"}, {"bis 1250 A", "up to 1250 A"}, "500/690 V AC"}});
}

Table fuseCylindrical() {
    return make("fuse_cylindrical", {"Zylindersicherungen", "Cylindrical fuses"},
                {"Industrie-Zylindersicherungen nach Abmessung", "Industrial cylindrical fuses by size"},
                {"IEC 60269-2 (zylindrische Sicherungseinsätze, z. B. für Sicherungs-Trennschalter). Höchste übliche Bemessungsströme; "
                 "je nach Hersteller und Spannung kleiner – Datenblatt prüfen.",
                 "IEC 60269-2 (cylindrical fuse links, e.g. for fuse disconnectors). Highest usual rated currents; smaller depending on "
                 "manufacturer and voltage – check the data sheet."},
                {{"Abmessung", "Size"}, {"Bemessungsstrom bis", "Rated current up to"}, {"Spannung", "Voltage"}, {"Hinweis", "Note"}},
                {{"8,5 × 31,5 mm", "20 A", "400 V AC", {"kleine Verbraucher", "small loads"}},
                 {"10 × 38 mm", "32 A", "400–690 V AC", {"auch gPV bis 1000/1500 V DC", "also gPV up to 1000/1500 V DC"}},
                 {"14 × 51 mm", "50 A", "400–690 V AC", {"auch aM für Motoren", "also aM for motors"}},
                 {"22 × 58 mm", "125 A", "400–690 V AC", {"auch aM für Motoren", "also aM for motors"}}});
}

Table fuseGlass() {
    return make("fuse_glass", {"Feinsicherungen (G-Sicherungen)", "Miniature fuses (G fuses)"},
                {"Auslösecharakteristik FF … TT, 5 × 20 mm", "Time-current characteristic FF … TT, 5 × 20 mm"},
                {"IEC 60127 / DIN EN 60127. Bauformen 5 × 20 mm und 6,3 × 32 mm. Schaltvermögen: L = niedrig (Glasrohr, typ. 35 A "
                 "oder 10 · I_N), E = erhöht, H = hoch (Keramik mit Sandfüllung, 1500 A). Kennzeichnung z. B. T 1,6 A H 250 V.",
                 "IEC 60127. Sizes 5 × 20 mm and 6.3 × 32 mm. Breaking capacity: L = low (glass tube, typ. 35 A or 10 · I_N), "
                 "E = enhanced, H = high (ceramic with sand filling, 1500 A). Marking e.g. T 1.6 A H 250 V."},
                {{"Zeichen", "Symbol"}, {"Charakteristik", "Characteristic"}, {"Einsatz", "Use"}},
                {{"FF", {"superflink", "very fast acting"}, {"Halbleiter, Messgeräte", "semiconductors, meters"}},
                 {"F", {"flink", "fast acting"}, {"ohne Einschaltstromspitzen", "loads without inrush"}},
                 {"M", {"mittelträge", "medium time lag"}, {"kleine Einschaltströme", "small inrush"}},
                 {"T", {"träge", "time lag"}, {"Netzteile, Transformatoren, Motoren", "power supplies, transformers, motors"}},
                 {"TT", {"superträge", "long time lag"}, {"hohe Einschaltströme", "high inrush"}}});
}

Table fuseAutomotive() {
    return make("fuse_automotive", {"Kfz-Flachsicherungen: Farben", "Automotive blade fuses: colours"},
                {"Bemessungsstrom und Farbe (Standard und Mini)", "Rating and colour (standard and mini)"},
                {"ISO 8820-3 (Flachstecksicherungen ATO/ATC und Mini). 35 A ist nicht in allen Bauformen üblich. "
                 "Maxi-Sicherungen verwenden teilweise andere Farben.",
                 "ISO 8820-3 (blade fuses ATO/ATC and mini). 35 A is not common in all designs. Maxi fuses partly use other colours."},
                {{"Strom", "Current"}, {"Farbe", "Colour"}},
                {{"1 A", {"schwarz", "black"}},       {"2 A", {"grau", "grey"}},        {"3 A", {"violett", "violet"}},
                 {"4 A", {"rosa", "pink"}},           {"5 A", {"hellbraun", "tan"}},    {"7,5 A", {"braun", "brown"}},
                 {"10 A", {"rot", "red"}},            {"15 A", {"blau", "blue"}},       {"20 A", {"gelb", "yellow"}},
                 {"25 A", {"farblos/weiß", "clear/white"}}, {"30 A", {"grün", "green"}}, {"35 A", {"blaugrün", "blue-green"}},
                 {"40 A", {"orange", "amber"}}});
}

// ------------------------------------------------------------------ component codes

Table eia96Table() {
    std::vector<Row> rows;
    const auto v = eia96Values();
    for (std::size_t i = 0; i < 24; ++i) {
        Row row;
        for (std::size_t k = 0; k < 4; ++k) {
            const std::size_t idx = i + 24 * k;
            const std::string code = (idx + 1 < 10 ? "0" : "") + std::to_string(idx + 1);
            row.push_back({code, code});
            row.push_back(num(v[idx], 0));
        }
        rows.push_back(row);
    }
    return make("eia96_table", {"EIA-96-Code (SMD 1 %)", "EIA-96 code (SMD 1 %)"}, {"Code 01–96 → Wert der E96-Reihe", "Code 01–96 → E96 value"},
                {"Zwei Ziffern = Index der E96-Reihe, danach Multiplikator-Buchstabe: Z ×0,001, Y/R ×0,01, X/S ×0,1, A ×1, B/H ×10, "
                 "C ×100, D ×1000, E ×10⁴, F ×10⁵. Beispiel 01C = 100 · 100 = 10 kΩ, 68X = 499 · 0,1 = 49,9 Ω.",
                 "Two digits = index of the E96 series, then the multiplier letter: Z ×0.001, Y/R ×0.01, X/S ×0.1, A ×1, B/H ×10, "
                 "C ×100, D ×1000, E ×10⁴, F ×10⁵. Example 01C = 100 · 100 = 10 kΩ, 68X = 499 · 0.1 = 49.9 Ω."},
                {"Code", {"Wert", "Value"}, "Code", {"Wert", "Value"}, "Code", {"Wert", "Value"}, "Code", {"Wert", "Value"}}, rows);
}

Table capacitorCodes() {
    std::vector<Row> rows;
    for (int e = 0; e <= 5; ++e) {
        for (const int m : {10, 15, 22, 33, 47, 68}) {
            const double pF = m * std::pow(10.0, e);
            const std::string code = std::to_string(m) + std::to_string(e);
            rows.push_back({{code, code}, num(pF, 0), num(pF / 1e3, 3), num(pF / 1e6, 6)});
        }
    }
    return make("capacitor_code_table", {"Kondensatorcode-Tabelle", "Capacitor code table"},
                {"3-stelliger Code → pF, nF, µF (E6-Werte)", "3-digit code → pF, nF, µF (E6 values)"},
                {"Zwei Ziffern + Anzahl Nullen in pF (EIA/IEC 60062). Toleranzbuchstabe danach: J ±5 %, K ±10 %, M ±20 %. "
                 "Andere Werte siehe Rechner Kondensator-Code.",
                 "Two digits + number of zeros in pF (EIA/IEC 60062). Tolerance letter after: J ±5 %, K ±10 %, M ±20 %. "
                 "Other values see the capacitor code calculator."},
                {"Code", "pF", "nF", "µF"}, rows);
}

Table capacitorVoltages() {
    std::vector<Row> rows;
    for (const auto& v : capacitorVoltageCodes()) rows.push_back({{std::string(v.code), std::string(v.code)}, num(v.volts, 1)});
    return make("capacitor_voltage_codes", {"Kondensator-Spannungscodes (EIA)", "Capacitor voltage codes (EIA)"},
                {"Vorangestellter Code, z. B. 2A = 100 V", "Prefix code, e.g. 2A = 100 V"},
                {"EIA-Spannungscode auf Folien-, Keramik- und Elektrolytkondensatoren (z. B. 2A104J = 100 V, 100 nF, ±5 %).",
                 "EIA voltage code on film, ceramic and electrolytic capacitors (e.g. 2A104J = 100 V, 100 nF, ±5 %)."},
                {"Code", {"Nennspannung in V", "Rated voltage in V"}}, rows);
}

// ------------------------------------------------------------------ motors

Table necTable(int phases) {
    std::vector<Row> rows;
    const auto volts = necVoltages(phases);
    for (const double hp : necHorsepowers(phases)) {
        std::string h;
        if (std::fabs(hp - 1.0 / 6.0) < 1e-6) h = "1/6";
        else if (std::fabs(hp - 0.25) < 1e-6) h = "1/4";
        else if (std::fabs(hp - 1.0 / 3.0) < 1e-6) h = "1/3";
        else if (std::fabs(hp - 0.5) < 1e-6) h = "1/2";
        else if (std::fabs(hp - 0.75) < 1e-6) h = "3/4";
        else if (std::fabs(hp - 1.5) < 1e-6) h = "1 1/2";
        else if (std::fabs(hp - 7.5) < 1e-6) h = "7 1/2";
        else h = formatNumber(hp, 0, Lang::En);
        Row row{{h, h}, num(hp * kWattPerHp / 1e3, 2)};
        for (const int v : volts) {
            const auto a = necFullLoadCurrent(phases, hp, v);
            row.push_back(a ? num(*a, 1) : dash());
        }
        rows.push_back(row);
    }
    Row cols{"hp", "kW"};
    for (const int v : volts) cols.push_back({std::to_string(v) + " V", std::to_string(v) + " V"});
    const bool three = phases == 3;
    return make(three ? "nec_flc_3ph" : "nec_flc_1ph",
                three ? Text{"NEC 430.250: Volllaststrom Drehstrommotoren", "NEC 430.250: full-load current three-phase motors"}
                      : Text{"NEC 430.248: Volllaststrom Einphasenmotoren", "NEC 430.248: full-load current single-phase motors"},
                {"Tabellenströme in A (Nordamerika)", "Table currents in A (North America)"},
                {"NEC (NFPA 70) Tab. 430.250 (Drehstrom-Asynchronmotoren) bzw. 430.248 (Einphasen-Wechselstrommotoren); für Leiter "
                 "und Schutz nach 430.6(A)(1). Gegen die gültige NEC-Ausgabe prüfen.",
                 "NEC (NFPA 70) Tab. 430.250 (three-phase induction motors) or 430.248 (single-phase AC motors); used for conductors "
                 "and protection per 430.6(A)(1). Check against the NEC edition in force."},
                cols, rows);
}

Table motorMarkings() {
    return make("motor_terminal_markings", {"Kennzeichnung der Motorklemmen", "Motor terminal markings"},
                {"IEC 60034-8, alte DIN-Bezeichnung und NEMA", "IEC 60034-8, old DIN marking and NEMA"},
                {"IEC 60034-8 (DIN EN 60034-8): Ziffer 1 = Wicklungsanfang, 2 = Wicklungsende; vorangestellte Ziffer = Drehzahlstufe "
                 "bzw. Wicklung. Alte Bezeichnung nach DIN VDE 0530 (bis 1977). NEMA MG 1: T-Nummern. Typenschild hat Vorrang.",
                 "IEC 60034-8: digit 1 = start, 2 = end of winding; prefixed digit = speed step or winding. Old marking per "
                 "DIN VDE 0530 (until 1977). NEMA MG 1: T numbers. The nameplate takes precedence."},
                {{"Maschine / Wicklung", "Machine / winding"}, "IEC 60034-8", {"Alt (DIN VDE 0530)", "Old (DIN VDE 0530)"}, "NEMA MG 1"},
                {{{"Drehstrom, 6 Klemmen", "Three-phase, 6 terminals"}, "U1 V1 W1 / U2 V2 W2", "U V W / X Y Z", "T1 T2 T3 / T4 T5 T6"},
                 {{"Drehstrom, 9 Klemmen", "Three-phase, 9 leads"}, dash(), dash(), "T1 … T9"},
                 {{"Drehstrom, 12 Klemmen", "Three-phase, 12 leads"}, "U1 U2 U5 U6, V…, W…", dash(), "T1 … T12"},
                 {{"Dahlander (2 Drehzahlen)", "Dahlander (2 speeds)"}, "1U 1V 1W / 2U 2V 2W", dash(), dash()},
                 {{"Zwei getrennte Wicklungen", "Two separate windings"}, "1U1 … 1W2 / 2U1 … 2W2", dash(), dash()},
                 {{"Einphasen: Haupt-/Hilfswicklung", "Single-phase: main/auxiliary winding"}, "U1–U2 / Z1–Z2", dash(), "T1–T4 / T5, T8"},
                 {{"Gleichstrom: Anker", "DC: armature"}, "A1–A2", "A–B", "A1–A2"},
                 {{"Gleichstrom: Nebenschluss", "DC: shunt field"}, "E1–E2", "C–D", "F1–F2"},
                 {{"Gleichstrom: Reihenschluss", "DC: series field"}, "D1–D2", "E–F", "S1–S2"},
                 {{"Gleichstrom: Wendepol", "DC: interpole"}, "B1–B2", "G–H", dash()},
                 {{"Gleichstrom: Fremderregung", "DC: separate excitation"}, "F1–F2", "J–K", "F1–F2"}});
}

// ------------------------------------------------------------------ wire gauges

Table awgTable() {
    std::vector<Row> rows;
    for (int n = -3; n <= 40; ++n) {
        const std::string name = n <= 0 ? std::to_string(1 - n) + "/0" : std::to_string(n);
        const double A = units::awgArea(n);
        const double R = cables::resistivity20(cables::Material::Cu) / A * 1000.0;
        const auto m = nextAtLeast(cables::kStandardAreas, A);
        rows.push_back({{name, name}, num(units::awgDiameter(n), 3), num(A, A < 1 ? 4 : 2), num(R, R < 1 ? 4 : 2), m ? num(*m, 2) : dash()});
    }
    return make("awg_table", {"AWG-Umrechnungstabelle", "AWG conversion table"}, {"American Wire Gauge 4/0 … 40", "American Wire Gauge 4/0 … 40"},
                {"ASTM B258: d = 0,127 mm · 92^((36 − n)/39), n = 0 für 1/0, −1 für 2/0 …; A = π/4 · d²; R für Kupfer (massiv) bei 20 °C "
                 "mit ρ = 0,017241 Ω·mm²/m. Letzte Spalte: nächster metrischer Normquerschnitt ≥ AWG-Querschnitt.",
                 "ASTM B258: d = 0.127 mm · 92^((36 − n)/39), n = 0 for 1/0, −1 for 2/0 …; A = π/4 · d²; R for solid copper at 20 °C "
                 "with ρ = 0.017241 Ω·mm²/m. Last column: next metric standard size ≥ AWG area."},
                {"AWG", {"d in mm", "d in mm"}, {"A in mm²", "A in mm²"}, {"R in Ω/km", "R in Ω/km"}, {"metrisch ≥", "metric ≥"}}, rows);
}

Table swgTableDef() {
    std::vector<Row> rows;
    for (const auto& g : swgTable()) {
        const double d = g.inch * kMmPerInch;
        rows.push_back({{std::string(g.name), std::string(g.name)}, num(g.inch, 4), num(d, 3), num(std::numbers::pi / 4 * d * d, d < 1 ? 4 : 2)});
    }
    return make("swg_table", {"SWG-Umrechnungstabelle", "SWG conversion table"}, {"Imperial Standard Wire Gauge 7/0 … 50", "Imperial Standard Wire Gauge 7/0 … 50"},
                {"Britische Drahtlehre (Imperial Standard Wire Gauge, 1884; BS 3737), Durchmesser in Zoll definiert, 1 in = 25,4 mm.",
                 "British wire gauge (Imperial Standard Wire Gauge, 1884; BS 3737), diameters defined in inches, 1 in = 25.4 mm."},
                {"SWG", {"d in Zoll", "d in inch"}, {"d in mm", "d in mm"}, {"A in mm²", "A in mm²"}}, rows);
}

Table wireCompare() {
    struct Imp {
        std::string name;
        double mm2;
    };
    std::vector<Imp> imp;
    for (int n = 20; n >= -3; --n) imp.push_back({n <= 0 ? std::to_string(1 - n) + "/0 AWG" : std::to_string(n) + " AWG", units::awgArea(n)});
    for (const int k : {250, 300, 350, 400, 500, 600, 750, 1000}) imp.push_back({std::to_string(k) + " kcmil", k * kMm2PerKcmil});
    std::vector<Row> rows;
    for (const double a : cables::kStandardAreas) {
        const Imp* best = &imp.front();
        for (const auto& i : imp) {
            if (std::fabs(std::log(i.mm2 / a)) < std::fabs(std::log(best->mm2 / a))) best = &i;
        }
        const Imp* atLeast = nullptr;
        for (const auto& i : imp) {
            if (i.mm2 >= a * (1 - 1e-9)) {
                atLeast = &i;
                break;
            }
        }
        rows.push_back({num(a, 2), {best->name, best->name}, num(best->mm2, 2), num((best->mm2 / a - 1) * 100, 1),
                        atLeast ? Text{atLeast->name, atLeast->name} : dash()});
    }
    return make("wire_size_compare", {"Vergleich imperialer/metrischer Leitergrößen", "Imperial/metric conductor sizes"},
                {"mm² ↔ AWG/kcmil", "mm² ↔ AWG/kcmil"},
                {"Für jeden metrischen Normquerschnitt (IEC 60228) die nächstliegende AWG-/kcmil-Größe (logarithmisch) und die "
                 "nächstgrößere. Größen sind nicht gleichwertig – Belastbarkeit immer nach der jeweiligen Norm (IEC bzw. NEC) bestimmen.",
                 "For each metric standard size (IEC 60228) the nearest AWG/kcmil size (logarithmic) and the next larger one. Sizes "
                 "are not equivalent – always determine the ampacity per the respective standard (IEC or NEC)."},
                {{"mm² (IEC)", "mm² (IEC)"}, {"Nächste US-Größe", "Nearest US size"}, {"deren mm²", "its mm²"}, {"Abweichung in %", "Deviation in %"},
                 {"US-Größe ≥", "US size ≥"}},
                rows);
}

}  // namespace

void addElectricalTables(std::vector<Table>& out) {
    out.push_back(letThrough());
    out.push_back(busbarTable());
    out.push_back(miTable());
    out.push_back(fuseClasses());
    out.push_back(fuseSizes());
    out.push_back(fuseCylindrical());
    out.push_back(fuseGlass());
    out.push_back(fuseAutomotive());
    out.push_back(eia96Table());
    out.push_back(capacitorCodes());
    out.push_back(capacitorVoltages());
    out.push_back(necTable(3));
    out.push_back(necTable(1));
    out.push_back(motorMarkings());
    out.push_back(awgTable());
    out.push_back(swgTableDef());
    out.push_back(wireCompare());
}

std::vector<CategoryGroup> electricalGroups() {
    return {
        {{"Strom, Spannung & Leistung", "Current, voltage & power"},
         {"current_from_power", "voltage_from_power", "power", "power_factor_calc", "ohm", "conductor_resistance", "impedance_ui",
          "reactance", "operating_current", "joule_effect"}},
        {{"Leitungen & Schutz", "Cables & protection"},
         {"cable_sizing", "overload_protection", "voltage_drop", "max_length_vdrop", "disconnection", "ampacity", "ampacity_mi",
          "busbar_ampacity", "conduit_fill", "short_circuit_withstand", "let_through_table", "cable_rxz", "cable_temperature",
          "cable_losses", "neutral_current", "vdrop_distributed", "sizing_distributed", "mi_cable_table", "busbar_table"}},
        {{"Sicherungen", "Fuses"},
         {"fuse_classes", "fuse_d_nh", "fuse_colours", "fuse_cylindrical", "fuse_glass", "fuse_automotive", "mcb_characteristics"}},
        {{"Kurzschluss, Erdung & Überspannung", "Short circuit, earthing & overvoltage"},
         {"short_circuit", "transformer", "short_circuit_min", "earthing_rcd", "surge_risk", "network_systems"}},
        {{"Blindleistungskompensation", "Power factor correction"}, {"pf_correction", "transformer_pfc", "capacitor_voltage"}},
        {{"Bauteile & Codes", "Components & codes"},
         {"resistor_color", "color_from_value", "smd_resistor", "eia96_table", "capacitor_code", "capacitor_code_table",
          "capacitor_voltage_codes", "inductor_code", "e_series", "resistor_colours"}},
        {{"Schaltungen", "Circuits"},
         {"resistors_series_parallel", "capacitors_series_parallel", "lc_resonance", "voltage_divider", "current_divider",
          "zener_regulator", "series_resistor", "led_resistor", "rc", "battery_runtime"}},
        {{"Transformator, Antenne, CCTV & Sensoren", "Transformer, antenna, CCTV & sensors"},
         {"transformer_windings", "antenna_length", "cctv_storage", "rtd", "rtd_cu", "ntc", "thermocouple", "analog_scaling"}},
        {{"Motor", "Motor"},
         {"motor_current", "motor_power", "motor_voltage", "motor_pf", "motor_efficiency", "steinmetz", "motor_speed", "torque",
          "nec_flc", "nec_flc_3ph", "nec_flc_1ph", "motor_wiring_6", "motor_wiring_9", "motor_wiring_12", "motor_connections",
          "motor_terminal_markings", "motor_terminals", "insulation_classes", "star_delta", "motor_duty", "motor_ie"}},
        {{"Ressourcen", "Resources"},
         {"fuse_classes", "fuse_d_nh", "ul_fuse_classes", "e_series_table", "trip_curves", "trip_characteristics", "mcb_characteristics",
          "cable_reactance_table", "resistivity_table", "vdrop_per_length", "conductor_data", "earthing_systems", "network_systems",
          "ip_first", "ip_second", "ik_codes", "nema_enclosures", "ex_selection", "ex_zones", "protection_classes", "cctv_resolutions",
          "thermocouple_table", "ansi_device_numbers", "device_acronyms", "electrical_symbols", "world_electricity", "plug_types",
          "iec60320_connectors", "iec60309_sockets", "nema_connectors", "ev_plugs", "conductor_colours", "wiring_colours_intl",
          "si_prefixes", "units_table", "pipe_dimensions", "cable_glands"}},
        {{"Umrechnung", "Conversion"},
         {"delta_star", "conv_power", "conv_awg", "awg_table", "swg_table", "wire_size_compare", "conv_area", "conv_length",
          "conv_voltage", "conv_phase", "conv_energy", "conv_temperature", "conv_pressure"}},
    };
}

}  // namespace elc::catalog
