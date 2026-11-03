// Area "Überspannungsschutz" (v1.10): selection of surge protective devices (SPD) for power (230/400 V) by lightning
// protection zone and earthing system, SPD for I&C, sensors and data lines, protective distance, reference tables.
// Oriented on DEHN (Kurzkatalog Elektrohandwerk DS135, Auswahlmatrix Industriegebäude DS390, BLITZDUCTOR data sheets)
// and the standards IEC 62305-1/-4, IEC 60364-4-44/-5-53 (DIN VDE 0100-443/-534), IEC 60664-1, IEC 61643-11/-21,
// OVE Fachinformation E 09 "Schutzbereich von SPD".
#include <cmath>
#include <string>
#include <vector>

#include "catalog/helpers.hpp"
#include "elc/formulas/automation.hpp"

namespace elc::catalog {

namespace {

constexpr const char* kCat = "surge";

/// peak lightning current of the lightning protection level (IEC 62305-1, 10/350 µs)
double lplCurrent(const std::string& lpl) {
    if (lpl == "1") return 200;
    if (lpl == "2") return 150;
    return 100;  // III and IV
}

std::vector<Option> lplOptions() {
    return {{"1", {"Blitzschutzklasse I (200 kA)", "LPL I (200 kA)"}},
            {"2", {"Blitzschutzklasse II (150 kA)", "LPL II (150 kA)"}},
            {"3", {"Blitzschutzklasse III (100 kA)", "LPL III (100 kA)"}},
            {"4", {"Blitzschutzklasse IV (100 kA)", "LPL IV (100 kA)"}}};
}

// ------------------------------------------------------------------ power supply 230/400 V

Calculator spdPower() {
    Calculator c;
    c.id = "spd_power";
    c.category = kCat;
    c.title = {"Überspannungsschutz 230/400 V: Auswahl", "Surge protection 230/400 V: selection"};
    c.summary = {"Typ 1+2, 2 oder 3 je Einbauort und Blitzschutzzone, Schaltung, U_C, I_imp/I_n, U_p",
                 "Type 1+2, 2 or 3 per location and protection zone, connection, U_C, I_imp/I_n, U_p"};
    c.description = {
        "Blitzschutzzonen-Konzept (IEC 62305-4): LPZ 0A → 1 (äußerer Blitzschutz oder Freileitung) Blitzstrom-Ableiter Typ 1 "
        "bzw. Kombi-Ableiter Typ 1+2; LPZ 0B → 1 und 1 → 2 Überspannungs-Ableiter Typ 2; LPZ 2 → 3 Typ 3 am Endgerät.\n"
        "Folgestufe: Typ 2 in der Unterverteilung, wenn die Leitung zum vorgeschalteten SPD länger als 10 m ist; Typ 3 vor "
        "dem Gerät entsprechend (DEHN-Auswahlmatrix).\n"
        "I_imp je Pfad (äußerer Blitzschutz, IEC 62305-1 Anhang E vereinfacht): 50 % des Blitzstroms der Blitzschutzklasse "
        "fließen in die Erdungsanlage, 50 % verteilen sich auf die n Versorgungsleitungen und deren m Leiter: "
        "I_imp = 0,5 · I / (n · m). Ohne Bestimmung mindestens 12,5 kA je Pfad (DIN VDE 0100-534). N–PE-Funkenstrecke (3+1) "
        "führt die Summe.\n"
        "Typ 2: I_n ≥ 5 kA je Pfad (8/20 µs), üblich 20 kA. U_C ≥ 1,1 · U₀ = 253 V (L–N, L–PE), N–PE ≥ U₀; IT ohne N: L–PE ≥ U.\n"
        "Schutzpegel U_p ≤ 2,5 kV (Überspannungskategorie II), wirksamer Schutz bis ca. 10 m bei U_p ≤ 0,8 · U_W = 2,0 kV.\n"
        "Anschluss: a + b ≤ 0,5 m (V-Verdrahtung), Cu ≥ 16 mm² (Typ 1) bzw. ≥ 6 mm² (Typ 2) zum Potentialausgleich. "
        "In DE im Vorzählerbereich nur Typ 1 (VDE-AR-N 4100). Produktbeispiele DEHN: DEHNventil, DEHNshield (Typ 1+2), "
        "DEHNguard (Typ 2), DEHNrail, DEHNflex, DEHNsafe (Typ 3). Gegen gültige Normausgabe und Herstellerdaten prüfen.",
        "Lightning protection zone concept (IEC 62305-4): LPZ 0A → 1 (external LPS or overhead line) lightning current arrester "
        "type 1 or combined arrester type 1+2; LPZ 0B → 1 and 1 → 2 surge arrester type 2; LPZ 2 → 3 type 3 at the equipment.\n"
        "Next stage: type 2 in the sub-distribution if the cable to the upstream SPD is longer than 10 m; type 3 ahead of the "
        "equipment likewise (DEHN selection matrix).\n"
        "I_imp per path (external LPS, IEC 62305-1 annex E simplified): 50 % of the lightning current of the LPL flows into the "
        "earth termination, 50 % divides over the n services and their m conductors: I_imp = 0.5 · I / (n · m). Without "
        "determination at least 12.5 kA per path (IEC 60364-5-53). The N–PE spark gap (3+1) carries the sum.\n"
        "Type 2: I_n ≥ 5 kA per path (8/20 µs), commonly 20 kA. U_C ≥ 1.1 · U₀ = 253 V (L–N, L–PE), N–PE ≥ U₀; IT without N: "
        "L–PE ≥ U.\n"
        "Protection level U_p ≤ 2.5 kV (overvoltage category II), effective protection up to about 10 m with U_p ≤ 0.8 · U_W = "
        "2.0 kV.\n"
        "Connection: a + b ≤ 0.5 m (V-wiring), Cu ≥ 16 mm² (type 1) or ≥ 6 mm² (type 2) to the equipotential bonding. In "
        "Germany only type 1 ahead of the meter (VDE-AR-N 4100). DEHN examples: DEHNventil, DEHNshield (type 1+2), DEHNguard "
        "(type 2), DEHNrail, DEHNflex, DEHNsafe (type 3). Check against the edition in force and manufacturer data."};
    c.inputs = {
        Num::choice("loc", {"Einbauort", "Location"},
                    {{"main", {"Einspeisung / Hauptverteilung", "Service entrance / main distribution"}},
                     {"sub", {"Unterverteilung", "Sub-distribution"}},
                     {"device", {"Endgerät (Steckdose, Gerät)", "Terminal equipment (socket, device)"}}},
                    "main"),
        Num::choice("lps", {"Gebäude / Einspeisung", "Building / supply"},
                    {{"none", {"ohne äußeren Blitzschutz, Erdkabel", "no external LPS, underground cable"}},
                     {"overhead", {"ohne äußeren Blitzschutz, Freileitung", "no external LPS, overhead line"}},
                     {"lps", {"mit äußerem Blitzschutz (LPS)", "with external LPS"}}},
                    "lps"),
        Num::choice("lpl", {"Blitzschutzklasse", "Lightning protection level"}, lplOptions(), "3"),
        Num::number("services", {"Metallene Versorgungsleitungen n", "Metallic services n"}, "", 1).range(1, 10)
            .withHint({"Strom, Wasser, Gas, Daten … – 1 = alles über die Energieleitung (sicher)",
                       "power, water, gas, data … – 1 = everything via the power line (safe side)"}),
        Num::number("dist", {"Leitungslänge zum vorgeschalteten SPD", "Cable length to the upstream SPD"}, "m", 15).nonNegative(),
        Num::choice("net", {"Netzsystem", "Earthing system"},
                    {{"tns", "TN-S / TN-C-S"}, {"tnc", {"TN-C (vor der PEN-Aufteilung)", "TN-C (ahead of the PEN split)"}},
                     {"tt", "TT"}, {"it", {"IT ohne Neutralleiter", "IT without neutral"}}},
                    "tns"),
        Num::choice("ph", {"Versorgung", "Supply"}, {{"3", "3~ 230/400 V"}, {"1", "1~ 230 V"}}, "3"),
    };
    c.inputs[1].showIf("loc", {"main"});
    c.inputs[2].showIf("lps", {"lps"});
    c.inputs[3].showIf("lps", {"lps"});
    c.inputs[4].showIf("loc", {"sub", "device"});
    c.outputs = {Out::text("type", {"Ableiter", "Arrester"}).primary(),
                 Out::text("lpz", {"Zonenübergang", "Zone transition"}),
                 Out::text("scheme", {"Schaltung", "Connection"}).primary(),
                 Out::number("Uc", {"U_C mindestens (L–N / L–PE)", "U_C at least (L–N / L–PE)"}, "V", 0),
                 Out::number("UcN", {"U_C mindestens N–PE", "U_C at least N–PE"}, "V", 0),
                 Out::number("Iimp", {"I_imp je Pfad (10/350 µs)", "I_imp per path (10/350 µs)"}, "kA", 1),
                 Out::number("IimpTot", {"I_imp gesamt / N–PE", "I_imp total / N–PE"}, "kA", 1),
                 Out::number("In", {"I_n je Pfad (8/20 µs) mindestens", "I_n per path (8/20 µs) at least"}, "kA", 0),
                 Out::number("Up", {"Schutzpegel U_p höchstens", "Protection level U_p at most"}, "kV", 1),
                 Out::text("wiring", {"Anschluss", "Connection wiring"}),
                 Out::text("fuse", {"Vorsicherung", "Back-up fuse"}),
                 Out::text("dehn", {"Beispiele DEHN", "DEHN examples"}),
                 Out::text("next", {"Nächste Schutzstufe", "Next protection stage"})};
    c.keywords = {"überspannungsschutz", "blitzschutz", "spd", "typ 1", "typ 2", "typ 3", "kombiableiter", "dehn", "dehnguard",
                  "dehnventil", "blitzschutzzone", "lpz", "surge protection", "lightning"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& loc = in.choice("loc");
        const auto& net = in.choice("net");
        const bool three = in.choice("ph") == "3";
        if (net == "tnc" && !three) throw CalcError({"TN-C einphasig ist nicht zulässig (PEN ≥ 10 mm² Cu) – TN-S wählen.",
                                                     "Single-phase TN-C is not permitted (PEN ≥ 10 mm² Cu) – choose TN-S."});
        // conductors that share the lightning current: L + N or PEN (PE is bonded directly)
        const int m = three ? (net == "it" ? 3 : 4) : 2;
        // connection scheme
        Text scheme;
        if (net == "tnc") scheme = {"3+0: L1, L2, L3 → PEN", "3+0: L1, L2, L3 → PEN"};
        else if (net == "it") scheme = three ? Text{"3+0: L1, L2, L3 → PE (U_C ≥ U)", "3+0: L1, L2, L3 → PE (U_C ≥ U)"}
                                             : Text{"2+0: L, L → PE", "2+0: L, L → PE"};
        else if (net == "tt") scheme = three ? Text{"3+1: L1, L2, L3 → N, N → PE (Summenfunkenstrecke)", "3+1: L1, L2, L3 → N, N → PE (spark gap)"}
                                             : Text{"1+1: L → N, N → PE", "1+1: L → N, N → PE"};
        else scheme = three ? Text{"4+0 (L1, L2, L3, N → PE) oder 3+1", "4+0 (L1, L2, L3, N → PE) or 3+1"}
                            : Text{"2+0 (L, N → PE) oder 1+1", "2+0 (L, N → PE) or 1+1"};
        r.setText("scheme", scheme);
        r.set("Uc", net == "it" ? 400.0 : 1.1 * 230.0);
        if (net == "tns" || net == "tt") r.set("UcN", 230.0);

        Text type, lpz, dehn, next, wiring;
        bool type1 = false, type2 = false;
        double up = 2.0;
        const double dist = in.num("dist");
        if (loc == "main") {
            const auto& lps = in.choice("lps");
            if (lps == "none") {
                type2 = true;
                type = {"Überspannungs-Ableiter Typ 2", "Surge arrester type 2"};
                lpz = {"LPZ 0B → 1 (keine Direkteinschläge über die Einspeisung)", "LPZ 0B → 1 (no direct strikes via the supply)"};
                r.info({"In DE gilt im Vorzählerbereich nur Typ 1 (VDE-AR-N 4100) – dort Kombi-Ableiter Typ 1+2 „Basic“ einsetzen.",
                        "In Germany only type 1 is allowed ahead of the meter (VDE-AR-N 4100) – use a basic type 1+2 combined arrester there."});
            } else {
                type1 = true;
                type = {"Kombi-Ableiter Typ 1+2 (Blitzstrom-Ableiter)", "Combined arrester type 1+2 (lightning current arrester)"};
                lpz = {"LPZ 0A → 1 (Blitzteilströme möglich)", "LPZ 0A → 1 (partial lightning currents possible)"};
                double iimp = 12.5;
                if (lps == "lps") {
                    iimp = 0.5 * lplCurrent(in.choice("lpl")) / (std::round(in.num("services")) * m);
                    if (iimp < 12.5) {
                        r.info({"Rechnerisch < 12,5 kA je Pfad – mindestens 12,5 kA wählen (DIN VDE 0100-534).",
                                "Calculated < 12.5 kA per path – choose at least 12.5 kA (IEC 60364-5-53)."});
                    }
                } else {
                    r.info({"Freileitung ohne äußeren Blitzschutz: Typ 1 mit mindestens 12,5 kA je Pfad.",
                            "Overhead line without external LPS: type 1 with at least 12.5 kA per path."});
                }
                r.set("Iimp", iimp);
                r.set("IimpTot", iimp * m);
            }
            up = 2.0;
            dehn = type1 ? Text{"DEHNventil (modular, Industrie/Gewerbe) oder DEHNshield (anwendungsoptimiert, Wohn- und Zweckbau)",
                                "DEHNventil (modular, industry/commercial) or DEHNshield (application-optimised, residential/functional)"}
                         : Text{"DEHNguard M/MP (Typ 2, modular)", "DEHNguard M/MP (type 2, modular)"};
            next = {"Typ 2 in jeder Unterverteilung, die mehr als 10 m Leitung vom Hauptverteiler entfernt ist; Typ 3 vor empfindlichen Geräten.",
                    "Type 2 in every sub-distribution more than 10 m of cable away from the main board; type 3 ahead of sensitive equipment."};
        } else if (loc == "sub") {
            if (dist > 10) {
                type2 = true;
                type = {"Überspannungs-Ableiter Typ 2", "Surge arrester type 2"};
                lpz = {"LPZ 1 → 2", "LPZ 1 → 2"};
            } else {
                type = {"kein weiteres SPD nötig (im Schutzbereich ≤ 10 m des vorgeschalteten SPD)",
                        "no further SPD needed (within the ≤ 10 m protective range of the upstream SPD)"};
                lpz = {"innerhalb LPZ 1", "within LPZ 1"};
            }
            up = 2.0;
            dehn = {"DEHNguard M/MP (Typ 2)", "DEHNguard M/MP (type 2)"};
            next = {"Typ 3 vor Geräten, die mehr als 10 m Leitung entfernt sind.", "Type 3 ahead of equipment more than 10 m of cable away."};
        } else {
            if (dist > 10) {
                type = {"Geräteschutz Typ 3", "Device protection type 3"};
                lpz = {"LPZ 2 → 3", "LPZ 2 → 3"};
            } else {
                type = {"Typ 3 nur für besonders empfindliche Geräte – Gerät liegt im Schutzbereich des Typ 2",
                        "Type 3 only for particularly sensitive equipment – within the protective range of the type 2"};
                lpz = {"innerhalb LPZ 2", "within LPZ 2"};
            }
            up = 1.5;
            dehn = {"DEHNrail (Hutschiene), DEHNflex (Unterputzdose), DEHNsafe (Steckdose), DEHNprotector (Steckdosenleiste), DEHNcord",
                    "DEHNrail (DIN rail), DEHNflex (flush box), DEHNsafe (socket), DEHNprotector (power strip), DEHNcord"};
            next = {"Typ 3 benötigt ein vorgeschaltetes Typ 2 (energetische Koordination); Daten- und Antennenleitungen des Geräts mitschützen.",
                    "Type 3 needs an upstream type 2 (energy coordination); protect the device's data and antenna lines as well."};
        }
        if (type2) r.set("In", 5.0);
        r.set("Up", up);
        wiring = type1 ? Text{"a + b ≤ 0,5 m (V-Verdrahtung), Cu ≥ 16 mm² zur Haupterdungsschiene", "a + b ≤ 0.5 m (V-wiring), Cu ≥ 16 mm² to the main earthing bar"}
                 : type2 ? Text{"a + b ≤ 0,5 m (V-Verdrahtung), Cu ≥ 6 mm² zum PE/Potentialausgleich", "a + b ≤ 0.5 m (V-wiring), Cu ≥ 6 mm² to PE/bonding"}
                         : Text{"möglichst nahe am Gerät, gemeinsamer Bezug mit den Datenleitungen", "as close to the equipment as possible, common reference with the data lines"};
        r.setText("type", type);
        r.setText("lpz", lpz);
        r.setText("wiring", wiring);
        r.setText("fuse", type1 || type2
                              ? Text{"Nur nötig, wenn die vorgeschaltete Sicherung größer als die max. Vorsicherung des SPD ist (z. B. DEHNguard 125 A gG, "
                                     "DEHNshield 160 A gG, DEHNventil 250/315 A gG).",
                                     "Only needed if the upstream fuse exceeds the SPD's maximum back-up fuse (e.g. DEHNguard 125 A gG, DEHNshield "
                                     "160 A gG, DEHNventil 250/315 A gG)."}
                              : Text{"durch den Endstromkreis (z. B. LS 16 A)", "by the final circuit (e.g. MCB 16 A)"});
        r.setText("dehn", dehn);
        r.setText("next", next);
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ I&C, sensors, bus and data lines

struct SignalSpec {
    const char* id;
    Text name;
    double defaultU;
    Text dehn;
    Text band;
};

const std::vector<SignalSpec>& signals() {
    static const std::vector<SignalSpec> s{
        {"ma", {"4–20 mA (Messumformer, 24-V-Speisung)", "4–20 mA (transmitter, 24 V supply)"}, 24,
         {"BLITZDUCTOR XT/XTU ML4 BE 24 bzw. BLITZDUCTORconnect ML2 BE 24 (U_C 33 V DC, 0,75 A)",
          "BLITZDUCTOR XT/XTU ML4 BE 24 or BLITZDUCTORconnect ML2 BE 24 (U_C 33 V DC, 0.75 A)"},
         {"Gleichstromsignal – Grenzfrequenz unkritisch", "DC signal – cut-off frequency not critical"}},
        {"v010", {"0–10 V (Stellsignal, Sollwert)", "0–10 V (control signal, set point)"}, 24,
         {"BLITZDUCTORconnect ML2 BE 24 / BLITZDUCTOR XT ML4 BE 24 (U_C 33 V DC)", "BLITZDUCTORconnect ML2 BE 24 / BLITZDUCTOR XT ML4 BE 24 (U_C 33 V DC)"},
         {"Gleichstromsignal – Grenzfrequenz unkritisch", "DC signal – cut-off frequency not critical"}},
        {"pt", {"Widerstandsfühler Pt100/Pt1000", "RTD Pt100/Pt1000"}, 5,
         {"BLITZDUCTOR mit geringer Längsimpedanz (Datenblatt), 3- oder 4-Leiter-Anschluss bevorzugen",
          "BLITZDUCTOR with low series impedance (data sheet), prefer 3- or 4-wire connection"},
         {"Messstrom im mA-Bereich – die Längsimpedanz des Ableiters verfälscht 2-Leiter-Messungen", "mA measuring current – the series impedance falsifies 2-wire measurements"}},
        {"bin", {"Binärsignal 24 V DC (Kontakt, Meldung)", "Binary signal 24 V DC (contact, status)"}, 24,
         {"BLITZDUCTOR XT ML4 BD 24 / BE 24 (U_C 33 V DC)", "BLITZDUCTOR XT ML4 BD 24 / BE 24 (U_C 33 V DC)"},
         {"Gleichstromsignal", "DC signal"}},
        {"bin12", {"Binärsignal 12 V DC (z. B. Einbruchmelde)", "Binary signal 12 V DC (e.g. intrusion alarm)"}, 12,
         {"BLITZDUCTORconnect ML2 BD 12 / BLITZDUCTOR XT ML2 BD S 12 (U_C 15 V DC)", "BLITZDUCTORconnect ML2 BD 12 / BLITZDUCTOR XT ML2 BD S 12 (U_C 15 V DC)"},
         {"Gleichstromsignal", "DC signal"}},
        {"rs485", {"RS-485: Modbus RTU, BACnet MS/TP, Profibus", "RS-485: Modbus RTU, BACnet MS/TP, Profibus"}, 5,
         {"BLITZDUCTORconnect ML2 BD HF 5 (U_C 8,5 V DC) / BLITZDUCTOR XT ML4 BD HF 5 (U_C 6 V DC)",
          "BLITZDUCTORconnect ML2 BD HF 5 (U_C 8.5 V DC) / BLITZDUCTOR XT ML4 BD HF 5 (U_C 6 V DC)"},
         {"HF-Typ mit hoher Grenzfrequenz (Bitrate bis Mbit/s), Schirm einbinden", "HF type with high cut-off frequency (bit rate up to Mbit/s), include the screen"}},
        {"knx", {"KNX TP (Busleitung 30 V DC)", "KNX TP (bus 30 V DC)"}, 30,
         {"BUStector BT 24 (U_C 45 V DC) bzw. BLITZDUCTOR … B 180 für Busleitungen zwischen Gebäuden",
          "BUStector BT 24 (U_C 45 V DC) or BLITZDUCTOR … B 180 for bus lines between buildings"},
         {"9,6 kbit/s – unkritisch", "9.6 kbit/s – not critical"}},
        {"eth", {"Ethernet / PoE (BACnet/IP, Modbus TCP, Profinet)", "Ethernet / PoE (BACnet/IP, Modbus TCP, Profinet)"}, 57,
         {"DEHNpatch DPA CL8 EA 4PPoE (Cat 6A, PoE bis 58 V DC, 500 MHz)", "DEHNpatch DPA CL8 EA 4PPoE (Cat 6A, PoE up to 58 V DC, 500 MHz)"},
         {"Bandbreite ≥ Kategorie der Strecke (Cat 6A: 500 MHz), PoE-Klasse beachten", "bandwidth ≥ link category (Cat 6A: 500 MHz), mind the PoE class"}},
        {"tel", {"Telefon / DSL (VDSL, G.fast)", "Telephone / DSL (VDSL, G.fast)"}, 120,
         {"DEHNbox DBX TC B 180 / DEHNrapid LSA (U_C 180 V DC)", "DEHNbox DBX TC B 180 / DEHNrapid LSA (U_C 180 V DC)"},
         {"Breitband bis 35 MHz (VDSL) bzw. 212 MHz (G.fast)", "broadband up to 35 MHz (VDSL) or 212 MHz (G.fast)"}},
    };
    return s;
}

const SignalSpec& signalOf(const std::string& id) {
    for (const auto& s : signals()) {
        if (id == s.id) return s;
    }
    return signals().front();
}

Calculator spdSignal() {
    Calculator c;
    c.id = "spd_signal";
    c.category = kCat;
    c.title = {"Überspannungsschutz MSR, Sensoren und Datenleitungen", "Surge protection I&C, sensors and data lines"};
    c.summary = {"Ableiterart nach Zone, U_C-Klasse, Bandbreite, Pt100-Messfehler – mit DEHN-Beispielen",
                 "Arrester type by zone, U_C class, bandwidth, Pt100 error – with DEHN examples"};
    c.description = {
        "Leitungen, die das Gebäude verlassen (Feldgeräte im Freien, Leitungen zwischen Gebäuden): Zonenübergang LPZ 0A → 1, "
        "Kombi-Ableiter mit Blitzstromtragfähigkeit D1 (10/350 µs), z. B. 2,5 kA je Ader (DEHN BLITZDUCTOR XT ML4 BE 24: "
        "D1 2,5 kA je Ader, C2 10 kA je Ader). Innerhalb des Gebäudes (LPZ 1 → 2): Überspannungs-Ableiter Kategorie C2 (8/20 µs).\n"
        "U_C ≥ höchste Betriebsspannung des Signals einschließlich Toleranz; Nennstrom des Ableiters ≥ Signal-/Laststrom; "
        "Grenzfrequenz ≥ Übertragungsfrequenz.\n"
        "Pt100/Pt1000 im 2-Leiter-Anschluss: jede Ader hat die Längsimpedanz R des Ableiters, Messfehler ΔT = 2 · R · Stufen / "
        "(dR/dT); 3-/4-Leiter-Anschluss kompensiert. In Ex-i-Kreisen nur dafür zugelassene Ableiter.\n"
        "Werte nach DEHN Auswahlmatrix Industriegebäude (DS390) und BLITZDUCTOR-Datenblatt; IEC 61643-21/-22, IEC 62305-4.",
        "Lines leaving the building (field devices outdoors, lines between buildings): zone transition LPZ 0A → 1, combined "
        "arrester with lightning current capability D1 (10/350 µs), e.g. 2.5 kA per core (DEHN BLITZDUCTOR XT ML4 BE 24: "
        "D1 2.5 kA per core, C2 10 kA per core). Inside the building (LPZ 1 → 2): surge arrester category C2 (8/20 µs).\n"
        "U_C ≥ highest operating voltage of the signal incl. tolerance; rated current of the arrester ≥ signal/load current; "
        "cut-off frequency ≥ transmission frequency.\n"
        "Pt100/Pt1000 in 2-wire connection: every core carries the series impedance R of the arrester, error ΔT = 2 · R · stages "
        "/ (dR/dT); 3/4-wire compensates. In Ex i circuits only arresters approved for them.\n"
        "Values per DEHN selection matrix industrial buildings (DS390) and BLITZDUCTOR data sheet; IEC 61643-21/-22, IEC 62305-4."};
    std::vector<Option> sigOpts;
    for (const auto& s : signals()) sigOpts.push_back({s.id, s.name});
    c.inputs = {
        Num::choice("sig", {"Signal / Schnittstelle", "Signal / interface"}, sigOpts, "ma"),
        Num::choice("zone", {"Leitungsweg", "Cable route"},
                    {{"out", {"verlässt das Gebäude / Feldgerät im Freien (LPZ 0A → 1)", "leaves the building / field device outdoors (LPZ 0A → 1)"}},
                     {"in", {"nur innerhalb des Gebäudes (LPZ 1 → 2)", "inside the building only (LPZ 1 → 2)"}}},
                    "out"),
        Num::number("U", {"Höchste Betriebsspannung des Signals", "Highest operating voltage of the signal"}, "V", 24).positive()
            .withHint({"z. B. 24 V DC + Toleranz; PoE bis 57 V", "e.g. 24 V DC + tolerance; PoE up to 57 V"}),
        Num::choice("rtd", {"Fühler", "Sensor"}, {{"pt100", "Pt100"}, {"pt1000", "Pt1000"}}, "pt100"),
        Num::choice("wires", {"Anschluss", "Connection"}, {{"2", {"2-Leiter", "2-wire"}}, {"3", {"3-Leiter", "3-wire"}}, {"4", {"4-Leiter", "4-wire"}}}, "2"),
        Num::number("R", {"Längsimpedanz je Ader und Ableiter", "Series impedance per core and arrester"}, "Ω", 1.8).nonNegative()
            .withHint({"Datenblatt, z. B. 1,8 Ω (BXT ML4 BE 24)", "data sheet, e.g. 1.8 Ω (BXT ML4 BE 24)"}),
        Num::choice("ends", {"Ableiter im Messkreis", "Arresters in the loop"},
                    {{"1", {"eine Stufe (Schaltschrank)", "one stage (panel)"}}, {"2", {"zwei Stufen (Feld + Schaltschrank)", "two stages (field + panel)"}}},
                    "2"),
    };
    for (const int i : {3, 4, 5, 6}) c.inputs[static_cast<std::size_t>(i)].showIf("sig", {"pt"});
    c.outputs = {Out::text("arrester", {"Ableiter", "Arrester"}).primary(),
                 Out::text("lpz", {"Zonenübergang", "Zone transition"}),
                 Out::number("Uc", {"U_C-Klasse mindestens", "U_C class at least"}, "V", 1).primary(),
                 Out::text("band", {"Übertragung", "Transmission"}),
                 Out::text("dehn", {"Beispiele DEHN", "DEHN examples"}),
                 Out::text("install", {"Einbau", "Installation"}),
                 Out::number("dT", {"Messfehler durch Ableiter (2-Leiter)", "Error caused by the arrester (2-wire)"}, "K", 2)};
    c.keywords = {"blitzductor", "msr", "sensor", "4-20 ma", "pt100", "bus", "modbus", "bacnet", "knx", "ethernet", "poe", "dehnpatch",
                  "datenleitung", "überspannungsschutz", "surge protection", "signal"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& s = signalOf(in.choice("sig"));
        const bool out = in.choice("zone") == "out";
        r.setText("arrester", out ? Text{"Kombi-Ableiter mit Blitzstromtragfähigkeit (D1 10/350 µs, z. B. 2,5 kA je Ader) und Überspannungsschutz (C2)",
                                         "Combined arrester with lightning current capability (D1 10/350 µs, e.g. 2.5 kA per core) and surge protection (C2)"}
                                  : Text{"Überspannungs-Ableiter Kategorie C2 (8/20 µs, z. B. 5–10 kA je Ader)",
                                         "Surge arrester category C2 (8/20 µs, e.g. 5–10 kA per core)"});
        r.setText("lpz", out ? Text{"LPZ 0A → 1 (Gebäudeeintritt) und am Feldgerät", "LPZ 0A → 1 (building entry) and at the field device"}
                             : Text{"LPZ 1 → 2 (Schaltschrank/Unterverteiler)", "LPZ 1 → 2 (panel/sub-distribution)"});
        // U_C classes of DEHN data/MSR arresters (DS390): 6, 8.5, 15, 33, 45, 54, 58 (PoE), 180 V DC
        const double U = in.num("U");
        double uc = 180;
        for (const double k : {6.0, 8.5, 15.0, 33.0, 45.0, 54.0, 58.0, 180.0}) {
            if (k >= U) {
                uc = k;
                break;
            }
        }
        if (U > 180) throw CalcError({"Über 180 V: Ableiter für Energietechnik verwenden.", "Above 180 V: use a power-supply SPD."});
        r.set("Uc", uc);
        r.setText("band", s.band);
        r.setText("dehn", s.dehn);
        r.setText("install", out ? Text{"Beidseitig beschalten: am Gebäudeeintritt (Zonengrenze) und am Feldgerät; Erdung kurz an den Potentialausgleich, "
                                        "Schirm beidseitig direkt oder einseitig direkt und anderseitig indirekt (Gasableiter).",
                                        "Protect both ends: at the building entry (zone boundary) and at the field device; short earthing to the bonding, "
                                        "screen directly at both ends or directly at one end and indirectly (gas discharge tube) at the other."}
                                 : Text{"Am Eintritt in den Schaltschrank bzw. vor dem empfindlichen Gerät; kurze Erdung an die Potentialausgleichsschiene.",
                                        "At the entry to the panel or ahead of the sensitive device; short earthing to the bonding bar."});
        if (in.choice("sig") == "pt") {
            const auto type = in.choice("rtd") == "pt1000" ? automation::Rtd::Pt1000 : automation::Rtd::Pt100;
            const double sens = automation::rtdSensitivity(type, 20.0);
            const double stages = in.choice("ends") == "2" ? 2.0 : 1.0;
            const double err = in.choice("wires") == "2" ? 2.0 * in.num("R") * stages / sens : 0.0;
            r.set("dT", err);
            if (err > 0.5) {
                r.warn({"Messfehler > 0,5 K – 3-/4-Leiter-Anschluss, Pt1000 oder Ableiter mit kleinerer Längsimpedanz wählen.",
                        "Error > 0.5 K – use 3/4-wire, Pt1000 or an arrester with lower series impedance."});
            } else if (in.choice("wires") != "2") {
                r.info({"3-/4-Leiter: Längsimpedanzen werden kompensiert (gleiche Adern).", "3/4-wire: series impedances are compensated (equal cores)."});
            }
        }
        return r;
    };
    return c;
}

// ------------------------------------------------------------------ protective distance

Calculator spdDistance() {
    Calculator c;
    c.id = "spd_distance";
    c.category = kCat;
    c.title = {"Wirksamer Schutzpegel und Schutzbereich", "Effective protection level and protective distance"};
    c.summary = {"U_p,f = U_p + ΔU, Vergleich mit U_W, Schwingungsschutzbereich l_po", "U_p/f = U_p + ΔU, comparison with U_W, oscillation distance l_po"};
    c.description = {
        "Wirksamer Schutzpegel U_p,f = U_p + ΔU (Anschlussleitungen): für 230/400 V pauschal ΔU = 0,5 kV (OVE Fachinformation E 09) "
        "bzw. bei Blitzstrom ca. 1 kV je m Anschlussleitung (IEC 62305-4).\n"
        "U_p,f ≤ 0,5 · U_W: Schutzbereich unabhängig von der Leitungslänge. Sonst gilt der konventionelle Schutzbereich von 10 m, "
        "wenn U_p,f ≤ 0,8 · U_W; darüber hinaus Schwingungsschutzbereich l_po = (U_W − U_p,f) / k mit k = 25 V/m.\n"
        "U_W = Bemessungs-Stoßspannung des Geräts (230/400 V: Kategorie I 1,5 kV, II 2,5 kV, III 4 kV, IV 6 kV, IEC 60664-1). "
        "Induktiver Schutzbereich bei nahen/direkten Einschlägen zusätzlich nach IEC 62305-4 Anhang prüfen.",
        "Effective protection level U_p/f = U_p + ΔU (connecting leads): for 230/400 V a flat ΔU = 0.5 kV (OVE information E 09) "
        "or about 1 kV per m of lead with lightning current (IEC 62305-4).\n"
        "U_p/f ≤ 0.5 · U_W: protection independent of the cable length. Otherwise the conventional protective range of 10 m "
        "applies if U_p/f ≤ 0.8 · U_W; beyond that oscillation protective distance l_po = (U_W − U_p/f) / k with k = 25 V/m.\n"
        "U_W = rated impulse withstand voltage of the equipment (230/400 V: category I 1.5 kV, II 2.5 kV, III 4 kV, IV 6 kV, "
        "IEC 60664-1). Check the induction protective distance for near/direct strikes per IEC 62305-4 annex as well."};
    c.inputs = {
        Num::number("Up", {"Schutzpegel U_p des SPD", "Protection level U_p of the SPD"}, "kV", 1.5).positive(),
        Num::choice("du", {"Zuschlag Anschlussleitung", "Lead allowance"},
                    {{"flat", {"pauschal 0,5 kV (230/400 V)", "flat 0.5 kV (230/400 V)"}},
                     {"lead", {"1 kV je m Anschlussleitung (Blitzstrom)", "1 kV per m of lead (lightning current)"}},
                     {"none", {"keiner (V-Verdrahtung)", "none (V-wiring)"}}},
                    "flat"),
        Num::number("a", {"Länge Anschlussleitungen a + b", "Length of leads a + b"}, "m", 0.5).nonNegative(),
        Num::choice("cat", {"Gerät (Überspannungskategorie)", "Equipment (overvoltage category)"},
                    {{"1", {"I – empfindliche Elektronik (1,5 kV)", "I – sensitive electronics (1.5 kV)"}},
                     {"2", {"II – Geräte, Steckdosen (2,5 kV)", "II – appliances, sockets (2.5 kV)"}},
                     {"3", {"III – feste Installation (4 kV)", "III – fixed installation (4 kV)"}},
                     {"4", {"IV – Einspeisung, Zähler (6 kV)", "IV – origin, meter (6 kV)"}}},
                    "2"),
        Num::number("l", {"Leitungslänge SPD → Gerät", "Cable length SPD → equipment"}, "m", 15).nonNegative(),
    };
    c.inputs[2].showIf("du", {"lead"});
    c.outputs = {Out::number("Upf", {"Wirksamer Schutzpegel U_p,f", "Effective protection level U_p/f"}, "kV", 2).primary(),
                 Out::number("Uw", {"Bemessungs-Stoßspannung U_W", "Rated impulse withstand voltage U_W"}, "kV", 1),
                 Out::number("ratio", {"U_p,f / U_W", "U_p/f / U_W"}, "%", 0),
                 Out::number("lpo", {"Schwingungsschutzbereich l_po", "Oscillation protective distance l_po"}, "m", 1).primary(),
                 Out::text("verdict", {"Bewertung", "Assessment"})};
    c.keywords = {"schutzbereich", "schutzpegel", "up", "uw", "bemessungsstoßspannung", "protective distance", "10 m"};
    c.compute = [](const Inputs& in, const Context&) {
        Result r;
        const auto& du = in.choice("du");
        const double dU = du == "flat" ? 0.5 : du == "lead" ? 1.0 * in.num("a") : 0.0;
        const double upf = in.num("Up") + dU;
        const std::string cat = in.choice("cat");
        const double uw = cat == "1" ? 1.5 : cat == "2" ? 2.5 : cat == "3" ? 4.0 : 6.0;
        const double l = in.num("l");
        r.set("Upf", upf);
        r.set("Uw", uw);
        r.set("ratio", upf / uw * 100.0);
        if (upf > uw) {
            r.set("lpo", 0.0);
            r.setText("verdict", {"Nicht geschützt: U_p,f > U_W – SPD mit kleinerem U_p oder zusätzliches Typ-3-Gerät.",
                                  "Not protected: U_p/f > U_W – SPD with lower U_p or an additional type 3 device."});
            r.error({"Wirksamer Schutzpegel über der Stoßfestigkeit des Geräts.", "Effective protection level above the equipment withstand."});
            return r;
        }
        const double lpo = (uw - upf) * 1000.0 / 25.0;  // k = 25 V/m
        r.set("lpo", lpo);
        if (upf <= 0.5 * uw) {
            r.setText("verdict", {"Geschützt – U_p,f ≤ 0,5 · U_W, Schutzbereich unabhängig von der Länge.",
                                  "Protected – U_p/f ≤ 0.5 · U_W, protection independent of the length."});
            r.ok({"Schwingungen vernachlässigbar.", "Oscillation negligible."});
        } else if (l <= 10 && upf <= 0.8 * uw) {
            r.setText("verdict", {"Geschützt – innerhalb des konventionellen Schutzbereichs (≤ 10 m, U_p,f ≤ 0,8 · U_W).",
                                  "Protected – within the conventional range (≤ 10 m, U_p/f ≤ 0.8 · U_W)."});
            r.ok({"Schutzbereich eingehalten.", "Protective range met."});
        } else if (l <= lpo) {
            r.setText("verdict", {"Geschützt – Leitungslänge ≤ l_po.", "Protected – cable length ≤ l_po."});
            r.ok({"Schutzbereich eingehalten.", "Protective range met."});
        } else {
            r.setText("verdict", {"Leitung länger als der Schutzbereich – weiteres SPD (Typ 2/3) nahe am Gerät vorsehen.",
                                  "Cable longer than the protective range – add an SPD (type 2/3) close to the equipment."});
            r.warn({"Schutzbereich überschritten.", "Protective range exceeded."});
        }
        return r;
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

Table lpzTable() {
    return make("lpz_zones", {"Blitzschutzzonen (LPZ) und Ableiter", "Lightning protection zones (LPZ) and arresters"},
                {"Zonen nach IEC 62305-4 und Schutzgerät an der Zonengrenze", "Zones per IEC 62305-4 and protective device at the boundary"},
                {"IEC 62305-4 / DIN EN 62305-4 (VDE 0185-305-4), Darstellung nach DEHN Blitzplaner. An jeder Zonengrenze werden alle "
                 "Leitungen in den Potentialausgleich einbezogen – direkt oder über SPD.",
                 "IEC 62305-4, presentation after the DEHN lightning protection guide. At every zone boundary all lines are bonded – "
                 "directly or via SPD."},
                {{"Zone", "Zone"}, {"Gefährdung", "Threat"}, {"Übergang", "Transition"}, {"Energietechnik", "Power"}, {"Informationstechnik", "IT / I&C"}},
                {{"LPZ 0A", {"direkter Einschlag, volles Blitzfeld", "direct strike, full lightning field"}, {"0A → 1", "0A → 1"},
                  {"Typ 1 bzw. Kombi-Ableiter Typ 1+2", "type 1 or combined type 1+2"}, {"Kombi-Ableiter, D1 (10/350 µs)", "combined arrester, D1 (10/350 µs)"}},
                 {"LPZ 0B", {"kein Direkteinschlag, volles Feld", "no direct strike, full field"}, {"0B → 1", "0B → 1"},
                  {"Typ 2", "type 2"}, {"Überspannungs-Ableiter C2", "surge arrester C2"}},
                 {"LPZ 1", {"Teilblitzströme und Überspannungen begrenzt", "partial currents and surges limited"}, {"1 → 2", "1 → 2"},
                  {"Typ 2", "type 2"}, {"Überspannungs-Ableiter C2", "surge arrester C2"}},
                 {"LPZ 2", {"weiter gedämpft (geschirmte Räume, Schränke)", "further attenuated (screened rooms, cabinets)"}, {"2 → 3", "2 → 3"},
                  {"Typ 3", "type 3"}, {"Feinschutz C1/C2", "fine protection C1/C2"}},
                 {"LPZ 3", {"Gerät/Baugruppe", "device/module"}, "–", "–", "–"}});
}

Table spdTypes() {
    return make("spd_types", {"SPD-Typen Energietechnik", "SPD types for power"}, {"Typ 1, 2, 3 – Prüfstrom, Einsatz, Beispiele", "Type 1, 2, 3 – test current, use, examples"},
                {"IEC 61643-11 / DIN EN 61643-11 (Typ = Klasse I/II/III bzw. T1/T2/T3). Energetische Koordination der Stufen nach "
                 "Herstellerangabe; Produktbeispiele DEHN (Kurzkatalog Elektrohandwerk 08/2023).",
                 "IEC 61643-11 (type = class I/II/III or T1/T2/T3). Energy coordination of the stages per manufacturer; DEHN examples "
                 "(short catalogue 08/2023)."},
                {{"Typ", "Type"}, {"Prüfung", "Test"}, {"Einsatz", "Use"}, {"Typische Werte", "Typical values"}, {"Beispiele DEHN", "DEHN examples"}},
                {{{"Typ 1 (Blitzstrom-Ableiter)", "type 1 (lightning current arrester)"}, {"I_imp 10/350 µs", "I_imp 10/350 µs"},
                  {"Einspeisung bei LPS/Freileitung (LPZ 0A → 1)", "origin with LPS/overhead line (LPZ 0A → 1)"},
                  {"12,5–25 kA je Pfad, U_p ≤ 1,5–2,5 kV", "12.5–25 kA per path, U_p ≤ 1.5–2.5 kV"}, {"DEHNbloc, DEHNgap, DEHNvenCI", "DEHNbloc, DEHNgap, DEHNvenCI"}},
                 {{"Typ 1+2 (Kombi-Ableiter)", "type 1+2 (combined arrester)"}, {"I_imp und I_n", "I_imp and I_n"},
                  {"Hauptverteilung, Zählerschrank", "main distribution, meter cabinet"}, {"U_p ≤ 1,5 kV, I_imp 12,5–25 kA je Pfad", "U_p ≤ 1.5 kV, I_imp 12.5–25 kA per path"},
                  {"DEHNventil, DEHNshield", "DEHNventil, DEHNshield"}},
                 {{"Typ 2 (Überspannungs-Ableiter)", "type 2 (surge arrester)"}, {"I_n 8/20 µs", "I_n 8/20 µs"},
                  {"Haupt- und Unterverteilung (LPZ 0B → 1, 1 → 2)", "main and sub-distribution (LPZ 0B → 1, 1 → 2)"},
                  {"I_n 20 kA, I_max 40 kA, U_p ≤ 1,5 kV", "I_n 20 kA, I_max 40 kA, U_p ≤ 1.5 kV"}, {"DEHNguard M/MP/S", "DEHNguard M/MP/S"}},
                 {{"Typ 3 (Geräteschutz)", "type 3 (device protection)"}, {"Kombiwelle 1,2/50 µs, 8/20 µs", "combination wave 1.2/50 µs, 8/20 µs"},
                  {"vor dem Endgerät (LPZ 2 → 3)", "ahead of the equipment (LPZ 2 → 3)"}, {"U_OC 6 kV, I_L 16–25 A", "U_OC 6 kV, I_L 16–25 A"},
                  {"DEHNrail, DEHNflex, DEHNsafe, DEHNprotector", "DEHNrail, DEHNflex, DEHNsafe, DEHNprotector"}}});
}

Table overvoltageCategories() {
    return make("overvoltage_categories", {"Überspannungskategorien und Bemessungs-Stoßspannung", "Overvoltage categories and rated impulse voltage"},
                {"U_W für 230/400 V nach Einbauort", "U_W for 230/400 V by location"},
                {"IEC 60664-1 / DIN VDE 0100-443 Tabelle 443.2 für Netze 230/400 V (120/208 V in Klammern). Das SPD muss U_p,f "
                 "unter U_W der zu schützenden Betriebsmittel halten.",
                 "IEC 60664-1 / IEC 60364-4-44 table 443.2 for 230/400 V systems (120/208 V in brackets). The SPD must keep U_p/f "
                 "below U_W of the equipment to be protected."},
                {{"Kategorie", "Category"}, {"U_W", "U_W"}, {"Betriebsmittel", "Equipment"}},
                {{"IV", "6 kV (4 kV)", {"am Speisepunkt: Zähler, Hauptsicherung, Rundsteuerempfänger", "at the origin: meter, main fuse, ripple control receiver"}},
                 {"III", "4 kV (2,5 kV)", {"feste Installation: Verteiler, Schalter, Steckdosen, Motoren", "fixed installation: boards, switches, sockets, motors"}},
                 {"II", "2,5 kV (1,5 kV)", {"Verbrauchsgeräte: Haushaltsgeräte, Werkzeuge", "appliances: household appliances, tools"}},
                 {"I", "1,5 kV (0,8 kV)", {"besonders schutzbedürftige Elektronik", "particularly sensitive electronics"}}});
}

Table spdConnection() {
    return make("spd_connection", {"SPD-Schaltungen je Netzsystem", "SPD connection per earthing system"}, {"3+0, 4+0, 3+1, 1+1 und U_C", "3+0, 4+0, 3+1, 1+1 and U_C"},
                {"DIN VDE 0100-534 / IEC 60364-5-53 (Mindest-U_C, Tabelle 534.2) und DEHN-Typbezeichnungen. 3+1/1+1: Varistoren L–N und "
                 "Summenfunkenstrecke N–PE (CT2); in TT-Systemen vor dem RCD üblich.",
                 "IEC 60364-5-53 (minimum U_C, table 534.2) and DEHN type designations. 3+1/1+1: varistors L–N and spark gap N–PE "
                 "(CT2); usual in TT systems upstream of the RCD."},
                {{"Netzsystem", "System"}, {"Schaltung", "Connection"}, {"U_C L–N / L–PE", "U_C L–N / L–PE"}, {"U_C N–PE", "U_C N–PE"}},
                {{"TN-C (3~)", "3+0", "≥ 253 V", "–"},
                 {"TN-S (3~)", {"4+0 oder 3+1", "4+0 or 3+1"}, "≥ 253 V", "≥ 230 V"},
                 {"TN-S (1~)", {"2+0 oder 1+1", "2+0 or 1+1"}, "≥ 253 V", "≥ 230 V"},
                 {"TT (3~)", "3+1", "≥ 253 V", "≥ 230 V"},
                 {"TT (1~)", "1+1", "≥ 253 V", "≥ 230 V"},
                 {{"IT ohne N (3~)", "IT without N (3~)"}, "3+0", {"L–PE ≥ U (400 V)", "L–PE ≥ U (400 V)"}, "–"}});
}

Table lightningParameters() {
    return make("lightning_parameters", {"Blitzschutzklassen: Blitzstrom und Fangeinrichtung", "Lightning protection levels: current and air-termination"},
                {"Höchst- und Mindestwerte, Blitzkugel, Maschenweite", "Maximum and minimum values, rolling sphere, mesh size"},
                {"IEC 62305-1 Tabelle 3/4 und IEC 62305-3 Tabelle 2 (DIN EN 62305). Höchstwerte bestimmen die Bemessung der Ableiter, "
                 "Mindestwerte den Fangradius. Gegen gültige Ausgabe prüfen.",
                 "IEC 62305-1 tables 3/4 and IEC 62305-3 table 2. Maximum values size the arresters, minimum values the air-termination "
                 "radius. Check against the edition in force."},
                {{"Klasse", "Level"}, {"I_max (10/350 µs)", "I_max (10/350 µs)"}, {"Ladung Q_kurz", "Charge Q_short"}, {"W/R", "W/R"},
                 {"I_min", "I_min"}, {"Blitzkugel", "Rolling sphere"}, {"Maschenweite", "Mesh size"}},
                {{"I", "200 kA", "100 C", "10 MJ/Ω", "3 kA", "20 m", "5 × 5 m"},
                 {"II", "150 kA", "75 C", "5,6 MJ/Ω", "5 kA", "30 m", "10 × 10 m"},
                 {"III", "100 kA", "50 C", "2,5 MJ/Ω", "10 kA", "45 m", "15 × 15 m"},
                 {"IV", "100 kA", "50 C", "2,5 MJ/Ω", "16 kA", "60 m", "20 × 20 m"}});
}

Table signalCategories() {
    return make("signal_spd_categories", {"Prüfkategorien für Datenleitungs-Ableiter", "Test categories for data line arresters"},
                {"IEC 61643-21: C1, C2, C3, D1", "IEC 61643-21: C1, C2, C3, D1"},
                {"IEC 61643-21 / DIN EN 61643-21 (Auswahl). D1 = Blitzstromtragfähigkeit für LPZ 0A → 1, C2 = Überspannung LPZ 0B → 1 "
                 "und höher. DEHN-Beispiel BLITZDUCTOR XT ML4 BE 24: D1 2,5 kA je Ader (10 kA gesamt), C2 10 kA je Ader. Gegen gültige "
                 "Ausgabe prüfen.",
                 "IEC 61643-21 (selection). D1 = lightning current capability for LPZ 0A → 1, C2 = surge for LPZ 0B → 1 and higher. "
                 "DEHN example BLITZDUCTOR XT ML4 BE 24: D1 2.5 kA per core (10 kA total), C2 10 kA per core. Check against the edition "
                 "in force."},
                {{"Kategorie", "Category"}, {"Prüfspannung", "Test voltage"}, {"Prüfstrom", "Test current"}, {"Einsatz", "Use"}},
                {{"C1", {"0,5–1 kV (1,2/50 µs)", "0.5–1 kV (1.2/50 µs)"}, {"0,25–0,5 kA (8/20 µs)", "0.25–0.5 kA (8/20 µs)"}, {"induktive Kopplung, Feinschutz", "inductive coupling, fine protection"}},
                 {"C2", {"2–10 kV (1,2/50 µs)", "2–10 kV (1.2/50 µs)"}, {"1–5 kA (8/20 µs)", "1–5 kA (8/20 µs)"}, {"Überspannung, LPZ 0B → 1 und höher", "surge, LPZ 0B → 1 and higher"}},
                 {"C3", {"≥ 1 kV (1 kV/µs)", "≥ 1 kV (1 kV/µs)"}, {"10–100 A (10/1000 µs)", "10–100 A (10/1000 µs)"}, {"Schutzpegel-Prüfung (steile Flanke)", "protection level test (steep front)"}},
                 {"D1", {"≥ 1 kV", "≥ 1 kV"}, {"0,5–2,5 kA (10/350 µs)", "0.5–2.5 kA (10/350 µs)"}, {"Blitzteilströme, LPZ 0A → 1", "partial lightning currents, LPZ 0A → 1"}}});
}

}  // namespace

void addSurge(std::vector<Calculator>& out) {
    out.push_back(spdPower());
    out.push_back(spdSignal());
    out.push_back(spdDistance());
}

void addSurgeTables(std::vector<Table>& out) {
    out.push_back(lpzTable());
    out.push_back(spdTypes());
    out.push_back(spdConnection());
    out.push_back(overvoltageCategories());
    out.push_back(lightningParameters());
    out.push_back(signalCategories());
}

}  // namespace elc::catalog
