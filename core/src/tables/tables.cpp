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

Table ipFirst() {
    return make("ip_first", {"IP-Schutzart – 1. Ziffer", "IP code – 1st digit"},
                {"Schutz gegen Fremdkörper und Berühren", "Protection against solids and contact"},
                {"IEC 60529 / EN 60529 (DIN VDE 0470-1)", "IEC 60529 / EN 60529"},
                {{"Ziffer", "Digit"}, {"Fremdkörperschutz", "Solid objects"}, {"Berührungsschutz", "Contact protection"}},
                {{"0", {"kein Schutz", "no protection"}, {"kein Schutz", "no protection"}},
                 {"1", {"≥ 50 mm", "≥ 50 mm"}, {"Handrücken", "back of hand"}},
                 {"2", {"≥ 12,5 mm", "≥ 12.5 mm"}, {"Finger", "finger"}},
                 {"3", {"≥ 2,5 mm", "≥ 2.5 mm"}, {"Werkzeug", "tool"}},
                 {"4", {"≥ 1,0 mm", "≥ 1.0 mm"}, {"Draht", "wire"}},
                 {"5", {"staubgeschützt", "dust-protected"}, {"Draht", "wire"}},
                 {"6", {"staubdicht", "dust-tight"}, {"Draht", "wire"}}});
}

Table ipSecond() {
    return make("ip_second", {"IP-Schutzart – 2. Ziffer", "IP code – 2nd digit"}, {"Schutz gegen Wasser", "Protection against water"},
                {"IEC 60529 inkl. Änderung 2 (2013) für IPX9.", "IEC 60529 incl. amendment 2 (2013) for IPX9."},
                {{"Ziffer", "Digit"}, {"Schutz gegen", "Protection against"}},
                {{"0", {"kein Schutz", "no protection"}},
                 {"1", {"senkrecht fallendes Tropfwasser", "vertically dripping water"}},
                 {"2", {"Tropfwasser bis 15° Neigung", "dripping water, tilted up to 15°"}},
                 {"3", {"Sprühwasser bis 60° gegen die Senkrechte", "spraying water up to 60° from vertical"}},
                 {"4", {"Spritzwasser aus allen Richtungen", "splashing water from any direction"}},
                 {"5", {"Strahlwasser (Düse 6,3 mm)", "water jets (6.3 mm nozzle)"}},
                 {"6", {"starkes Strahlwasser (Düse 12,5 mm)", "powerful water jets (12.5 mm nozzle)"}},
                 {"7", {"zeitweiliges Untertauchen (1 m, 30 min)", "temporary immersion (1 m, 30 min)"}},
                 {"8", {"dauerndes Untertauchen (nach Vereinbarung)", "continuous immersion (as agreed)"}},
                 {"9", {"Hochdruck-/Dampfstrahl (80 °C)", "high-pressure, high-temperature jets (80 °C)"}}});
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

Table networkSystems() {
    return make("network_systems", {"Netzsysteme", "Earthing systems"}, {"TN-C, TN-S, TN-C-S, TT, IT", "TN-C, TN-S, TN-C-S, TT, IT"},
                {"IEC 60364-1 / DIN VDE 0100-100 / NIN 3.1.2. 1. Buchstabe: Erdung der Stromquelle, 2. Buchstabe: Erdung der Körper.",
                 "IEC 60364-1. 1st letter: source earthing, 2nd letter: exposed-conductive-part earthing."},
                {{"System", "System"}, {"Aufbau", "Arrangement"}, {"Hinweis", "Note"}},
                {{"TN-C", {"N und PE als PEN kombiniert", "N and PE combined as PEN"}, {"PEN ≥ 10 mm² Cu; EMV-ungünstig", "PEN ≥ 10 mm² Cu; poor EMC"}},
                 {"TN-S", {"N und PE getrennt im ganzen Netz", "separate N and PE throughout"}, {"EMV-günstig, Standard für Neuanlagen", "good EMC, standard for new installations"}},
                 {"TN-C-S", {"PEN-Aufteilung an einem Punkt, danach getrennt", "PEN split at one point, separate afterwards"},
                  {"nach Aufteilung nie wieder verbinden", "never reconnect after the split"}},
                 {"TT", {"Körper über eigenen Anlagenerder", "exposed parts on local earth electrode"}, {"Fehlerschutz meist mit RCD", "fault protection usually by RCD"}},
                 {"IT", {"Quelle isoliert oder hochohmig geerdet", "source isolated or high-impedance earthed"},
                  {"Isolationsüberwachung, Betrieb bei 1. Fehler (Medizin, Industrie)", "insulation monitoring, operates after 1st fault (medical, industry)"}}});
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

Table conductorColours() {
    return make("conductor_colours", {"Aderfarben", "Conductor colours"}, {"Netz und Maschinen (Empfehlung)", "Mains and machines (recommendation)"},
                {"Kennzeichnung nach IEC 60445 (DIN EN 60445 / VDE 0197); Maschinen nach IEC 60204-1 Abschn. 13.2 (Empfehlung).",
                 "Identification per IEC 60445; machines per IEC 60204-1 clause 13.2 (recommendation)."},
                {{"Leiter / Kreis", "Conductor / circuit"}, {"Farbe", "Colour"}},
                {{{"Außenleiter L1", "Line conductor L1"}, {"braun", "brown"}},
                 {{"Außenleiter L2", "Line conductor L2"}, {"schwarz", "black"}},
                 {{"Außenleiter L3", "Line conductor L3"}, {"grau", "grey"}},
                 {{"Neutralleiter N", "Neutral N"}, {"blau", "blue"}},
                 {{"Schutzleiter PE", "Protective conductor PE"}, {"grün-gelb", "green-yellow"}},
                 {{"PEN-Leiter", "PEN conductor"}, {"grün-gelb + blaue Markierung an den Enden", "green-yellow + blue marking at ends"}},
                 {{"Maschine: Hauptstromkreise AC/DC", "Machine: AC/DC power circuits"}, {"schwarz", "black"}},
                 {{"Maschine: AC-Steuerstromkreise", "Machine: AC control circuits"}, {"rot", "red"}},
                 {{"Maschine: DC-Steuerstromkreise", "Machine: DC control circuits"}, {"blau", "blue"}},
                 {{"Maschine: Fremdspannung (bleibt bei Hauptschalter AUS)", "Machine: external supply (live with main switch OFF)"},
                  {"orange", "orange"}}});
}

Table mcbCharacteristics() {
    return make("mcb_characteristics", {"LS-Schalter-Charakteristiken", "MCB tripping curves"}, {"B, C, D, K, Z – Sofortauslösung", "B, C, D, K, Z – instantaneous trip"},
                {"B/C/D nach IEC 60898-1: Nichtauslösestrom 1,13 · I_n, Auslösestrom 1,45 · I_n (< 1 h). K/Z nach IEC 60947-2 bzw. Herstellerangaben.",
                 "B/C/D per IEC 60898-1: non-tripping 1.13 · I_n, tripping 1.45 · I_n (< 1 h). K/Z per IEC 60947-2 or manufacturer data."},
                {{"Typ", "Type"}, {"Sofortauslösung", "Instantaneous trip"}, {"Einsatz", "Application"}},
                {{"B", "3 … 5 · I_n", {"Leitungsschutz, ohmsche Lasten, Wohnbau", "cable protection, resistive loads, residential"}},
                 {"C", "5 … 10 · I_n", {"Motoren, Leuchten, Trafos, Industrie", "motors, luminaires, transformers, industry"}},
                 {"D", "10 … 20 · I_n", {"hohe Einschaltströme (Trafos, Ventile)", "high inrush (transformers, valves)"}},
                 {"K", "8 … 14 · I_n", {"Motoren/Trafos, enge thermische Auslösung", "motors/transformers, tight thermal trip"}},
                 {"Z", "2 … 3 · I_n", {"Halbleiter, Messkreise", "semiconductors, measuring circuits"}}});
}

Table fuseColours() {
    return make("fuse_colours", {"D-Sicherungen: Kennmelder", "D-type fuses: indicator colours"}, {"DIAZED/NEOZED Farbcode", "DIAZED/NEOZED colour code"},
                {"Kennmelderfarben nach DIN VDE 0636 / IEC 60269-3.", "Indicator colours per IEC 60269-3 (DIAZED/NEOZED)."},
                {{"I_n", "I_n"}, {"Farbe", "Colour"}},
                {{"2 A", {"rosa", "pink"}},   {"4 A", {"braun", "brown"}},    {"6 A", {"grün", "green"}},
                 {"10 A", {"rot", "red"}},    {"13 A", {"schwarz", "black"}}, {"16 A", {"grau", "grey"}},
                 {"20 A", {"blau", "blue"}},  {"25 A", {"gelb", "yellow"}},   {"35 A", {"schwarz", "black"}},
                 {"50 A", {"weiß", "white"}}, {"63 A", {"kupfer", "copper"}}, {"80 A", {"silber", "silver"}},
                 {"100 A", {"rot", "red"}}});
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
                {"Typische Grenzwerte ohne Repeater laut jeweiliger Spezifikation – Herstellerangaben und Kabeltyp beachten.",
                 "Typical limits without repeaters per the respective specification – check manufacturer data and cable type."},
                {{"Bus", "Bus"}, {"Physik", "Physical"}, {"Max. Länge", "Max. length"}, {"Teilnehmer", "Nodes"}, {"Datenrate", "Data rate"}, {"Hinweis", "Note"}},
                {{"BACnet MS/TP", "RS-485", {"1200 m je Segment", "1200 m per segment"}, {"32 Unit Loads/Segment, MAC 0–127 (Master)", "32 unit loads/segment, MAC 0–127 (master)"},
                  "9,6–115,2 kbit/s", {"120 Ω beidseitig, Bias einmal", "120 Ω both ends, bias once"}},
                 {"Modbus RTU", "RS-485", {"ca. 1000–1200 m (baudratenabh.)", "approx. 1000–1200 m (baud-dependent)"}, {"32/Segment, Adressen 1–247", "32/segment, addresses 1–247"},
                  "9,6–115,2 kbit/s", {"120 Ω beidseitig; A/B-Bezeichnung prüfen", "120 Ω both ends; check A/B naming"}},
                 {"KNX TP1", {"2-Draht, freie Topologie", "2-wire, free topology"}, {"Linie 1000 m; 350 m Netzteil–TN; 700 m TN–TN", "line 1000 m; 350 m PSU–device; 700 m device–device"},
                  {"64 je Liniensegment", "64 per line segment"}, "9,6 kbit/s", {"SELV 30 V DC, kein Abschluss", "SELV 30 V DC, no termination"}},
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

Table motorTerminals() {
    return make("motor_terminals", {"Motor-Klemmbrett: Stern / Dreieck", "Motor terminal board: star / delta"},
                {"Brücken, Spannungen, Drehrichtung", "Links, voltages, rotation"},
                {"Klemmenbezeichnung nach IEC 60034-8. Klemmbrett oben: W2 U2 V2, unten: U1 V1 W1.",
                 "Terminal marking per IEC 60034-8. Upper row: W2 U2 V2, lower row: U1 V1 W1."},
                {{"Thema", "Topic"}, {"Ausführung", "Arrangement"}},
                {{{"Stern (Y)", "Star (Y)"}, {"eine waagerechte Brücke W2–U2–V2, Netz an U1/V1/W1", "one horizontal link W2–U2–V2, supply to U1/V1/W1"}},
                 {{"Dreieck (Δ)", "Delta (Δ)"}, {"drei senkrechte Brücken U1–W2, V1–U2, W1–V2", "three vertical links U1–W2, V1–U2, W1–V2"}},
                 {{"Motor 230/400 V", "Motor 230/400 V"}, {"an 400-V-Netz in Stern", "on 400 V mains in star"}},
                 {{"Motor 400/690 V", "Motor 400/690 V"}, {"an 400-V-Netz in Dreieck (Y-Δ-Anlauf möglich)", "on 400 V mains in delta (Y-Δ start possible)"}},
                 {{"Rechtslauf", "Clockwise"}, {"L1→U1, L2→V1, L3→W1 (Blick auf Wellenende)", "L1→U1, L2→V1, L3→W1 (facing shaft end)"}},
                 {{"Drehrichtungsumkehr", "Reverse rotation"}, {"zwei Außenleiter tauschen", "swap two line conductors"}}});
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

Table resistorColours() {
    return make("resistor_colours", {"Farbcode-Tabelle", "Colour code table"}, {"Ziffern, Multiplikator, Toleranz", "digits, multiplier, tolerance"},
                {"IEC 60062.", "IEC 60062."}, {{"Farbe", "Colour"}, {"Ziffer", "Digit"}, {"Multiplikator", "Multiplier"}, {"Toleranz", "Tolerance"}},
                {{{"schwarz", "black"}, "0", "×1", "–"},
                 {{"braun", "brown"}, "1", "×10", "±1 %"},
                 {{"rot", "red"}, "2", "×100", "±2 %"},
                 {{"orange", "orange"}, "3", "×1 k", "–"},
                 {{"gelb", "yellow"}, "4", "×10 k", "–"},
                 {{"grün", "green"}, "5", "×100 k", {"±0,5 %", "±0.5 %"}},
                 {{"blau", "blue"}, "6", "×1 M", {"±0,25 %", "±0.25 %"}},
                 {{"violett", "violet"}, "7", "×10 M", {"±0,1 %", "±0.1 %"}},
                 {{"grau", "grey"}, "8", "×100 M", {"±0,05 %", "±0.05 %"}},
                 {{"weiß", "white"}, "9", "×1 G", "–"},
                 {"Gold", "–", {"×0,1", "×0.1"}, "±5 %"},
                 {{"Silber", "Silver"}, "–", {"×0,01", "×0.01"}, "±10 %"},
                 {{"keine", "none"}, "–", "–", "±20 %"}});
}

}  // namespace

void addTables(std::vector<Table>& out) {
    out.push_back(ipFirst());
    out.push_back(ipSecond());
    out.push_back(protectionClasses());
    out.push_back(networkSystems());
    out.push_back(conductorColours());
    out.push_back(voltageDropLimits());
    out.push_back(ampacityTable(cables::Insulation::PVC, 2));
    out.push_back(ampacityTable(cables::Insulation::PVC, 3));
    out.push_back(ampacityTable(cables::Insulation::XLPE, 2));
    out.push_back(ampacityTable(cables::Insulation::XLPE, 3));
    out.push_back(temperatureFactors());
    out.push_back(groupingFactors());
    out.push_back(mcbCharacteristics());
    out.push_back(fuseColours());
    out.push_back(cableTypes());
    out.push_back(fieldbuses());
    out.push_back(insulationClasses());
    out.push_back(motorDuty());
    out.push_back(motorIe());
    out.push_back(motorTerminals());
    out.push_back(motorMounting());
    out.push_back(pinRj45());
    out.push_back(pinRs485());
    out.push_back(pinM12());
    out.push_back(pinKnx());
    out.push_back(pinRs232());
    out.push_back(pinUsb());
    out.push_back(pinHdmi());
    out.push_back(resistorColours());
    out.push_back(siPrefixes());
}

}  // namespace elc::catalog
