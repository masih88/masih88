// Reference tables: protection, colours, cables, fieldbuses, motors, pinouts.
// Each entry should name its source standard. Please verify against the edition in force.
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/cables.hpp"

namespace elc::catalog {

namespace {

using Row = std::vector<Text>;

Table make(std::string id, Text title, Text summary, Text description, Row columns, std::vector<Row> rows) {
    Table t;
    t.id = std::move(id);
    t.category = "tables";
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

Table ipCodes() {
    const Text none{"kein Schutz", "no protection"};
    return make("ip_codes", {"IP-Schutzarten (IP-Code)", "IP codes (ingress protection)"},
                {"1. Ziffer Fremdkörper/Berührung, 2. Ziffer Wasser", "1st digit solids/contact, 2nd digit water"},
                {"IEC 60529 / EN 60529 (DIN VDE 0470-1) inkl. Änderung 2 (2013) für IPX9. Beispiel IP54: staubgeschützt, Draht; "
                 "Spritzwasser. X = nicht geprüft. Zusatzbuchstaben A–D (Berührungsschutz) und H, M, S, W möglich.",
                 "IEC 60529 incl. amendment 2 (2013) for IPX9. Example IP54: dust-protected, wire; splashing water. X = not tested. "
                 "Additional letters A–D (contact) and H, M, S, W possible."},
                {{"Ziffer", "Digit"}, {"1. Ziffer: Fremdkörper", "1st digit: solid objects"}, {"1. Ziffer: Berührung", "1st digit: contact"},
                 {"2. Ziffer: Wasser", "2nd digit: water"}},
                {{"0", none, none, none},
                 {"1", {"≥ 50 mm", "≥ 50 mm"}, {"Handrücken", "back of hand"}, {"senkrecht fallendes Tropfwasser", "vertically dripping water"}},
                 {"2", {"≥ 12,5 mm", "≥ 12.5 mm"}, {"Finger", "finger"}, {"Tropfwasser bis 15° Neigung", "dripping water, tilted up to 15°"}},
                 {"3", {"≥ 2,5 mm", "≥ 2.5 mm"}, {"Werkzeug", "tool"}, {"Sprühwasser bis 60° gegen die Senkrechte", "spraying water up to 60° from vertical"}},
                 {"4", {"≥ 1,0 mm", "≥ 1.0 mm"}, {"Draht", "wire"}, {"Spritzwasser aus allen Richtungen", "splashing water from any direction"}},
                 {"5", {"staubgeschützt", "dust-protected"}, {"Draht", "wire"}, {"Strahlwasser (Düse 6,3 mm)", "water jets (6.3 mm nozzle)"}},
                 {"6", {"staubdicht", "dust-tight"}, {"Draht", "wire"}, {"starkes Strahlwasser (Düse 12,5 mm)", "powerful water jets (12.5 mm nozzle)"}},
                 {"7", "–", "–", {"zeitweiliges Untertauchen (1 m, 30 min)", "temporary immersion (1 m, 30 min)"}},
                 {"8", "–", "–", {"dauerndes Untertauchen (nach Vereinbarung)", "continuous immersion (as agreed)"}},
                 {"9", "–", "–", {"Hochdruck-/Dampfstrahl (80 °C), IPX9", "high-pressure, high-temperature jets (80 °C), IPX9"}}});
}

Table protectionClasses() {
    return make("protection_classes", {"Schutzklassen", "Protection classes"}, {"Schutz gegen elektrischen Schlag", "Protection against electric shock"},
                {"IEC 61140 / DIN EN 61140 (VDE 0140-1)", "IEC 61140"},
                {{"Klasse", "Class"}, {"Schutzmaßnahme", "Protective measure"}, {"Kennzeichen / Hinweis", "Marking / note"}},
                {{"0", {"nur Basisisolierung", "basic insulation only"}, {"in DE/CH nicht zulässig", "not permitted in DE/CH"}},
                 {"I", {"Basisisolierung + Schutzleiteranschluss", "basic insulation + PE connection"}, {"Schutzleiterzeichen ⏚", "PE symbol ⏚"}},
                 {"II", {"doppelte oder verstärkte Isolierung", "double or reinforced insulation"}, {"Doppelquadrat ⧈, kein PE", "double square ⧈, no PE"}},
                 {"III", {"Schutzkleinspannung SELV/PELV", "extra-low voltage SELV/PELV"}, {"≤ 50 V AC / 120 V DC", "≤ 50 V AC / 120 V DC"}}});
}

Table insulationClasses() {
    return make("insulation_classes", {"Isolierstoffklassen", "Thermal classes"}, {"Max. Temperatur, Motor-Grenzübertemperatur", "Max. temperature, motor temperature rise"},
                {"IEC 60085 (Wärmeklassen); Grenzübertemperatur Wicklung nach IEC 60034-1 (Widerstandsmethode, Kühlmittel 40 °C).",
                 "IEC 60085 (thermal classes); winding temperature rise per IEC 60034-1 (resistance method, coolant 40 °C)."},
                {{"Klasse", "Class"}, {"Max. Temperatur", "Max. temperature"}, {"Grenzübertemperatur Motor", "Motor temperature rise"}},
                {{"Y", "90 °C", "–"},
                 {"A", "105 °C", "60 K"},
                 {"E", "120 °C", "75 K"},
                 {"B", "130 °C", "80 K"},
                 {"F", "155 °C", "105 K"},
                 {"H", "180 °C", "125 K"},
                 {"N", "200 °C", "–"},
                 {"R", "220 °C", "–"}});
}

Table voltageDropLimits() {
    return make("voltage_drop_limits", {"Spannungsfall-Grenzwerte DE/CH", "Voltage drop limits DE/CH"}, {"Empfohlene Höchstwerte", "Recommended maximum values"},
                {"Übersicht – maßgeblich sind die gültigen Normausgaben und die TAB des Netzbetreibers.",
                 "Overview – the editions in force and local utility rules are authoritative."},
                {{"Land", "Country"}, {"Regel", "Rule"}, {"Bereich", "Section"}, {"ΔU max", "ΔU max"}},
                {{"DE", "DIN 18015-1", {"Zähler bis Verbraucher (Wohngebäude)", "meter to load (residential)"}, "3 %"},
                 {"DE", "VDE-AR-N 4100 (TAB)", {"Hauptleitung bis Zähler", "service to meter"}, {"0,5 % (≤ 100 kVA)", "0.5 % (≤ 100 kVA)"}},
                 {"DE", {"DIN VDE 0100-520 Anh. G", "DIN VDE 0100-520 Annex G"}, {"öff. Netz – Beleuchtung / sonstige", "public LV – lighting / other"}, "3 % / 5 %"},
                 {"DE", {"DIN VDE 0100-520 Anh. G", "DIN VDE 0100-520 Annex G"}, {"eigene Trafostation – Beleuchtung / sonstige", "private supply – lighting / other"}, "6 % / 8 %"},
                 {"CH", "NIN", {"Anschlussüberstromunterbrecher bis Verbraucher", "service fuse to load"}, "4 %"}});
}

Table ampacityTable(cables::Insulation ins, int loaded) {
    using namespace cables;
    const bool pvc = ins == Insulation::PVC;
    const std::string insDe = pvc ? "PVC 70 °C" : "VPE 90 °C";
    const std::string insEn = pvc ? "PVC 70 °C" : "XLPE 90 °C";
    const std::string n = std::to_string(loaded);
    const std::string table = pvc ? (loaded == 2 ? "B.52.2" : "B.52.4") : (loaded == 2 ? "B.52.3" : "B.52.5");
    Table t = make("ampacity_" + std::string(pvc ? "pvc" : "xlpe") + n,
                   {"Strombelastbarkeit Cu " + insDe + ", " + n + " belastete Adern", "Ampacity Cu " + insEn + ", " + n + " loaded conductors"},
                   {"I_z0 in A bei 30 °C (Luft) bzw. 20 °C (Erde)", "I_z0 in A at 30 °C (air) or 20 °C (ground)"},
                   {"IEC 60364-5-52 Tabelle " + table + " (Methode E: B.52.10/11); entspricht DIN VDE 0298-4 / NIN 5.2.3. Gegen gültige Ausgabe prüfen.",
                    "IEC 60364-5-52 Table " + table + " (method E: B.52.10/11). Check against the edition in force."},
                   {"mm²", "A1", "A2", "B1", "B2", "C", "D1", "E"}, {});
    const Method methods[] = {Method::A1, Method::A2, Method::B1, Method::B2, Method::C, Method::D1, Method::E};
    for (const double a : kAmpacityAreas) {
        Row row{formatNumber(a, 1)};
        for (const auto m : methods) row.push_back(formatNumber(baseAmpacity(ins, m, loaded, a).value(), 1));
        t.rows.push_back(std::move(row));
    }
    return t;
}

Table temperatureFactors() {
    using namespace cables;
    Table t = make("temperature_factors", {"Umrechnungsfaktoren Temperatur", "Ambient temperature factors"},
                   {"f_T für Luft (30 °C) und Erde (20 °C)", "f_T for air (30 °C) and ground (20 °C)"},
                   {"f_T = √((θ_max − θ_a)/(θ_max − θ_ref)) – entspricht IEC 60364-5-52 Tab. B.52.14 / B.52.15.",
                    "f_T = √((θ_max − θ_a)/(θ_max − θ_ref)) – matches IEC 60364-5-52 Tab. B.52.14 / B.52.15."},
                   {{"θ_a", "θ_a"}, {"Luft PVC", "Air PVC"}, {"Luft VPE", "Air XLPE"}, {"Erde PVC", "Ground PVC"}, {"Erde VPE", "Ground XLPE"}}, {});
    for (int ta = 10; ta <= 80; ta += 5) {
        const auto f = [&](Insulation ins, Method m) {
            const double v = ambientFactor(ins, m, ta);
            return v > 0 ? formatNumber(v, 2) : Text{"–"};
        };
        t.rows.push_back({std::to_string(ta) + " °C", f(Insulation::PVC, Method::C), f(Insulation::XLPE, Method::C),
                          f(Insulation::PVC, Method::D1), f(Insulation::XLPE, Method::D1)});
    }
    return t;
}

Table groupingFactors() {
    using namespace cables;
    Table t = make("grouping_factors", {"Häufungsfaktoren", "Grouping factors"}, {"f_G nach Anzahl Stromkreise", "f_G by number of circuits"},
                   {"IEC 60364-5-52 Tabelle B.52.17 (mehradrige Kabel/Stromkreise).", "IEC 60364-5-52 Table B.52.17 (multi-core cables/circuits)."},
                   {{"Anzahl", "Count"}, {"gebündelt", "bunched"}, {"Wand", "wall"}, {"Decke", "ceiling"}, {"gelochte Rinne", "perf. tray"}, {"Leiter", "ladder"}}, {});
    for (int n : {1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 20}) {
        t.rows.push_back({std::to_string(n), formatNumber(groupingFactor(Arrangement::Bunched, n), 2),
                          formatNumber(groupingFactor(Arrangement::SingleLayerWall, n), 2),
                          formatNumber(groupingFactor(Arrangement::SingleLayerCeiling, n), 2),
                          formatNumber(groupingFactor(Arrangement::PerforatedTray, n), 2),
                          formatNumber(groupingFactor(Arrangement::Ladder, n), 2)});
    }
    return t;
}

Table cableTypes() {
    return make("cable_types", {"Kabel- & Leitungstypen", "Cable types"}, {"Häufige Typen in GA und Industrie", "Common types in BMS and industry"},
                {"Kurzübersicht – Datenblatt des Herstellers ist maßgeblich.", "Overview – the manufacturer datasheet is authoritative."},
                {{"Typ", "Type"}, {"Aufbau", "Construction"}, {"U_0/U", "U_0/U"}, {"Einsatz", "Use"}},
                {{"NYM-J", {"PVC-Mantelleitung", "PVC sheathed cable"}, "300/500 V", {"innen, auf/unter Putz, nicht im Erdreich", "indoors, surface/flush, not buried"}},
                 {"NYY-J", {"PVC-Kabel", "PVC power cable"}, "0,6/1 kV", {"Erdreich, außen, Kabelrinne", "buried, outdoor, trays"}},
                 {"NYCWY", {"Kabel mit konz. Leiter", "cable with concentric conductor"}, "0,6/1 kV", {"EMV-günstige Einspeisung, Erdreich", "EMC-friendly feeders, buried"}},
                 {"N2XH-J", {"VPE, halogenfrei", "XLPE, halogen-free"}, "0,6/1 kV", {"öffentl. Gebäude, Rechenzentren", "public buildings, data centres"}},
                 {"NHXH FE180 E30/E90", {"halogenfrei mit Funktionserhalt", "halogen-free, circuit integrity"}, "0,6/1 kV",
                  {"Sicherheitsanlagen, RWA, Notlicht", "safety systems, smoke extraction, emergency lighting"}},
                 {"H07RN-F", {"Gummischlauchleitung", "rubber flexible cable"}, "450/750 V", {"Baustelle, außen, flexible Anschlüsse", "site, outdoor, flexible connections"}},
                 {"H05VV-F", {"PVC-Schlauchleitung", "PVC flexible cord"}, "300/500 V", {"Geräteanschluss, leichte Beanspruchung", "appliances, light duty"}},
                 {"H07V-K", {"PVC-Aderleitung feindrähtig", "PVC single core, fine-stranded"}, "450/750 V", {"Schaltschrankverdrahtung, im Rohr", "panel wiring, in conduit"}},
                 {"J-Y(St)Y n×2×0,8", {"Fernmelde-Installationskabel, Folienschirm", "telecom installation cable, foil screen"}, {"300 V Spitze", "300 V peak"},
                  {"Busleitungen (KNX grün), Signale; kein Starkstrom", "bus lines (KNX green), signals; no power"}},
                 {"LiYCY", {"Steuerleitung, Cu-Geflechtschirm", "control cable, braided screen"}, {"Steuer-/Signal", "control/signal"},
                  {"Analogsignale, Fühler, EMV", "analog signals, sensors, EMC"}}});
}

Table fieldbuses() {
    return make("fieldbuses", {"Feldbusse: Längen & Teilnehmer", "Fieldbuses: lengths & nodes"}, {"BACnet, Modbus, KNX, DALI, LON, PROFIBUS, CAN …", "BACnet, Modbus, KNX, DALI, LON, PROFIBUS, CAN …"},
                {"Typische Grenzwerte ohne Repeater laut jeweiliger Spezifikation – Herstellerangaben und Kabeltyp beachten. "
                 "Abgeglichen mit Siemens „Ethernet, TCP/IP, MS/TP und BACnet“ (2023), Kap. 7.1, und KNX Association "
                 "„Grundlagenwissen zum KNX Standard“ (2013), S. 8–9.",
                 "Typical limits without repeaters per the respective specification – check manufacturer data and cable type. "
                 "Checked against Siemens “Ethernet, TCP/IP, MS/TP and BACnet” (2023), ch. 7.1, and KNX Association "
                 "“KNX basic knowledge” (2013), p. 8–9."},
                {{"Bus", "Bus"}, {"Physik", "Physical"}, {"Max. Länge", "Max. length"}, {"Teilnehmer", "Nodes"}, {"Datenrate", "Data rate"}, {"Hinweis", "Note"}},
                {{"BACnet MS/TP", "RS-485", {"1200 m je Segment", "1200 m per segment"}, {"32 Unit Loads/Segment, max. 128 Geräte je Bus, MAC 0–127 (Manager)", "32 unit loads/segment, max. 128 devices per bus, MAC 0–127 (manager)"},
                  "9,6–115,2 kbit/s", {"120 Ω beidseitig, Bias einmal; > 750 m ggf. 76,8 kbit/s", "120 Ω both ends, bias once; > 750 m possibly 76.8 kbit/s"}},
                 {"Modbus RTU", "RS-485", {"ca. 1000–1200 m (baudratenabh.)", "approx. 1000–1200 m (baud-dependent)"}, {"32/Segment, Adressen 1–247", "32/segment, addresses 1–247"},
                  "9,6–115,2 kbit/s", {"120 Ω beidseitig; A/B-Bezeichnung prüfen", "120 Ω both ends; check A/B naming"}},
                 {"KNX TP1", {"2-Draht, freie Topologie", "2-wire, free topology"}, {"Linie 1000 m; 350 m Netzteil–TN; 700 m TN–TN", "line 1000 m; 350 m PSU–device; 700 m device–device"},
                  {"64 je Liniensegment; 4 Segmente = 255 je Linie; 15 Linien × 15 Bereiche", "64 per line segment; 4 segments = 255 per line; 15 lines × 15 areas"}, "9,6 kbit/s", {"SELV 30 V DC, kein Abschluss", "SELV 30 V DC, no termination"}},
                 {"DALI", {"2-Draht, frei", "2-wire, free"}, "300 m", {"64 Betriebsgeräte", "64 control gear"}, "1,2 kbit/s", {"max. 2 V Spannungsfall, 250 mA", "max. 2 V drop, 250 mA"}},
                 {"LON FT-10", {"2-Draht, frei oder Bus", "2-wire, free or bus"}, {"frei 500 m, Bus 2700 m", "free 500 m, bus 2700 m"}, {"64 je Segment", "64 per segment"},
                  "78 kbit/s", {"Terminator: frei 1×, Bus 2×", "terminator: free 1×, bus 2×"}},
                 {"PROFIBUS DP", "RS-485", {"1200 m (≤ 93,75 kbit/s) … 100 m (12 Mbit/s)", "1200 m (≤ 93.75 kbit/s) … 100 m (12 Mbit/s)"},
                  {"32/Segment, 126 Adressen", "32/segment, 126 addresses"}, "9,6 kbit/s – 12 Mbit/s", {"aktiver Abschluss beidseitig", "active termination both ends"}},
                 {"CAN / CANopen", {"2-Draht, Linie", "2-wire, line"}, {"25 m (1 Mbit/s) … 1000 m (50 kbit/s)", "25 m (1 Mbit/s) … 1000 m (50 kbit/s)"},
                  {"bis 127 (CANopen)", "up to 127 (CANopen)"}, "10 kbit/s – 1 Mbit/s", {"120 Ω beidseitig", "120 Ω both ends"}},
                 {"IO-Link", {"Punkt-zu-Punkt, 3/5-Leiter", "point-to-point, 3/5-wire"}, "20 m", {"1 Gerät je Port", "1 device per port"}, "4,8/38,4/230,4 kbit/s",
                  {"ungeschirmte Standardleitung, M12", "unshielded standard cable, M12"}},
                 {"Ethernet (BACnet/IP, Modbus TCP, PROFINET)", {"Twisted Pair, Stern", "twisted pair, star"}, {"100 m je Kupferstrecke", "100 m per copper link"}, "–",
                  "100 Mbit/s +", {"Cat 5e oder besser", "Cat 5e or better"}}});
}

Table motorDuty() {
    return make("motor_duty", {"Motor-Betriebsarten S1–S10", "Motor duty types S1–S10"}, {"IEC 60034-1", "IEC 60034-1"},
                {"Betriebsarten nach IEC 60034-1 / DIN EN 60034-1.", "Duty types per IEC 60034-1."},
                {{"Art", "Type"}, {"Bezeichnung", "Description"}},
                {{"S1", {"Dauerbetrieb", "continuous running duty"}},
                 {"S2", {"Kurzzeitbetrieb", "short-time duty"}},
                 {"S3", {"periodischer Aussetzbetrieb", "intermittent periodic duty"}},
                 {"S4", {"Aussetzbetrieb mit Einfluss des Anlaufs", "intermittent periodic duty with starting"}},
                 {"S5", {"Aussetzbetrieb mit elektrischer Bremsung", "intermittent periodic duty with electric braking"}},
                 {"S6", {"ununterbrochener periodischer Betrieb", "continuous operation periodic duty"}},
                 {"S7", {"ununterbrochener Betrieb mit elektrischer Bremsung", "continuous operation with electric braking"}},
                 {"S8", {"ununterbrochener Betrieb mit Last-/Drehzahländerungen", "continuous operation with load/speed changes"}},
                 {"S9", {"Betrieb mit nichtperiodischen Last- und Drehzahländerungen", "non-periodic load and speed variations"}},
                 {"S10", {"Betrieb mit einzelnen konstanten Belastungen", "discrete constant loads"}}});
}

Table motorIe() {
    return make("motor_ie", {"Motor-Effizienzklassen IE", "Motor efficiency classes IE"}, {"IE1–IE5 und EU-Mindestanforderung", "IE1–IE5 and EU minimum"},
                {"IEC 60034-30-1; EU-Verordnung 2019/1781 (Stand: IE3 ab 0,75–1000 kW seit 07/2021, IE4 für 75–200 kW seit 07/2023). "
                 "In der Schweiz gelten vergleichbare Anforderungen (EnEV).",
                 "IEC 60034-30-1; EU regulation 2019/1781 (IE3 for 0.75–1000 kW since 07/2021, IE4 for 75–200 kW since 07/2023). "
                 "Switzerland has comparable requirements (EnEV)."},
                {{"Klasse", "Class"}, {"Bezeichnung", "Name"}},
                {{"IE1", {"Standard-Wirkungsgrad", "standard efficiency"}},
                 {"IE2", {"Hoher Wirkungsgrad", "high efficiency"}},
                 {"IE3", {"Premium-Wirkungsgrad", "premium efficiency"}},
                 {"IE4", {"Super-Premium-Wirkungsgrad", "super premium efficiency"}},
                 {"IE5", {"Ultra-Premium-Wirkungsgrad", "ultra premium efficiency"}}});
}

Table motorMounting() {
    return make("motor_mounting", {"Motor-Bauformen (IM)", "Motor mounting (IM)"}, {"IEC 60034-7 Code I", "IEC 60034-7 code I"},
                {"Häufige Bauformen nach IEC 60034-7.", "Common mounting arrangements per IEC 60034-7."},
                {{"Bauform", "Code"}, {"Beschreibung", "Description"}},
                {{"IM B3", {"Fußausführung, Welle waagerecht", "foot mounted, horizontal shaft"}},
                 {"IM B5", {"Flansch (FF, Durchgangslöcher), waagerecht", "flange (FF, clearance holes), horizontal"}},
                 {"IM B14", {"Normflansch (FT, Gewindelöcher), waagerecht", "face flange (FT, tapped holes), horizontal"}},
                 {"IM B35", {"Fuß + Flansch FF, waagerecht", "foot + flange FF, horizontal"}},
                 {"IM V1", {"Flansch FF, Welle nach unten", "flange FF, shaft down"}},
                 {"IM V3", {"Flansch FF, Welle nach oben", "flange FF, shaft up"}}});
}

Table pinRj45() {
    return make("pin_rj45", {"Pinout RJ45 / Ethernet", "Pinout RJ45 / Ethernet"}, {"T568A/B, 10/100/1000BASE-T, PoE", "T568A/B, 10/100/1000BASE-T, PoE"},
                {"TIA-568, IEEE 802.3. Patchkabel: beide Seiten gleich (meist T568B). PoE Mode A nutzt Paare 1/2 und 3/6.",
                 "TIA-568, IEEE 802.3. Patch cable: same on both ends (usually T568B). PoE mode A uses pairs 1/2 and 3/6."},
                {"Pin", "T568A", "T568B", "10/100BASE-T", "1000BASE-T", {"PoE Mode B", "PoE mode B"}},
                {{"1", {"weiß-grün", "white-green"}, {"weiß-orange", "white-orange"}, "TX+", "BI_DA+", "–"},
                 {"2", {"grün", "green"}, {"orange", "orange"}, "TX−", "BI_DA−", "–"},
                 {"3", {"weiß-orange", "white-orange"}, {"weiß-grün", "white-green"}, "RX+", "BI_DB+", "–"},
                 {"4", {"blau", "blue"}, {"blau", "blue"}, "–", "BI_DC+", "DC +"},
                 {"5", {"weiß-blau", "white-blue"}, {"weiß-blau", "white-blue"}, "–", "BI_DC−", "DC +"},
                 {"6", {"orange", "orange"}, {"grün", "green"}, "RX−", "BI_DB−", "–"},
                 {"7", {"weiß-braun", "white-brown"}, {"weiß-braun", "white-brown"}, "–", "BI_DD+", "DC −"},
                 {"8", {"braun", "brown"}, {"braun", "brown"}, "–", "BI_DD−", "DC −"}});
}

Table pinUsb() {
    return make("pin_usb", {"Pinout USB 2.0", "Pinout USB 2.0"}, {"Typ A/B und Micro-B", "Type A/B and Micro-B"}, {"USB 2.0 Specification.", "USB 2.0 Specification."},
                {"Pin", {"Typ A/B", "Type A/B"}, {"Aderfarbe", "Wire colour"}, "Micro-B"},
                {{"1", "VBUS +5 V", {"rot", "red"}, "VBUS +5 V"},
                 {"2", "D−", {"weiß", "white"}, "D−"},
                 {"3", "D+", {"grün", "green"}, "D+"},
                 {"4", "GND", {"schwarz", "black"}, "ID"},
                 {"5", "–", "–", "GND"}});
}

Table pinRs232() {
    return make("pin_rs232", {"Pinout RS-232 (D-Sub 9)", "Pinout RS-232 (D-sub 9)"}, {"DTE-Belegung (PC)", "DTE pinout (PC)"},
                {"TIA-232 / DIN 66020, Stecker am DTE (PC). Nullmodem: 2↔3, 7↔8, 4↔6, 5–5.",
                 "TIA-232, male connector on DTE (PC). Null modem: 2↔3, 7↔8, 4↔6, 5–5."},
                {"Pin", {"Signal", "Signal"}, {"Richtung (DTE)", "Direction (DTE)"}, {"Bedeutung", "Meaning"}},
                {{"1", "DCD", {"Eingang", "input"}, "Data Carrier Detect"},
                 {"2", "RxD", {"Eingang", "input"}, "Receive Data"},
                 {"3", "TxD", {"Ausgang", "output"}, "Transmit Data"},
                 {"4", "DTR", {"Ausgang", "output"}, "Data Terminal Ready"},
                 {"5", "GND", "–", "Signal Ground"},
                 {"6", "DSR", {"Eingang", "input"}, "Data Set Ready"},
                 {"7", "RTS", {"Ausgang", "output"}, "Request To Send"},
                 {"8", "CTS", {"Eingang", "input"}, "Clear To Send"},
                 {"9", "RI", {"Eingang", "input"}, "Ring Indicator"}});
}

Table pinRs485() {
    return make("pin_rs485", {"RS-485: Modbus RTU / BACnet MS/TP", "RS-485: Modbus RTU / BACnet MS/TP"}, {"Klemmenbezeichnungen", "Terminal naming"},
                {"Modbus over Serial Line V1.02: D1 = B/B' (positiv bei Ruhepegel „1“), D0 = A/A'. Achtung: Viele Hersteller "
                 "bezeichnen A und B umgekehrt – im Zweifel Polarität im Ruhezustand messen (D1 > D0).",
                 "Modbus over Serial Line V1.02: D1 = B/B' (positive in idle state \"1\"), D0 = A/A'. Note: many manufacturers "
                 "swap the A/B naming – if in doubt measure idle polarity (D1 > D0)."},
                {{"Leitung", "Line"}, {"Modbus", "Modbus"}, {"Übliche Bezeichnungen", "Common names"}, {"Funktion", "Function"}},
                {{"D1", "B / B'", "B, +, Data+, TxD+/RxD+", {"nicht invertierte Leitung", "non-inverting line"}},
                 {"D0", "A / A'", "A, −, Data−, TxD−/RxD−", {"invertierte Leitung", "inverting line"}},
                 {"Common", "C / C'", "GND, 0 V, SC", {"Bezugspotential (mitführen!)", "reference (do connect!)"}},
                 {{"Abschluss", "Termination"}, "120 Ω", {"an beiden Enden", "at both ends"}, {"zwischen D1 und D0", "between D1 and D0"}},
                 {{"Schirm", "Screen"}, "–", "SH", {"nach EMV-Konzept auf PE", "to PE per EMC concept"}}});
}

Table pinM12() {
    return make("pin_m12", {"Pinout M12 A-codiert (Sensor)", "Pinout M12 A-coded (sensor)"}, {"Sensoren, Aktoren, IO-Link", "sensors, actuators, IO-Link"},
                {"IEC 61076-2-101, Aderfarben nach IEC 60947-5-2; IO-Link Port Class A.", "IEC 61076-2-101, wire colours per IEC 60947-5-2; IO-Link port class A."},
                {"Pin", {"Farbe", "Colour"}, {"Funktion", "Function"}, "IO-Link"},
                {{"1", {"braun (BN)", "brown (BN)"}, "L+ (24 V DC)", "L+"},
                 {"2", {"weiß (WH)", "white (WH)"}, {"Ausgang 2 / Öffner", "output 2 / NC"}, "DI/DO"},
                 {"3", {"blau (BU)", "blue (BU)"}, "L− (0 V)", "L−"},
                 {"4", {"schwarz (BK)", "black (BK)"}, {"Schaltausgang Q1", "switching output Q1"}, "C/Q"},
                 {"5", {"grau (GY)", "grey (GY)"}, {"herstellerabhängig / FE", "manufacturer-specific / FE"}, "–"}});
}

Table pinHdmi() {
    Table t = make("pin_hdmi", {"Pinout HDMI Typ A", "Pinout HDMI type A"}, {"19-polig", "19-pin"}, {"HDMI Specification 1.4.", "HDMI Specification 1.4."},
                   {"Pin", {"Signal", "Signal"}, "Pin", {"Signal", "Signal"}}, {});
    const char* pins[19] = {"TMDS Data2+", "TMDS Data2 Shield", "TMDS Data2−", "TMDS Data1+", "TMDS Data1 Shield", "TMDS Data1−",
                            "TMDS Data0+", "TMDS Data0 Shield", "TMDS Data0−", "TMDS Clock+", "TMDS Clock Shield", "TMDS Clock−",
                            "CEC", "Utility / HEAC+", "SCL (DDC)", "SDA (DDC)", "DDC/CEC GND / HEAC Shield", "+5 V",
                            "Hot Plug Detect / HEAC−"};
    for (int i = 0; i < 10; ++i) {
        const int j = i + 10;
        t.rows.push_back({std::to_string(i + 1), pins[i], j < 19 ? std::to_string(j + 1) : std::string("–"), j < 19 ? pins[j] : "–"});
    }
    return t;
}

Table pinKnx() {
    return make("pin_knx", {"KNX-Busleitung", "KNX bus cable"}, {"Aderbelegung", "Core assignment"},
                {"KNX TP1, Busleitung z. B. J-Y(St)Y 2×2×0,8 bzw. KNX-zertifiziert (grüner Mantel).",
                 "KNX TP1, bus cable e.g. J-Y(St)Y 2×2×0.8 or KNX-certified (green sheath)."},
                {{"Ader", "Core"}, {"Funktion", "Function"}},
                {{{"rot", "red"}, {"Bus + (KNX+)", "bus + (KNX+)"}},
                 {{"schwarz", "black"}, {"Bus − (KNX−)", "bus − (KNX−)"}},
                 {{"gelb", "yellow"}, {"Reserve / Zusatzspannung", "spare / auxiliary supply"}},
                 {{"weiß", "white"}, {"Reserve / Zusatzspannung", "spare / auxiliary supply"}}});
}

Table siPrefixes() {
    return make("si_prefixes", {"SI-Vorsätze", "SI prefixes"}, {"T … p", "T … p"}, {"ISO 80000-1 / SI-Broschüre.", "ISO 80000-1 / SI brochure."},
                {{"Vorsatz", "Prefix"}, {"Zeichen", "Symbol"}, {"Faktor", "Factor"}},
                {{"Tera", "T", "10¹²"}, {"Giga", "G", "10⁹"},   {"Mega", "M", "10⁶"},   {"Kilo", "k", "10³"},
                 {"Hekto", "h", "10²"}, {"Deka", "da", "10¹"},  {"Dezi", "d", "10⁻¹"},  {"Zenti", "c", "10⁻²"},
                 {"Milli", "m", "10⁻³"}, {"Mikro", "µ", "10⁻⁶"}, {"Nano", "n", "10⁻⁹"}, {"Piko", "p", "10⁻¹²"}});
}

}  // namespace

void addTables(std::vector<Table>& out) {
    out.push_back(ipCodes());
    out.push_back(protectionClasses());
    out.push_back(voltageDropLimits());
    out.push_back(ampacityTable(cables::Insulation::PVC, 2));
    out.push_back(ampacityTable(cables::Insulation::PVC, 3));
    out.push_back(ampacityTable(cables::Insulation::XLPE, 2));
    out.push_back(ampacityTable(cables::Insulation::XLPE, 3));
    out.push_back(temperatureFactors());
    out.push_back(groupingFactors());
    out.push_back(cableTypes());
    out.push_back(fieldbuses());
    out.push_back(insulationClasses());
    out.push_back(motorDuty());
    out.push_back(motorIe());
    out.push_back(motorMounting());
    out.push_back(pinRj45());
    out.push_back(pinRs485());
    out.push_back(pinM12());
    out.push_back(pinKnx());
    out.push_back(pinRs232());
    out.push_back(pinUsb());
    out.push_back(pinHdmi());
    out.push_back(siPrefixes());
}

}  // namespace elc::catalog
