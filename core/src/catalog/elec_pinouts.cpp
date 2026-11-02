// Pin assignments (v1.9, tab "div. Pinbelegungen" of the app "Elektroberechnungen"): connectors for network, audio/video,
// computer, embedded and vehicle. Every table names its standard; check against the device documentation.
#include <array>
#include <string>
#include <vector>

#include <cctype>

#include "catalog/helpers.hpp"

namespace elc::catalog {

namespace {

using Row = std::vector<Text>;

/// "0,7 V" → "0.7 V" in the English text of cells that use one string for both languages
std::string decimalPoint(std::string s) {
    for (std::size_t i = 1; i + 1 < s.size(); ++i) {
        if (s[i] == ',' && std::isdigit(static_cast<unsigned char>(s[i - 1])) && std::isdigit(static_cast<unsigned char>(s[i + 1]))) s[i] = '.';
    }
    return s;
}

Table pin(std::string id, Text title, Text summary, Text description, Row columns, std::vector<Row> rows) {
    for (auto& row : rows) {
        for (auto& cell : row) {
            if (cell.de.empty() && cell.en.empty()) cell = {"–", "–"};
            if (cell.de == cell.en) cell.en = decimalPoint(cell.en);
        }
    }
    Table t;
    t.id = std::move(id);
    t.category = "comms";  // final area from the layout
    t.title = std::move(title);
    t.summary = std::move(summary);
    t.description = std::move(description);
    t.columns = std::move(columns);
    t.rows = std::move(rows);
    return t;
}

const Row kPinSig{{"Pin", "Pin"}, {"Signal", "Signal"}, {"Beschreibung", "Description"}};

Table poe() {
    return pin("pin_poe", {"Pinbelegung Ethernet mit PoE", "Ethernet with PoE"}, {"Power over Ethernet, Modus A und B", "Power over Ethernet, mode A and B"},
               {"IEEE 802.3af/at (PoE, PoE+): Modus A speist über die Datenpaare 1/2 und 3/6 (Phantomspeisung), Modus B über die "
                "freien Paare 4/5 und 7/8 (10/100 Mbit/s). IEEE 802.3bt (4PPoE, bis 90 W) nutzt alle vier Paare. Polarität bei "
                "Modus A je nach MDI/MDI-X. Leistung am Gerät: af 12,95 W, at 25,5 W, bt Typ 3 51 W, Typ 4 71,3 W.",
                "IEEE 802.3af/at (PoE, PoE+): mode A feeds over data pairs 1/2 and 3/6 (phantom), mode B over spare pairs 4/5 and "
                "7/8 (10/100 Mbit/s). IEEE 802.3bt (4PPoE, up to 90 W) uses all four pairs. Polarity in mode A depends on MDI/MDI-X. "
                "Power at the device: af 12.95 W, at 25.5 W, bt type 3 51 W, type 4 71.3 W."},
               {"Pin", {"T568B-Farbe", "T568B colour"}, {"Daten (10/100)", "Data (10/100)"}, {"PoE Modus A", "PoE mode A"}, {"PoE Modus B", "PoE mode B"}},
               {{"1", {"weiß-orange", "white-orange"}, "TX+", "DC+ / DC−", "–"},
                {"2", {"orange", "orange"}, "TX−", "DC+ / DC−", "–"},
                {"3", {"weiß-grün", "white-green"}, "RX+", "DC− / DC+", "–"},
                {"4", {"blau", "blue"}, {"frei", "unused"}, "–", "DC+"},
                {"5", {"weiß-blau", "white-blue"}, {"frei", "unused"}, "–", "DC+"},
                {"6", {"grün", "green"}, "RX−", "DC− / DC+", "–"},
                {"7", {"weiß-braun", "white-brown"}, {"frei", "unused"}, "–", "DC−"},
                {"8", {"braun", "brown"}, {"frei", "unused"}, "–", "DC−"}});
}

Table rjTelephone() {
    return pin("pin_rj_telephone", {"Pinbelegung RJ-9, 11, 14, 25, 48", "RJ-9, 11, 14, 25, 48 pinout"},
               {"Telefon- und T1/E1-Steckverbinder", "Telephone and T1/E1 connectors"},
               {"Registered Jacks (FCC Part 68 / TIA-968): RJ11 = 6P2C (1 Leitung), RJ14 = 6P4C (2 Leitungen), RJ25 = 6P6C "
                "(3 Leitungen), RJ9 = 4P4C (Hörerkabel), RJ48C = 8P8C für T1/E1. Kontakte von links bei Blick auf die Kontakte, "
                "Rastnase unten. Tip (a) / Ring (b) einer Leitung liegen symmetrisch zur Mitte.",
                "Registered jacks (FCC Part 68 / TIA-968): RJ11 = 6P2C (1 line), RJ14 = 6P4C (2 lines), RJ25 = 6P6C (3 lines), "
                "RJ9 = 4P4C (handset cord), RJ48C = 8P8C for T1/E1. Contacts from the left looking at the contacts, latch down. "
                "Tip / ring of one line are symmetrical to the centre."},
               {{"Stecker", "Jack"}, {"Kontakte", "Contacts"}, {"Belegung", "Assignment"}},
               {{"RJ9 (4P4C)", "4", {"1 Mikrofon, 2/3 Hörer, 4 Mikrofon (herstellerabhängig)", "1 mic, 2/3 receiver, 4 mic (vendor-specific)"}},
                {"RJ11 (6P2C)", "6", {"3 Ring, 4 Tip (Leitung 1)", "3 ring, 4 tip (line 1)"}},
                {"RJ14 (6P4C)", "6", {"3/4 Leitung 1, 2/5 Leitung 2", "3/4 line 1, 2/5 line 2"}},
                {"RJ25 (6P6C)", "6", {"3/4 Leitung 1, 2/5 Leitung 2, 1/6 Leitung 3", "3/4 line 1, 2/5 line 2, 1/6 line 3"}},
                {"RJ48C (8P8C)", "8", {"1 RX Ring, 2 RX Tip, 4 TX Ring, 5 TX Tip, 3/6 Schirm", "1 RX ring, 2 RX tip, 4 TX ring, 5 TX tip, 3/6 shield"}}});
}

Table scart() {
    return pin("pin_scart", {"Pinbelegung SCART", "SCART pinout"}, {"21-polig, EN 50049-1", "21 pins, EN 50049-1"},
               {"Péritel/SCART nach EN 50049-1 (IEC 60933-1). Pin 8: 0–2 V aus, 5–8 V 16:9, 9,5–12 V 4:3. Pin 16: 1–3 V = RGB.",
                "Péritel/SCART per EN 50049-1 (IEC 60933-1). Pin 8: 0–2 V off, 5–8 V 16:9, 9.5–12 V 4:3. Pin 16: 1–3 V = RGB."},
               kPinSig,
               {{"1", {"Audio R aus", "Audio R out"}, "0,5 V eff"},          {"2", {"Audio R ein", "Audio R in"}, "0,5 V eff"},
                {"3", {"Audio L aus", "Audio L out"}, "0,5 V eff"},          {"4", {"Audio-Masse", "Audio ground"}, "–"},
                {"5", {"Masse Blau", "Blue ground"}, "–"},                   {"6", {"Audio L ein", "Audio L in"}, "0,5 V eff"},
                {"7", {"RGB Blau", "RGB blue"}, "0,7 V / 75 Ω"},             {"8", {"Schaltspannung / Format", "Status / aspect"}, "0–12 V"},
                {"9", {"Masse Grün", "Green ground"}, "–"},                  {"10", {"Daten 2 (Clock)", "Data 2 (clock)"}, "–"},
                {"11", {"RGB Grün", "RGB green"}, "0,7 V / 75 Ω"},           {"12", {"Daten 1", "Data 1"}, "–"},
                {"13", {"Masse Rot", "Red ground"}, "–"},                    {"14", {"Masse Daten", "Data ground"}, "–"},
                {"15", {"RGB Rot / Chroma (C)", "RGB red / chroma (C)"}, "0,7 V / 75 Ω"},
                {"16", {"Austastung / RGB-Umschaltung", "Blanking / RGB switching"}, "1–3 V"},
                {"17", {"Masse Video", "Video ground"}, "–"},                {"18", {"Masse Austastung", "Blanking ground"}, "–"},
                {"19", {"FBAS (Composite) aus", "Composite video out"}, "1 V / 75 Ω"},
                {"20", {"FBAS ein / Luma (Y)", "Composite in / luma (Y)"}, "1 V / 75 Ω"},
                {"21", {"Schirm / Masse", "Shield / ground"}, "–"}});
}

Table vga() {
    return pin("pin_vga", {"Pinbelegung VGA", "VGA pinout"}, {"DE-15 (HD15) mit DDC", "DE-15 (HD15) with DDC"},
               {"VESA DDC2 / E-DDC. RGB analog 0,7 V an 75 Ω, H-/V-Sync TTL. Pin 9 = +5 V für das EDID-EEPROM (bei älteren Karten frei).",
                "VESA DDC2 / E-DDC. Analogue RGB 0.7 V into 75 Ω, H/V sync TTL. Pin 9 = +5 V for the EDID EEPROM (unused on old cards)."},
               kPinSig,
               {{"1", {"Rot", "Red"}, "0,7 V / 75 Ω"},     {"2", {"Grün", "Green"}, "0,7 V / 75 Ω"},  {"3", {"Blau", "Blue"}, "0,7 V / 75 Ω"},
                {"4", "ID2", {"reserviert", "reserved"}}, {"5", "GND", {"Masse (HSync)", "ground (HSync)"}},
                {"6", {"Masse Rot", "Red return"}, "–"},  {"7", {"Masse Grün", "Green return"}, "–"},  {"8", {"Masse Blau", "Blue return"}, "–"},
                {"9", "+5 V", {"Versorgung DDC", "DDC supply"}}, {"10", "GND", {"Masse (VSync, DDC)", "ground (VSync, DDC)"}},
                {"11", "ID0", {"reserviert", "reserved"}}, {"12", "SDA", {"DDC-Daten", "DDC data"}},
                {"13", "HSync", {"Horizontalsynchronisation", "horizontal sync"}}, {"14", "VSync", {"Vertikalsynchronisation", "vertical sync"}},
                {"15", "SCL", {"DDC-Takt", "DDC clock"}}});
}

Table dvi() {
    return pin("pin_dvi", {"Pinbelegung DVI", "DVI pinout"}, {"DVI-D/DVI-I, 24 + 5 Kontakte", "DVI-D/DVI-I, 24 + 5 contacts"},
               {"DDWG DVI 1.0: TMDS-Kanäle 0–2 (Single Link), 3–5 zusätzlich bei Dual Link. C1–C5 nur bei DVI-I/DVI-A (analog, "
                "VGA-kompatibel).",
                "DDWG DVI 1.0: TMDS channels 0–2 (single link), 3–5 additionally for dual link. C1–C5 only on DVI-I/DVI-A "
                "(analogue, VGA-compatible)."},
               kPinSig,
               {{"1", "TMDS 2−", ""}, {"2", "TMDS 2+", ""}, {"3", {"Schirm 2/4", "Shield 2/4"}, ""}, {"4", "TMDS 4−", "Dual Link"},
                {"5", "TMDS 4+", "Dual Link"}, {"6", "DDC SCL", {"Takt", "clock"}}, {"7", "DDC SDA", {"Daten", "data"}},
                {"8", {"Analog VSync", "Analogue VSync"}, "DVI-I"}, {"9", "TMDS 1−", ""}, {"10", "TMDS 1+", ""},
                {"11", {"Schirm 1/3", "Shield 1/3"}, ""}, {"12", "TMDS 3−", "Dual Link"}, {"13", "TMDS 3+", "Dual Link"},
                {"14", "+5 V", {"Versorgung DDC", "DDC supply"}}, {"15", "GND", {"Masse (+5 V, Sync)", "ground (+5 V, sync)"}},
                {"16", "Hot Plug", {"Monitor erkannt", "monitor present"}}, {"17", "TMDS 0−", ""}, {"18", "TMDS 0+", ""},
                {"19", {"Schirm 0/5", "Shield 0/5"}, ""}, {"20", "TMDS 5−", "Dual Link"}, {"21", "TMDS 5+", "Dual Link"},
                {"22", {"Schirm Takt", "Clock shield"}, ""}, {"23", {"TMDS Takt+", "TMDS clock+"}, ""}, {"24", {"TMDS Takt−", "TMDS clock−"}, ""},
                {"C1", {"Analog Rot", "Analogue red"}, "DVI-I"}, {"C2", {"Analog Grün", "Analogue green"}, "DVI-I"},
                {"C3", {"Analog Blau", "Analogue blue"}, "DVI-I"}, {"C4", {"Analog HSync", "Analogue HSync"}, "DVI-I"},
                {"C5", {"Analog Masse", "Analogue ground"}, "DVI-I"}});
}

Table firewire() {
    return pin("pin_firewire", {"Pinbelegung FireWire (IEEE 1394)", "FireWire (IEEE 1394) pinout"}, {"6-polig und 4-polig", "6-pin and 4-pin"},
               {"IEEE 1394a. 6-polig mit Versorgung (8–30 V DC, max. 1,5 A), 4-polig (i.LINK) ohne Versorgung: Pins 3–6 des "
                "6-poligen entsprechen 1–4 des 4-poligen.",
                "IEEE 1394a. 6-pin with power (8–30 V DC, max. 1.5 A), 4-pin (i.LINK) without power: pins 3–6 of the 6-pin match "
                "1–4 of the 4-pin."},
               {{"6-polig", "6-pin"}, {"4-polig", "4-pin"}, {"Signal", "Signal"}},
               {{"1", "–", {"Versorgung +", "Power +"}}, {"2", "–", {"Masse", "Ground"}}, {"3", "1", "TPB−"}, {"4", "2", "TPB+"},
                {"5", "3", "TPA−"}, {"6", "4", "TPA+"}});
}

Table molexSata() {
    return pin("pin_molex_sata", {"Pinbelegung Molex und SATA", "Molex and SATA pinout"}, {"PC-Stromversorgung und SATA-Daten", "PC power and SATA data"},
               {"Molex 8981 (4-polig, Laufwerke) nach ATX-Netzteilspezifikation; SATA-Daten 7-polig und SATA-Strom 15-polig (Serial ATA "
                "Rev. 3). Aderfarben: +12 V gelb, +5 V rot, +3,3 V orange, Masse schwarz.",
                "Molex 8981 (4-pin, drives) per ATX power supply spec; SATA data 7-pin and SATA power 15-pin (Serial ATA rev. 3). "
                "Wire colours: +12 V yellow, +5 V red, +3.3 V orange, ground black."},
               {{"Stecker", "Connector"}, "Pin", {"Signal", "Signal"}},
               {{"Molex 4-polig", "1", {"+12 V (gelb)", "+12 V (yellow)"}}, {"Molex 4-polig", "2", {"Masse (schwarz)", "Ground (black)"}},
                {"Molex 4-polig", "3", {"Masse (schwarz)", "Ground (black)"}}, {"Molex 4-polig", "4", {"+5 V (rot)", "+5 V (red)"}},
                {{"SATA-Daten", "SATA data"}, "1, 4, 7", {"Masse", "Ground"}}, {{"SATA-Daten", "SATA data"}, "2 / 3", {"A+ / A− (Senden)", "A+ / A− (transmit)"}},
                {{"SATA-Daten", "SATA data"}, "5 / 6", {"B− / B+ (Empfangen)", "B− / B+ (receive)"}},
                {{"SATA-Strom", "SATA power"}, "1–3", "+3,3 V"}, {{"SATA-Strom", "SATA power"}, "4–6", {"Masse", "Ground"}},
                {{"SATA-Strom", "SATA power"}, "7–9", "+5 V"}, {{"SATA-Strom", "SATA power"}, "10, 12", {"Masse", "Ground"}},
                {{"SATA-Strom", "SATA power"}, "11", {"Aktivität / Staggered Spin-up", "Activity / staggered spin-up"}},
                {{"SATA-Strom", "SATA power"}, "13–15", "+12 V"}});
}

Table lightning() {
    return pin("pin_lightning", {"Pinbelegung Apple Lightning", "Apple Lightning pinout"}, {"8-polig, beidseitig steckbar", "8-pin, reversible"},
               {"Apple Lightning (2012–2023): Der Stecker ist drehbar, die Buchse ordnet die Kontakte über einen Chip im Stecker zu. "
                "Belegung einer Steckerseite (nach Wikipedia „Lightning (connector)“).",
                "Apple Lightning (2012–2023): the plug is reversible, the socket assigns the contacts via a chip in the plug. "
                "Assignment of one plug side (after Wikipedia “Lightning (connector)”)."},
               kPinSig,
               {{"1", "GND", {"Masse", "Ground"}}, {"2", "L0p", {"Datenpaar 0 +", "Lane 0 +"}}, {"3", "L0n", {"Datenpaar 0 −", "Lane 0 −"}},
                {"4", "ID0", {"Kennung/Steuerung 0", "Identification/control 0"}}, {"5", "PWR", {"Versorgung (Ladegerät/Akku)", "Power (charger/battery)"}},
                {"6", "L1n", {"Datenpaar 1 −", "Lane 1 −"}}, {"7", "L1p", {"Datenpaar 1 +", "Lane 1 +"}},
                {"8", "ID1", {"Kennung/Steuerung 1", "Identification/control 1"}}});
}

Table appleDock() {
    return pin("pin_apple_dock", {"Pinbelegung Apple-Dock (30-polig)", "Apple dock connector (30-pin)"}, {"iPod/iPhone/iPad bis 2012", "iPod/iPhone/iPad until 2012"},
               {"Apple 30-Pin Dock Connector (2003–2012), Belegung nach The Apple Wiki. Video und FireWire nur bei bestimmten Modellen.",
                "Apple 30-pin dock connector (2003–2012), assignment after The Apple Wiki. Video and FireWire only on certain models."},
               kPinSig,
               {{"1, 2", "GND", {"Masse (Audio/Video)", "Ground (audio/video)"}}, {"3", {"Line out R", "Line out R"}, {"Audio rechts", "Audio right"}},
                {"4", {"Line out L", "Line out L"}, {"Audio links", "Audio left"}}, {"5", {"Line in R", "Line in R"}, {"Audio rechts", "Audio right"}},
                {"6", {"Line in L", "Line in L"}, {"Audio links", "Audio left"}}, {"8", {"Video aus", "Video out"}, {"Composite", "composite"}},
                {"9", "S-Video C", {"Chrominanz", "chrominance"}}, {"10", "S-Video Y", {"Luminanz", "luminance"}},
                {"11", "GND", {"Masse seriell", "Serial ground"}}, {"12", "TxD", {"seriell senden", "serial transmit"}},
                {"13", "RxD", {"seriell empfangen", "serial receive"}}, {"15, 16", "GND", {"Masse (USB)", "Ground (USB)"}},
                {"18", "+3,3 V", {"Versorgung", "Power"}}, {"19, 20", "+12 V", {"FireWire-Versorgung", "FireWire power"}},
                {"21", "ACC ID", {"Zubehörkennung (Widerstand)", "Accessory ID (resistor)"}}, {"22", "TPA−", "FireWire"},
                {"23", "+5 V", {"USB-Versorgung", "USB power"}}, {"24", "TPA+", "FireWire"}, {"25", "USB D−", "USB"},
                {"26", "TPB−", "FireWire"}, {"27", "USB D+", "USB"}, {"28", "TPB+", "FireWire"},
                {"29, 30", "GND", {"Masse (FireWire)", "Ground (FireWire)"}}, {"7, 14, 17", "–", {"nicht belegt", "not used"}}});
}

Table displayPort() {
    return pin("pin_displayport", {"Pinbelegung DisplayPort", "DisplayPort pinout"}, {"20-polig (Quelle)", "20 pins (source)"},
               {"VESA DisplayPort 1.x, Steckerbelegung an der Quelle (Grafikkarte). 4 Datenkanäle (Main Link), AUX-Kanal für EDID/DPCD, "
                "Hot Plug Detect, DP_PWR = 3,3 V / 500 mA.",
                "VESA DisplayPort 1.x, pinout at the source (graphics card). 4 main-link lanes, AUX channel for EDID/DPCD, hot plug "
                "detect, DP_PWR = 3.3 V / 500 mA."},
               kPinSig,
               {{"1", "ML_Lane 0+", ""}, {"2", "GND", ""}, {"3", "ML_Lane 0−", ""}, {"4", "ML_Lane 1+", ""}, {"5", "GND", ""},
                {"6", "ML_Lane 1−", ""}, {"7", "ML_Lane 2+", ""}, {"8", "GND", ""}, {"9", "ML_Lane 2−", ""}, {"10", "ML_Lane 3+", ""},
                {"11", "GND", ""}, {"12", "ML_Lane 3−", ""}, {"13", "CONFIG1", {"Masse bzw. Konfiguration", "ground or configuration"}},
                {"14", "CONFIG2", {"Masse bzw. Konfiguration", "ground or configuration"}}, {"15", "AUX+", {"Hilfskanal", "auxiliary channel"}},
                {"16", "GND", ""}, {"17", "AUX−", {"Hilfskanal", "auxiliary channel"}}, {"18", "Hot Plug", {"Monitor erkannt", "monitor present"}},
                {"19", "DP_PWR Return", {"Masse Versorgung", "power return"}}, {"20", "DP_PWR", "+3,3 V"}});
}

Table ps2() {
    return pin("pin_ps2", {"Pinbelegung PS/2", "PS/2 pinout"}, {"Mini-DIN 6, Tastatur (violett) und Maus (grün)", "Mini-DIN 6, keyboard (purple) and mouse (green)"},
               {"IBM PS/2 (Mini-DIN 6). Synchrones serielles Protokoll, Takt vom Gerät. Farben nach PC 99: Tastatur violett, Maus grün.",
                "IBM PS/2 (mini-DIN 6). Synchronous serial protocol, clock from the device. Colours per PC 99: keyboard purple, mouse green."},
               kPinSig,
               {{"1", "DATA", {"Daten", "data"}}, {"2", "NC", {"frei", "not connected"}}, {"3", "GND", {"Masse", "ground"}},
                {"4", "+5 V", {"Versorgung (max. ca. 275 mA)", "power (max. approx. 275 mA)"}}, {"5", "CLK", {"Takt", "clock"}},
                {"6", "NC", {"frei", "not connected"}}});
}

Table fiberColours() {
    return pin("fiber_colours", {"Farbcode optische Fasern", "Fibre optic colour code"}, {"Faserfarben TIA-598 und Mantelfarben", "Fibre colours TIA-598 and jacket colours"},
               {"Faser- und Bündeladerfarben nach TIA-598-D (international üblich; DIN VDE 0888 nutzt teils andere Reihenfolge: rot, grün, "
                "blau, gelb, weiß, grau, braun, violett, türkis, schwarz, orange, rosa). Ab Faser 13 Wiederholung mit Ring/Strich. "
                "Mantelfarben: OS1/OS2 gelb, OM1/OM2 orange, OM3 aqua, OM4 aqua oder violett, OM5 limettengrün. Stecker: UPC blau, "
                "APC (8°) grün, Multimode beige/aqua.",
                "Fibre and buffer tube colours per TIA-598-D (common internationally; DIN VDE 0888 partly uses another order: red, "
                "green, blue, yellow, white, grey, brown, violet, turquoise, black, orange, pink). From fibre 13 repeated with ring/stripe. "
                "Jackets: OS1/OS2 yellow, OM1/OM2 orange, OM3 aqua, OM4 aqua or violet, OM5 lime green. Connectors: UPC blue, APC "
                "(8°) green, multimode beige/aqua."},
               {{"Faser", "Fibre"}, {"Farbe TIA-598", "Colour TIA-598"}},
               {{"1", {"blau", "blue"}},   {"2", {"orange", "orange"}}, {"3", {"grün", "green"}},   {"4", {"braun", "brown"}},
                {"5", {"schiefergrau", "slate"}}, {"6", {"weiß", "white"}}, {"7", {"rot", "red"}}, {"8", {"schwarz", "black"}},
                {"9", {"gelb", "yellow"}},  {"10", {"violett", "violet"}}, {"11", {"rosa", "rose"}}, {"12", {"aqua (türkis)", "aqua"}}});
}

Table ledPinout() {
    return pin("pin_led", {"Pinbelegung LED", "LED pinout"}, {"Anode, Kathode und RGB-LEDs", "Anode, cathode and RGB LEDs"},
               {"Bedrahtete LED (3/5 mm): langes Bein = Anode (+), kurzes Bein = Kathode (−), abgeflachte Gehäuseseite = Kathode. "
                "RGB-LED mit 4 Beinen: längstes Bein = gemeinsamer Anschluss (Anode oder Kathode). Vorwiderstand siehe Rechner. "
                "Typische Flussspannungen bei 20 mA siehe Tabelle.",
                "Through-hole LED (3/5 mm): long leg = anode (+), short leg = cathode (−), flat side of the body = cathode. RGB LED "
                "with 4 legs: longest leg = common (anode or cathode). Series resistor see calculator. Typical forward voltages at 20 mA "
                "see table."},
               {{"Merkmal / Farbe", "Feature / colour"}, {"Anschluss bzw. U_F", "Terminal or U_F"}},
               {{{"langes Bein", "long leg"}, {"Anode (+)", "anode (+)"}}, {{"kurzes Bein, flache Seite", "short leg, flat side"}, {"Kathode (−)", "cathode (−)"}},
                {{"RGB, längstes Bein", "RGB, longest leg"}, {"gemeinsame Anode oder Kathode", "common anode or cathode"}},
                {{"rot", "red"}, "1,8–2,1 V"}, {{"gelb / orange", "yellow / orange"}, "2,0–2,2 V"}, {{"grün (klassisch)", "green (classic)"}, "2,0–2,4 V"},
                {{"blau, weiß, grün (InGaN)", "blue, white, green (InGaN)"}, "2,9–3,4 V"}, {"IR", "1,2–1,5 V"}});
}

Table raspberryPi() {
    const std::vector<std::array<const char*, 4>> p{
        {"1", "3,3 V", "2", "5 V"},           {"3", "GPIO2 (SDA1)", "4", "5 V"},       {"5", "GPIO3 (SCL1)", "6", "GND"},
        {"7", "GPIO4 (GPCLK0)", "8", "GPIO14 (TXD)"}, {"9", "GND", "10", "GPIO15 (RXD)"}, {"11", "GPIO17", "12", "GPIO18 (PWM0)"},
        {"13", "GPIO27", "14", "GND"},        {"15", "GPIO22", "16", "GPIO23"},        {"17", "3,3 V", "18", "GPIO24"},
        {"19", "GPIO10 (MOSI)", "20", "GND"}, {"21", "GPIO9 (MISO)", "22", "GPIO25"},  {"23", "GPIO11 (SCLK)", "24", "GPIO8 (CE0)"},
        {"25", "GND", "26", "GPIO7 (CE1)"},   {"27", "GPIO0 (ID_SD)", "28", "GPIO1 (ID_SC)"}, {"29", "GPIO5", "30", "GND"},
        {"31", "GPIO6", "32", "GPIO12 (PWM0)"}, {"33", "GPIO13 (PWM1)", "34", "GND"},  {"35", "GPIO19 (PCM_FS)", "36", "GPIO16"},
        {"37", "GPIO26", "38", "GPIO20 (PCM_DIN)"}, {"39", "GND", "40", "GPIO21 (PCM_DOUT)"},
    };
    std::vector<Row> rows;
    auto en = [](std::string s) {
        for (auto& ch : s) {
            if (ch == ',') ch = '.';
        }
        return s;
    };
    for (const auto& x : p) rows.push_back({{x[0], x[0]}, {x[1], en(x[1])}, {x[2], x[2]}, {x[3], en(x[3])}});
    return pin("pin_raspberry_pi", {"Pinbelegung Raspberry Pi", "Raspberry Pi pinout"}, {"40-polige GPIO-Leiste (ab Modell B+)", "40-pin GPIO header (from model B+)"},
               {"Raspberry Pi B+, 2, 3, 4, 5, Zero: physische Pinnummer und BCM-GPIO-Nummer. GPIO-Pegel 3,3 V (nicht 5-V-tolerant!). "
                "Pin 27/28 für HAT-EEPROM reserviert.",
                "Raspberry Pi B+, 2, 3, 4, 5, Zero: physical pin number and BCM GPIO number. GPIO level 3.3 V (not 5 V tolerant!). "
                "Pins 27/28 reserved for the HAT EEPROM."},
               {"Pin", {"Funktion", "Function"}, "Pin", {"Funktion", "Function"}}, rows);
}

Table iso10487() {
    return pin("pin_iso10487", {"Pinbelegung ISO 10487 (Autoradio)", "ISO 10487 (car radio) pinout"}, {"Stecker A (Strom) und B (Lautsprecher)", "Connector A (power) and B (speakers)"},
               {"ISO 10487-1/-2. Achtung: Einige Fahrzeughersteller tauschen A4 (Dauerplus) und A7 (Zündungsplus) – vor dem Einbau messen.",
                "ISO 10487-1/-2. Note: some vehicle makers swap A4 (permanent +12 V) and A7 (ignition +12 V) – measure before installing."},
               kPinSig,
               {{"A4", {"+12 V Dauerplus", "+12 V permanent"}, {"Speicher (gelb)", "memory (yellow)"}},
                {"A5", {"+12 V Antenne/Verstärker", "+12 V antenna/amplifier"}, {"geschaltet (blau)", "switched (blue)"}},
                {"A6", {"Beleuchtung", "Illumination"}, {"(orange)", "(orange)"}},
                {"A7", {"+12 V Zündungsplus", "+12 V ignition"}, {"(rot)", "(red)"}},
                {"A8", {"Masse", "Ground"}, {"(braun/schwarz)", "(brown/black)"}},
                {"A1–A3", {"herstellerabhängig", "manufacturer-specific"}, {"z. B. Telefon-Stummschaltung", "e.g. phone mute"}},
                {"B1 / B2", {"hinten rechts + / −", "rear right + / −"}, {"violett", "violet"}},
                {"B3 / B4", {"vorne rechts + / −", "front right + / −"}, {"grau", "grey"}},
                {"B5 / B6", {"vorne links + / −", "front left + / −"}, {"weiß", "white"}},
                {"B7 / B8", {"hinten links + / −", "rear left + / −"}, {"grün", "green"}}});
}

Table obd2() {
    return pin("pin_obd2", {"Pinbelegung OBD II", "OBD II pinout"}, {"16-polige Diagnosebuchse (SAE J1962)", "16-pin diagnostic connector (SAE J1962)"},
               {"SAE J1962 / ISO 15031-3. Protokolle: CAN ISO 15765-4 (6/14, Pflicht in USA ab 2008, EU-OBD), K-Line ISO 9141-2 / "
                "ISO 14230 (7/15), SAE J1850 PWM/VPW (2/10). Nicht genannte Pins: herstellerspezifisch.",
                "SAE J1962 / ISO 15031-3. Protocols: CAN ISO 15765-4 (6/14, mandatory in the USA from 2008, EU OBD), K-line "
                "ISO 9141-2 / ISO 14230 (7/15), SAE J1850 PWM/VPW (2/10). Pins not listed: manufacturer-specific."},
               kPinSig,
               {{"2", "J1850 Bus+", "PWM/VPW"}, {"4", {"Fahrzeugmasse", "Chassis ground"}, ""}, {"5", {"Signalmasse", "Signal ground"}, ""},
                {"6", "CAN High", "ISO 15765-4"}, {"7", "K-Line", "ISO 9141-2 / 14230"}, {"10", "J1850 Bus−", "PWM"},
                {"14", "CAN Low", "ISO 15765-4"}, {"15", "L-Line", "ISO 9141-2 / 14230"}, {"16", {"+12 V Batterie", "+12 V battery"}, {"Dauerplus", "permanent"}}});
}

Table xlr() {
    return pin("pin_xlr", {"Pinbelegung XLR (Audio/DMX)", "XLR (audio/DMX) pinout"}, {"3-polig Audio/DMX, 5-polig DMX", "3-pin audio/DMX, 5-pin DMX"},
               {"Audio symmetrisch nach IEC 60268-12 / AES14 (Pin 2 = heiß). DMX512 nach ANSI E1.11 (USITT): 5-poliger XLR genormt, "
                "3-polig verbreitet; RS-485, 120-Ω-Kabel und Abschluss am Ende.",
                "Balanced audio per IEC 60268-12 / AES14 (pin 2 = hot). DMX512 per ANSI E1.11 (USITT): 5-pin XLR standard, 3-pin "
                "common; RS-485, 120 Ω cable and termination at the end."},
               {"Pin", {"Audio (3-polig)", "Audio (3-pin)"}, {"DMX (3/5-polig)", "DMX (3/5-pin)"}},
               {{"1", {"Masse / Schirm", "Ground / shield"}, {"Masse / Schirm", "Common / shield"}}, {"2", {"heiß (+)", "hot (+)"}, {"Daten 1 −", "Data 1 −"}},
                {"3", {"kalt (−)", "cold (−)"}, {"Daten 1 +", "Data 1 +"}}, {"4", "–", {"Daten 2 − (optional)", "Data 2 − (optional)"}},
                {"5", "–", {"Daten 2 + (optional)", "Data 2 + (optional)"}}});
}

Table midi() {
    return pin("pin_midi", {"Pinbelegung MIDI", "MIDI pinout"}, {"DIN 5-polig (180°)", "DIN 5-pin (180°)"},
               {"MIDI 1.0 (MMA): Stromschleife 5 mA mit Optokoppler im Empfänger, 31 250 Bit/s. Schirm (Pin 2) nur an der Senderseite "
                "(MIDI OUT) mit Masse verbunden.",
                "MIDI 1.0 (MMA): 5 mA current loop with an opto-isolator in the receiver, 31 250 bit/s. Shield (pin 2) connected to "
                "ground at the sender (MIDI OUT) only."},
               kPinSig,
               {{"1", "NC", {"frei", "not connected"}}, {"2", {"Schirm", "Shield"}, {"Masse nur an MIDI OUT", "ground at MIDI OUT only"}},
                {"3", "NC", {"frei", "not connected"}}, {"4", {"Stromquelle", "Current source"}, {"+5 V über 220 Ω", "+5 V via 220 Ω"}},
                {"5", {"Stromsenke / Daten", "Current sink / data"}, {"über 220 Ω", "via 220 Ω"}}});
}

Table jack() {
    return pin("pin_jack", {"Pinbelegung Klinke (Jack)", "Jack (TRS/TRRS) pinout"}, {"3,5 mm und 6,35 mm, TS/TRS/TRRS", "3.5 mm and 6.35 mm, TS/TRS/TRRS"},
               {"Tip = Spitze, Ring, Sleeve = Schaft. TRRS-Headset: CTIA/AHJ (heute üblich: Apple, Android) vs. OMTP (ältere Geräte) – "
                "Mikrofon und Masse vertauscht.",
                "Tip, ring, sleeve. TRRS headset: CTIA/AHJ (common today: Apple, Android) vs. OMTP (older devices) – microphone and "
                "ground swapped."},
               {{"Kontakt", "Contact"}, {"Mono (TS)", "Mono (TS)"}, {"Stereo (TRS)", "Stereo (TRS)"}, {"Symmetrisch (TRS)", "Balanced (TRS)"},
                {"Headset CTIA", "Headset CTIA"}, {"Headset OMTP", "Headset OMTP"}},
               {{{"Spitze (Tip)", "Tip"}, {"Signal", "Signal"}, {"links", "left"}, {"heiß (+)", "hot (+)"}, {"links", "left"}, {"links", "left"}},
                {{"Ring 1", "Ring 1"}, "–", {"rechts", "right"}, {"kalt (−)", "cold (−)"}, {"rechts", "right"}, {"rechts", "right"}},
                {{"Ring 2", "Ring 2"}, "–", "–", "–", {"Masse", "ground"}, {"Mikrofon", "microphone"}},
                {{"Schaft (Sleeve)", "Sleeve"}, {"Masse", "ground"}, {"Masse", "ground"}, {"Masse / Schirm", "ground / shield"}, {"Mikrofon", "microphone"},
                 {"Masse", "ground"}}});
}

Table rcaColours() {
    return pin("rca_colours", {"RCA-Farbcode (Cinch)", "RCA colour code"}, {"Farben der Cinch-Stecker", "Colours of RCA plugs"},
               {"Übliche Farbkennzeichnung nach IEC 60603-14 / CEA-863 für Cinch-Verbindungen (Consumer-Video und -Audio).",
                "Common colour coding per IEC 60603-14 / CEA-863 for RCA connections (consumer video and audio)."},
               {{"Farbe", "Colour"}, {"Signal", "Signal"}},
               {{{"gelb", "yellow"}, {"Composite-Video (FBAS)", "Composite video (CVBS)"}},
                {{"weiß (oder schwarz)", "white (or black)"}, {"Audio links (analog)", "Audio left (analogue)"}},
                {{"rot", "red"}, {"Audio rechts (analog) bzw. Component Pr", "Audio right (analogue) or component Pr"}},
                {{"grün", "green"}, {"Component Y (Luminanz)", "Component Y (luma)"}},
                {{"blau", "blue"}, {"Component Pb", "Component Pb"}},
                {{"orange", "orange"}, {"Digital-Audio S/PDIF koaxial", "Digital audio S/PDIF coaxial"}},
                {{"violett", "purple"}, {"Subwoofer / LFE", "Subwoofer / LFE"}}});
}

Table thunderbolt() {
    return pin("pin_thunderbolt", {"Pinbelegung Thunderbolt 3/4 (USB-C)", "Thunderbolt 3/4 (USB-C) pinout"}, {"24-polige USB-C-Buchse", "24-pin USB-C receptacle"},
               {"Thunderbolt 3/4 nutzen die USB-C-Buchse (USB Type-C Spezifikation): je Seite ein Hochgeschwindigkeits-Sende-/Empfangspaar "
                "(TX/RX), USB 2.0 (D+/D−), CC für Erkennung und Power Delivery, SBU für Seitenbandsignale. Thunderbolt 1/2 nutzten den "
                "Mini-DisplayPort-Stecker.",
                "Thunderbolt 3/4 use the USB-C receptacle (USB Type-C specification): per side one high-speed transmit/receive pair "
                "(TX/RX), USB 2.0 (D+/D−), CC for detection and Power Delivery, SBU for sideband signals. Thunderbolt 1/2 used the "
                "Mini DisplayPort plug."},
               {{"Pin A", "Pin A"}, {"Signal", "Signal"}, {"Pin B", "Pin B"}, {"Signal", "Signal"}},
               {{"A1", "GND", "B12", "GND"},     {"A2", "TX1+", "B11", "RX1+"}, {"A3", "TX1−", "B10", "RX1−"}, {"A4", "VBUS", "B9", "VBUS"},
                {"A5", "CC1", "B8", "SBU2"},     {"A6", "D+", "B7", "D−"},      {"A7", "D−", "B6", "D+"},      {"A8", "SBU1", "B5", "CC2 (VCONN)"},
                {"A9", "VBUS", "B4", "VBUS"},    {"A10", "RX2−", "B3", "TX2−"}, {"A11", "RX2+", "B2", "TX2+"}, {"A12", "GND", "B1", "GND"}});
}

Table sdCard() {
    return pin("pin_sd_card", {"Pinbelegung SD-Karte", "SD card pinout"}, {"SD und microSD im SD- und SPI-Modus", "SD and microSD in SD and SPI mode"},
               {"SD Association Physical Layer Spec. Versorgung 2,7–3,6 V. Im SPI-Modus: CS, MOSI (CMD), MISO (DAT0), SCLK.",
                "SD Association physical layer spec. Supply 2.7–3.6 V. In SPI mode: CS, MOSI (CMD), MISO (DAT0), SCLK."},
               {{"SD-Pin", "SD pin"}, {"microSD-Pin", "microSD pin"}, {"SD-Modus", "SD mode"}, {"SPI-Modus", "SPI mode"}},
               {{"1", "2", "CD/DAT3", "CS"}, {"2", "3", "CMD", "MOSI"}, {"3", "–", "VSS1 (GND)", "GND"}, {"4", "4", "VDD", "VDD"},
                {"5", "5", "CLK", "SCLK"}, {"6", "6", "VSS2 (GND)", "GND"}, {"7", "7", "DAT0", "MISO"}, {"8", "8", "DAT1", "–"}, {"9", "1", "DAT2", "–"}});
}

Table simCard() {
    return pin("pin_sim_card", {"Pinbelegung SIM-Karte", "SIM card pinout"}, {"Kontakte C1–C8 nach ISO/IEC 7816-2", "Contacts C1–C8 per ISO/IEC 7816-2"},
               {"ISO/IEC 7816-2/-3 (Chipkarten), ETSI TS 102 221 (UICC). Gilt für Mini-, Micro- und Nano-SIM (gleiche Kontaktlage).",
                "ISO/IEC 7816-2/-3 (smart cards), ETSI TS 102 221 (UICC). Same for mini, micro and nano SIM (same contact layout)."},
               kPinSig,
               {{"C1", "VCC", {"Versorgung 1,8 / 3 / 5 V", "supply 1.8 / 3 / 5 V"}}, {"C2", "RST", {"Reset", "reset"}},
                {"C3", "CLK", {"Takt", "clock"}}, {"C4", {"(AUX1)", "(AUX1)"}, {"optional, z. B. USB D+", "optional, e.g. USB D+"}},
                {"C5", "GND", {"Masse", "ground"}}, {"C6", {"VPP / SWP", "VPP / SWP"}, {"NFC (Single Wire Protocol)", "NFC (single wire protocol)"}},
                {"C7", "I/O", {"Daten (halbduplex)", "data (half duplex)"}}, {"C8", {"(AUX2)", "(AUX2)"}, {"optional, z. B. USB D−", "optional, e.g. USB D−"}}});
}

Table lcd1602() {
    return pin("pin_lcd1602", {"Pinbelegung Display LCD 16x2", "LCD 16x2 display pinout"}, {"HD44780-kompatibel, 16-polig", "HD44780-compatible, 16 pins"},
               {"Zeichendisplays mit HD44780-Controller. 4-Bit-Betrieb: nur D4–D7 anschließen. Kontrast an V0 über Poti 10 kΩ; "
                "Hintergrundbeleuchtung meist mit Vorwiderstand.",
                "Character displays with HD44780 controller. 4-bit mode: connect D4–D7 only. Contrast at V0 via a 10 kΩ pot; backlight "
                "usually with a series resistor."},
               kPinSig,
               {{"1", "VSS", {"Masse", "ground"}}, {"2", "VDD", "+5 V"}, {"3", "V0", {"Kontrast", "contrast"}},
                {"4", "RS", {"0 = Befehl, 1 = Daten", "0 = command, 1 = data"}}, {"5", "R/W", {"0 = schreiben, 1 = lesen", "0 = write, 1 = read"}},
                {"6", "E", {"Enable (Takt)", "enable (strobe)"}}, {"7–10", "D0–D3", {"Datenbits (nur 8-Bit-Betrieb)", "data bits (8-bit mode only)"}},
                {"11–14", "D4–D7", {"Datenbits", "data bits"}}, {"15", "A", {"Beleuchtung +", "backlight +"}}, {"16", "K", {"Beleuchtung −", "backlight −"}}});
}

Table ioLink() {
    return pin("pin_io_link", {"Pinbelegung IO-Link", "IO-Link pinout"}, {"M12, Port Class A und B", "M12, port class A and B"},
               {"IEC 61131-9 (IO-Link). M12 A-codiert. Class A: Pin 2 frei verwendbar (DI/DO), Class B: Pin 2/5 zusätzliche "
                "galvanisch getrennte Aktorversorgung. Ungeschirmte 3- oder 5-adrige Leitung, max. 20 m. Aderfarben: 1 braun, 2 weiß, "
                "3 blau, 4 schwarz, 5 grau.",
                "IEC 61131-9 (IO-Link). M12 A-coded. Class A: pin 2 free (DI/DO), class B: pins 2/5 additional galvanically isolated "
                "actuator supply. Unshielded 3- or 5-core cable, max. 20 m. Wire colours: 1 brown, 2 white, 3 blue, 4 black, 5 grey."},
               {"Pin", {"Port Class A", "Port class A"}, {"Port Class B", "Port class B"}},
               {{"1", "L+ (24 V)", "L+ (24 V)"}, {"2", "I/Q (DI/DO)", {"P24 (Aktor +24 V)", "P24 (actuator +24 V)"}}, {"3", "L− (0 V)", "L− (0 V)"},
                {"4", {"C/Q (IO-Link / SIO)", "C/Q (IO-Link / SIO)"}, {"C/Q (IO-Link / SIO)", "C/Q (IO-Link / SIO)"}},
                {"5", {"frei", "not connected"}, {"N24 (Aktor 0 V)", "N24 (actuator 0 V)"}}});
}

}  // namespace

void addPinoutTables(std::vector<Table>& out) {
    out.push_back(poe());
    out.push_back(rjTelephone());
    out.push_back(scart());
    out.push_back(vga());
    out.push_back(dvi());
    out.push_back(firewire());
    out.push_back(molexSata());
    out.push_back(lightning());
    out.push_back(appleDock());
    out.push_back(displayPort());
    out.push_back(ps2());
    out.push_back(fiberColours());
    out.push_back(ledPinout());
    out.push_back(raspberryPi());
    out.push_back(iso10487());
    out.push_back(obd2());
    out.push_back(xlr());
    out.push_back(midi());
    out.push_back(jack());
    out.push_back(rcaColours());
    out.push_back(thunderbolt());
    out.push_back(sdCard());
    out.push_back(simCard());
    out.push_back(lcd1602());
    out.push_back(ioLink());
}

}  // namespace elc::catalog
