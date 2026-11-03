// Pictures of the area "Überspannungs- & Blitzschutz" (v1.11), drawn with catalog/sketch.hpp. Layout after the DEHN
// white papers WPX035 (Ex areas, Bild 1/4/5) and WPX050 (data centres, Bild 11/12/15) and the DEHN lightning protection
// guide; all pictures are schematic. Labels use the ink colour, zone boundaries coloured outlines, so the pictures read in
// light and dark mode.
#include "catalog/surge_drawings.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <vector>

#include "catalog/sketch.hpp"
#include "elc/types.hpp"

namespace elc::catalog::surgepics {

namespace {

constexpr const char* kLpz1 = "line:#FB8C00";  // boundary LPZ 1 (building)
constexpr const char* kLpz2 = "line:#43A047";  // boundary LPZ 2 (room, cabinet)
constexpr const char* kLpz3 = "line:#1E88E5";  // boundary LPZ 3 (device)
constexpr const char* kN = "line:#1E88E5";     // neutral
constexpr const char* kPE = "line:#43A047";    // PE, PEN, equipotential bonding
constexpr const char* kData = "line:#8E24AA";  // data / I&C line
constexpr const char* kBolt = "line:#E53935";  // lightning, lightning current

Text cat(std::initializer_list<Text> parts) {
    Text t;
    for (const auto& p : parts) {
        t.de += p.de;
        t.en += p.en;
    }
    return t;
}

/// Number of characters of a UTF-8 string (for the width of a tag)
std::size_t glyphs(const std::string& s) {
    return static_cast<std::size_t>(std::count_if(s.begin(), s.end(), [](char c) { return (static_cast<unsigned char>(c) & 0xC0) != 0x80; }));
}

void dot(Sketch& s, double x, double y) { s.circle("inkfill", x, y, 0.9, 12); }

/// Box with a centred label (device, panel)
void box(Sketch& s, double x0, double y0, double x1, double y1, Text t, const char* style = "c") {
    s.rect("bgfill", x0, y0, x1, y1);
    s.label((x0 + x1) / 2.0, (y0 + y1) / 2.0, std::move(t), style);
}

/// Surge protective device at a zone boundary: small box centred on (x, y)
void spd(Sketch& s, double x, double y, Text t) { box(s, x - 3, y - 2.5, x + 3, y + 2.5, std::move(t)); }

/// Zone name on a background tag
void tag(Sketch& s, double x, double y, Text t) {
    const double w = 1.75 * static_cast<double>(glyphs(t.de)) + 2.0;
    s.rect("bgfill", x - w / 2.0, y - 2.2, x + w / 2.0, y + 2.2);
    s.label(x, y, std::move(t), "cb");
}

/// Lightning flash ending at (x, y)
void bolt(Sketch& s, double x, double y) { s.line(kBolt, {{x + 3.5, y + 8}, {x - 0.5, y + 4.2}, {x + 2.0, y + 3.6}, {x, y}}); }

/// Varistor (resistor body with the bent diagonal) between yTop and yBot
void varistor(Sketch& s, double x, double yTop, double yBot) {
    s.rect("bgfill", x - 1.8, yBot, x + 1.8, yTop);
    s.line("ink", {{x - 3.6, yBot - 0.6}, {x - 2.5, yBot - 0.6}, {x + 2.5, yTop + 0.6}});
}

/// Spark gap (two electrodes tip to tip) between yTop and yBot
void sparkGap(Sketch& s, double x, double yTop, double yBot) {
    const double ym = (yTop + yBot) / 2.0;
    s.rect("bgfill", x - 1.8, yBot, x + 1.8, yTop);
    s.closed("inkfill", {{x - 1.3, yTop - 0.8}, {x + 1.3, yTop - 0.8}, {x, ym + 0.6}});
    s.closed("inkfill", {{x - 1.3, yBot + 0.8}, {x + 1.3, yBot + 0.8}, {x, ym - 0.6}});
}

struct SchemeSpec {
    std::vector<std::string> phases;
    bool neutral = false;
    bool lToN = false;  // 3+1 / 1+1: varistors L–N, spark gap N–PE
    std::string earth;
    std::string title;
};

SchemeSpec specOf(const std::string& net, bool three) {
    if (net == "tnc") return {{"L1", "L2", "L3"}, false, false, "PEN", "TN-C · 3+0"};
    if (net == "it") return three ? SchemeSpec{{"L1", "L2", "L3"}, false, false, "PE", "IT · 3+0"} : SchemeSpec{{"L1", "L2"}, false, false, "PE", "IT · 2+0"};
    if (net == "tt") return three ? SchemeSpec{{"L1", "L2", "L3"}, true, true, "PE", "TT · 3+1"} : SchemeSpec{{"L"}, true, true, "PE", "TT · 1+1"};
    return three ? SchemeSpec{{"L1", "L2", "L3"}, true, false, "PE", "TN-S · 4+0"} : SchemeSpec{{"L"}, true, false, "PE", "TN-S · 2+0"};
}

/// Connection scheme between x0 and x0 + w (y 6 … 62)
void drawScheme(Sketch& s, double x0, double w, const SchemeSpec& sp, const Text& title) {
    const double xEnd = x0 + w;
    const double yN = 36, yE = 10;
    const std::array<double, 3> yL{54, 50, 46};
    const std::size_t n = std::min<std::size_t>(sp.phases.size(), 3);
    for (std::size_t i = 0; i < n; ++i) {
        s.line("ink", {{x0 + 7, yL[i]}, {xEnd, yL[i]}});
        s.label(x0 + 0.5, yL[i], sp.phases[i], "l");
    }
    if (sp.neutral) {
        s.line(kN, {{x0 + 7, yN}, {xEnd, yN}});
        s.label(x0 + 0.5, yN, "N", "l");
    }
    s.line(kPE, {{x0 + 7, yE}, {xEnd, yE}});
    s.label(x0 + 0.5, yE, sp.earth, "l");
    const double paths = static_cast<double>(n + (sp.neutral ? 1 : 0));
    const double dx = std::min(9.0, (w - 16.0) / paths);
    double x = x0 + 14;
    for (std::size_t i = 0; i < n; ++i) {
        dot(s, x, yL[i]);
        if (sp.lToN) {
            s.line("ink", {{x, yL[i]}, {x, 44}});
            varistor(s, x, 44, 38);
            s.line("ink", {{x, 38}, {x, yN}});
            dot(s, x, yN);
        } else {
            s.line("ink", {{x, yL[i]}, {x, 30}});
            varistor(s, x, 30, 20);
            s.line("ink", {{x, 20}, {x, yE}});
            dot(s, x, yE);
        }
        x += dx;
    }
    if (sp.neutral) {
        dot(s, x, yN);
        s.line(kN, {{x, yN}, {x, 30}});
        if (sp.lToN) sparkGap(s, x, 30, 20);
        else varistor(s, x, 30, 20);
        s.line(kPE, {{x, 20}, {x, yE}});
        dot(s, x, yE);
    }
    s.label(x0 + w / 2.0, 60, title, "cb");
}

const Text kSchemeNote{"Schaltbild schematisch (DIN VDE 0100-534 / IEC 60364-5-53): Rechteck mit Schrägstrich = Varistor, Dreiecke = "
                       "Funkenstrecke (Summenfunkenstrecke N–PE bei 3+1). Anschlussleitungen a + b ≤ 0,5 m (V-Verdrahtung).",
                       "Schematic (IEC 60364-5-53): rectangle with diagonal = varistor, triangles = spark gap (N–PE spark gap with 3+1). "
                       "Connecting leads a + b ≤ 0.5 m (V-wiring)."};

}  // namespace

Chart zones() {
    Sketch s;
    s.frame(100, 66, 0.66);
    s.line("ink", {{0, 6}, {100, 6}});
    // protective volume of the air-termination: above/outside LPZ 0A, inside LPZ 0B
    s.line("sig", {{26, 58}, {74, 58}, {100, 10}});
    s.rect(kLpz1, 24, 6, 76, 50);
    s.line("ink", {{26, 50}, {26, 58}});
    s.line("ink", {{74, 50}, {74, 58}});
    bolt(s, 26, 58);
    s.rect(kLpz2, 44, 10, 72, 41);
    s.rect(kLpz3, 58, 13, 74, 29);
    // power line with type 1 / 2 / 3 at the boundaries
    s.line("ink", {{0, 19}, {58, 19}});
    s.label(1, 21.8, "230/400 V", "l");
    spd(s, 24, 19, "T1");
    spd(s, 44, 19, "T2");
    spd(s, 58, 19, "T3");
    // data line with D1 / C2
    s.line(kData, {{0, 33}, {70, 33}, {70, 29}});
    s.label(1, 35.8, {"Daten", "Data"}, "l");
    spd(s, 24, 33, "D1");
    spd(s, 44, 33, "C2");
    tag(s, 12, 60, "LPZ 0A");
    tag(s, 84, 20, "LPZ 0B");
    tag(s, 33, 44, "LPZ 1");
    tag(s, 58, 37.5, "LPZ 2");
    tag(s, 66.5, 25, "LPZ 3");
    s.ch.note = {"Blitzschutzzonen nach IEC 62305-4, schematisch (nach DEHN WPX035 Bild 1 / WPX050): orange = Gebäude LPZ 1, grün = "
                 "Raum/Schrank LPZ 2, blau = Gerät LPZ 3; gestrichelt = Schutzbereich der Fangeinrichtung (außen LPZ 0A, innen LPZ 0B). "
                 "T1/T2/T3 = SPD Typ 1/2/3 der Energietechnik, D1/C2 = Ableiter der Informationstechnik an der Zonengrenze.",
                 "Lightning protection zones per IEC 62305-4, schematic (after DEHN WPX035 / WPX050): orange = building LPZ 1, green = "
                 "room/cabinet LPZ 2, blue = device LPZ 3; dashed = protected volume of the air-termination (outside LPZ 0A, inside "
                 "LPZ 0B). T1/T2/T3 = SPD type 1/2/3 for power, D1/C2 = arresters for data lines at the zone boundary."};
    return s.ch;
}

Chart coordination() {
    Sketch s;
    s.frame(100, 44, 0.44);
    s.label(50, 41, {"Energetisch koordinierte Schutzkette", "Energy-coordinated protection chain"}, "cb");
    box(s, 0.5, 20, 12, 34, {"Netz", "Grid"}, "cb");
    struct Stage {
        double x0;
        Text name;
        Text type;
        Text zone;
        Text test;
    };
    const std::array<Stage, 3> st{Stage{20, "HV", {"Typ 1+2", "Type 1+2"}, "0A → 1", "10/350 µs"},
                                  Stage{50, "UV", {"Typ 2", "Type 2"}, "1 → 2", "8/20 µs"},
                                  Stage{80, {"Gerät", "Device"}, {"Typ 3", "Type 3"}, "2 → 3", {"Kombiwelle", "comb. wave"}}};
    for (const auto& g : st) {
        s.rect("bgfill", g.x0, 20, g.x0 + 18, 34);
        s.label(g.x0 + 9, 30, g.name, "cb");
        s.label(g.x0 + 9, 24, g.type, "c");
        s.label(g.x0 + 9, 16, g.zone, "c");
        s.label(g.x0 + 9, 10, g.test, "c");
    }
    s.line("ink", {{12, 27}, {20, 27}});
    s.line("ink", {{38, 27}, {50, 27}});
    s.line("ink", {{68, 27}, {80, 27}});
    s.label(44, 30.5, "> 10 m", "c");
    s.label(74, 30.5, "> 10 m", "c");
    s.label(4, 4, {"Zonenübergang und Prüfstrom", "Zone transition and test current"}, "l");
    s.ch.note = {"HV = Hauptverteilung (Einspeisung), UV = Unterverteilung. Weitere Stufe (Typ 2 bzw. Typ 3), wenn die Leitung zum "
                 "vorgeschalteten SPD länger als 10 m ist; die Typen müssen energetisch koordiniert sein (DEHN WPX050 Bild 12, "
                 "IEC 62305-4).",
                 "HV = main distribution (origin), UV = sub-distribution. Next stage (type 2 or type 3) if the cable to the upstream "
                 "SPD is longer than 10 m; the types must be energy-coordinated (DEHN WPX050 Bild 12, IEC 62305-4)."};
    return s.ch;
}

Chart schemes() {
    Sketch s;
    s.frame(100, 64, 0.64);
    const auto tns = specOf("tns", true);
    const auto tt = specOf("tt", true);
    drawScheme(s, 1, 47, tns, tns.title);
    drawScheme(s, 52, 47, tt, tt.title);
    s.ch.note = kSchemeNote;
    return s.ch;
}

Chart scheme(const std::string& net, bool three, const Text& type) {
    Sketch s;
    s.frame(100, 64, 0.64);
    const auto sp = specOf(net, three);
    drawScheme(s, 18, 64, sp, cat({sp.title, " · ", type}));
    s.label(50, 3, {"Anschluss a + b ≤ 0,5 m zum PE/PA", "Leads a + b ≤ 0.5 m to PE/bonding"}, "c");
    s.ch.note = kSchemeNote;
    return s.ch;
}

Chart categories() {
    Sketch s;
    s.frame(100, 56, 0.56);
    struct Col {
        const char* cat;
        double uw;
        const char* colour;
        Text kv;
        Text what;
    };
    const std::array<Col, 4> cols{Col{"IV", 6.0, "fill:#EF6C00", "6 kV", {"Zähler", "Meter"}},
                                  Col{"III", 4.0, "fill:#F9A825", "4 kV", {"Verteiler", "Boards"}},
                                  Col{"II", 2.5, "fill:#7CB342", "2,5 kV", {"Geräte", "Appliances"}},
                                  Col{"I", 1.5, "fill:#1E88E5", "1,5 kV", {"Elektronik", "Electronics"}}};
    const double base = 12, k = 6;  // 6 units per kV
    double x = 6;
    for (const auto& c : cols) {
        s.rect(c.colour, x, base, x + 16, base + k * c.uw);
        s.label(x + 8, base + k * c.uw + 2.6, c.kv, "cb");
        s.label(x + 8, 8, c.cat, "cb");
        s.label(x + 8, 3, c.what, "c");
        x += 24;
    }
    s.line("ink", {{2, base}, {98, base}});
    s.label(64, 53, {"Einspeisung → Endgerät", "Origin → equipment"}, "c");
    s.ch.note = {"Bemessungs-Stoßspannung U_W je Überspannungskategorie für 230/400 V (IEC 60664-1, DIN VDE 0100-443). Der wirksame "
                 "Schutzpegel U_p,f des SPD muss unter U_W der zu schützenden Betriebsmittel liegen.",
                 "Rated impulse withstand voltage U_W per overvoltage category for 230/400 V (IEC 60664-1, IEC 60364-4-44). The "
                 "effective protection level U_p/f of the SPD must stay below U_W of the equipment."};
    return s.ch;
}

Chart datacenter() {
    Sketch s;
    s.frame(100, 66, 0.66);
    s.line("ink", {{0, 8}, {100, 8}});
    s.line(kPE, {{4, 5}, {82, 5}});
    s.label(43, 2.2, {"Ringerder / Fundamenterder", "Ring / foundation earth"}, "c");
    s.rect(kLpz1, 6, 8, 80, 46);
    s.line("ink", {{6, 24}, {80, 24}});
    s.line("ink", {{30, 8}, {30, 46}});
    s.line("ink", {{56, 8}, {56, 46}});
    // server room LPZ 2 with racks
    s.rect(kLpz2, 31.5, 25.5, 54.5, 44.5);
    s.rect("bgfill", 34, 27.5, 38, 38);
    s.rect("bgfill", 48, 27.5, 52, 38);
    // roof: PV generator, HVAC unit, air-termination rods
    s.closed("fill:#5C6BC0", {{10, 46}, {26, 46}, {29, 52}, {13, 52}});
    s.rect("bgfill", 60, 46, 74, 53);
    s.circle("ink", 67, 49.5, 2.2, 16);
    s.line("ink", {{8, 46}, {8, 56}});
    s.line("ink", {{78, 46}, {78, 55}});
    bolt(s, 78, 55);
    // supply cable and camera on the outer wall
    s.line("ink", {{100, 14}, {80, 14}});
    s.label(91, 16.8, {"Netz", "Grid"}, "c");
    s.rect("bgfill", 1.5, 36, 6, 39.5);
    // rooms
    s.label(18, 20, "Generator", "c");
    s.label(43, 20, {"USV", "UPS"}, "c");
    s.label(68, 20, {"NSHV", "LV main"}, "c");
    s.label(18, 42, {"Büros", "Offices"}, "c");
    s.label(43, 41.5, "Server", "c");
    s.label(68, 42, {"BMA", "Fire alarm"}, "c");
    s.ch.markers = {{68, 14, "1", "state"}, {18, 14, "2", "state"}, {43, 32, "3", "state"}, {68, 32, "4", "state"},
                    {18, 32, "5", "state"}, {20, 57, "6", "state"}, {67, 58, "7", "state"}};
    tag(s, 40, 60, "LPZ 0A");
    s.ch.note = {"Schutzvorschlag Rechenzentrum, schematisch nach DEHN WPX050 Bild 11: Nummern = Zeilen der Tabelle (1 NSHV, "
                 "2 Notstrom, 3 Serverraum, 4 Brandmeldeanlage, 5 Büros/Sicherheit/IP-Kameras, 6 PV-Anlage, 7 Dachaufbauten). "
                 "Orange = Gebäude LPZ 1, grün = Serverraum LPZ 2.",
                 "Data centre protection proposal, schematic after DEHN WPX050 Bild 11: numbers = table rows (1 LV main, 2 standby "
                 "power, 3 server room, 4 fire alarm, 5 offices/security/IP cameras, 6 PV system, 7 roof units). Orange = building "
                 "LPZ 1, green = server room LPZ 2."};
    return s.ch;
}

Chart exCircuit(double len, bool lineOk, const Text& zone) {
    Sketch s;
    s.frame(100, 62, 0.62);
    s.line("ink", {{0, 12}, {100, 12}});
    // control room (LPZ 1) with cabinet (LPZ 2): PLC – isolating amplifier – SPD at the zone boundary
    s.rect(kLpz1, 2, 12, 42, 48);
    s.label(20, 44.5, {"Messwarte", "Control room"}, "cb");
    s.label(36, 41, "LPZ 1", "c");
    s.rect(kLpz2, 4, 16, 30, 38);
    s.label(17, 35, "LPZ 2", "c");
    box(s, 6, 21, 14, 31, {"SPS", "PLC"});
    box(s, 17, 21, 27, 31, "Ex i");
    s.line("ink", {{14, 26}, {17, 26}});
    s.line("ink", {{27, 26}, {39, 26}});
    spd(s, 42, 26, "SPD");
    // buried field cable to the tank
    s.line("ink", {{45, 26}, {47, 26}, {47, 8}, {64, 8}, {64, 41}, {66, 41}});
    // hazardous area around the tank, tank (zone 0 inside), transmitter with SPD
    s.closed("sig", {{62, 12}, {62, 52}, {99, 52}, {99, 12}});
    s.rect("bgfill", 70, 12, 96, 36);
    s.label(83, 24, {"Tank (Zone 0)", "Tank (zone 0)"}, "c");
    box(s, 78, 36, 88, 42, "TT");
    spd(s, 69, 41, "SPD");
    s.line(lineOk ? "ink" : kBolt, {{72, 41}, {78, 41}});
    s.label(75, 45.5, cat({formatNumber(len, len < 10 ? 1 : 0), " m"}), "c");
    s.label(97.5, 49.2, zone, "r");
    // air-termination mast outside the Ex zone
    s.line("ink", {{58, 12}, {58, 52}});
    bolt(s, 58, 52);
    tag(s, 49, 50, "LPZ 0B");
    // equipotential bonding conductor parallel to the cable
    s.line(kPE, {{42, 23.5}, {42, 4.5}, {69, 4.5}, {69, 38.5}});
    s.label(55.5, 1.8, {"PA ≥ 4 mm² Cu", "Bonding ≥ 4 mm² Cu"}, "c");
    s.ch.note = {"Eigensicherer Messkreis nach DEHN WPX035 Bild 4/5, schematisch: SPD am Übergang LPZ 0B → 1 am Feldgerät (Tank) und am "
                 "Eintritt in die Messwarte, danach Trennverstärker (Ex i) und SPS in LPZ 2. Leitung SPD → Messumformer (TT) höchstens "
                 "1 m geschirmt oder im beidseitig geerdeten Metallrohr (rot = nicht eingehalten); Fangeinrichtung außerhalb der Ex-Zone.",
                 "Intrinsically safe loop after DEHN WPX035 Bild 4/5, schematic: SPD at the transition LPZ 0B → 1 at the field device "
                 "(tank) and at the entry to the control room, then isolating amplifier (Ex i) and PLC in LPZ 2. Line SPD → transmitter "
                 "(TT) at most 1 m shielded or in a metal conduit earthed at both ends (red = not met); air-termination outside the Ex zone."};
    return s.ch;
}

Chart signalRoute(bool outside, const Text& arrester) {
    Sketch s;
    s.frame(100, 44, 0.44);
    s.line("ink", {{0, 6}, {100, 6}});
    s.rect(kLpz2, 64, 10, 96, 32);
    box(s, 76, 15, 92, 25, {"DDC/SPS", "DDC/PLC"});
    s.label(80, 28.6, "LPZ 2", "cb");
    if (outside) {
        s.rect(kLpz1, 44, 6, 98, 40);
        box(s, 2, 14, 15, 24, "Sensor");
        s.line("ink", {{15, 19}, {16, 19}});
        spd(s, 19, 19, "SPD");
        s.line("ink", {{22, 19}, {41, 19}});
        spd(s, 44, 19, "SPD");
        s.line("ink", {{47, 19}, {76, 19}});
        s.line(kPE, {{19, 16.5}, {19, 6}});
        s.line(kPE, {{44, 16.5}, {44, 6}});
        s.label(31.5, 22.6, arrester, "c");
        tag(s, 22, 34, "LPZ 0A");
        tag(s, 54, 35, "LPZ 1");
    } else {
        s.rect(kLpz1, 2, 6, 98, 40);
        box(s, 6, 14, 19, 24, "Sensor");
        s.line("ink", {{19, 19}, {61, 19}});
        spd(s, 64, 19, "SPD");
        s.line("ink", {{67, 19}, {76, 19}});
        s.line(kPE, {{64, 16.5}, {64, 10}});
        s.label(40, 22.6, arrester, "c");
        tag(s, 20, 34, "LPZ 1");
    }
    s.ch.note = {"Ableiter an jeder Zonengrenze, die die Leitung kreuzt; Erdung kurz an den Potentialausgleich (grün). Leitung ins Freie: "
                 "beidseitig beschalten (Gebäudeeintritt und Feldgerät), Kategorie D1 für Blitzteilströme.",
                 "Arrester at every zone boundary the line crosses; short earthing to the equipotential bonding (green). Line to the "
                 "outside: protect both ends (building entry and field device), category D1 for partial lightning currents."};
    return s.ch;
}

Chart loop(double M, double didt, double U) {
    Sketch s;
    s.frame(100, 50, 0.5);
    s.line(kBolt, {{80, 4}, {80, 48}});
    s.label(83, 40, "Δi/Δt", "l");
    s.label(83, 35, cat({formatNumber(didt, 0), " kA/µs"}), "l");
    s.line("ink", {{30, 27}, {30, 40}, {62, 40}, {62, 10}, {30, 10}, {30, 23}});
    s.circle("bgfill", 30, 27, 0.9, 12);
    s.circle("bgfill", 30, 23, 0.9, 12);
    s.label(28, 25, cat({"U = ", formatNumber(U, 0), " kV"}), "rb");
    s.label(46, 25, cat({"M = ", formatNumber(M, 2), " µH"}), "c");
    s.line("ink", {{30, 5}, {62, 5}});
    s.line("ink", {{30, 3.8}, {30, 6.2}});
    s.line("ink", {{62, 3.8}, {62, 6.2}});
    s.label(46, 7.3, "a", "c");
    s.line("ink", {{62, 45}, {80, 45}});
    s.line("ink", {{62, 43.8}, {62, 46.2}});
    s.line("ink", {{80, 43.8}, {80, 46.2}});
    s.label(71, 47.6, "s", "c");
    s.ch.note = {"Installationsschleife a × a im Abstand s zur blitzstromführenden Ableitung (rot), schematisch nach DEHN WPX050 Bild 15. "
                 "Die Spannung U steht an der offenen Stelle der Schleife (mögliche Überschlagstrecke).",
                 "Installation loop a × a at distance s from the down conductor carrying the lightning current (red), schematic after "
                 "DEHN WPX050 Bild 15. The voltage U appears across the open point of the loop (possible sparkover gap)."};
    return s.ch;
}

Chart distance(double upf, double uw, double l, double lpo) {
    Sketch s;
    s.frame(100, 36, 0.36);
    const bool ok = lpo < 0 || l <= lpo;
    box(s, 3, 12, 17, 24, "SPD");
    box(s, 81, 12, 97, 24, {"Gerät", "Device"});
    s.line(ok ? kPE : kBolt, {{17, 18}, {81, 18}});
    s.label(49, 21.6, cat({"l = ", formatNumber(l, 1), " m"}), "c");
    s.label(49, 14.4, lpo < 0 ? Text{"l_po unbegrenzt", "l_po unlimited"} : cat({"l_po = ", formatNumber(lpo, 1), " m"}), "c");
    s.label(2, 29, cat({"U_p,f = ", formatNumber(upf, 2), " kV"}), "l");
    s.label(98, 29, cat({"U_W = ", formatNumber(uw, 1), " kV"}), "r");
    s.label(50, 5, ok ? Text{"im Schutzbereich", "within the protective distance"} : Text{"außerhalb des Schutzbereichs", "beyond the protective distance"}, "cb");
    s.ch.note = {"Grün: Gerät im Schutzbereich des SPD, rot: weiteres SPD (Typ 2/3) nahe am Gerät nötig. l_po = (U_W − U_p,f) / 25 V/m "
                 "(OVE E 09).",
                 "Green: equipment within the protective distance of the SPD, red: add an SPD (type 2/3) close to the equipment. "
                 "l_po = (U_W − U_p/f) / 25 V/m (OVE E 09)."};
    return s.ch;
}

}  // namespace elc::catalog::surgepics
